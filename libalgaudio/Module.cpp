/*
This file is part of AlgAudio.

AlgAudio, Copyright (C) 2015 CeTA - Audiovisual Technology Center

AlgAudio is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

AlgAudio is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with AlgAudio.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "Module.hpp"
#include <algorithm>
#include "ModuleTemplate.hpp"
#include "SCLang.hpp"
#include "UI/StandardModuleGUI.hpp"

namespace AlgAudio{

Bus::Bus(int i) : id(i) {}

Bus::~Bus(){
  /// TODO: Ask SC to remove the bus
  std::cout << "Bus freed" << std::endl;
}

std::shared_ptr<ParramController> ParramController::Create(std::shared_ptr<Module> m, std::shared_ptr<ParramTemplate> templ){
  return std::shared_ptr<ParramController>(new ParramController(m,templ));
}

void ParramController::Set(float value){
  current_val = value;
  on_set.Happen(value);
}

LateReturn<std::shared_ptr<Bus>> Bus::CreateNew(){
  auto r = Relay<std::shared_ptr<Bus>>::Create();
  SCLang::SendOSCWithReply<int>("/algaudioSC/newbus").Then( [r](int id){
    r.Return( std::shared_ptr<Bus>(new Bus(id)) );
  });
  return r;
}
std::shared_ptr<Bus> Bus::CreateFake(){
  return std::shared_ptr<Bus>(new Bus(-42));
}

std::shared_ptr<Module::Outlet> Module::Outlet::Create(std::string id, std::shared_ptr<Module> mod){
  return std::shared_ptr<Module::Outlet>( new Module::Outlet(id, mod));
}

LateReturn<std::shared_ptr<Module::Inlet>> Module::Inlet::Create(std::string id, std::shared_ptr<Module> mod, bool fake){
  auto r = Relay<std::shared_ptr<Module::Inlet>>::Create();

  if(fake){
    r.Return(std::shared_ptr<Module::Inlet>( new Module::Inlet(id, mod, Bus::CreateFake())));
    return r;
  }

  Bus::CreateNew().Then([=](std::shared_ptr<Bus> b)mutable{
    SCLang::SendOSC("/algaudioSC/connectinlet", "isi", mod->sc_id, id.c_str(), b->GetID());
    r.Return(std::shared_ptr<Module::Inlet>( new Module::Inlet(id, mod, b)));
  });
  return r;
}

void Module::Outlet::ConnectToInlet(std::shared_ptr<Module::Inlet> i){
  buses.push_back(i->bus);
  SendConnections();
}
void Module::Outlet::DetachFromInlet(std::shared_ptr<Module::Inlet> i){
  auto b = i->bus;
  buses.remove_if([b](std::weak_ptr<Bus> p){
    std::shared_ptr<Bus> a = p.lock();
    if(a) return a == b;
    return false;
  });
  SendConnections();
}
void Module::Outlet::DetachFromAll(){
  buses.clear();
  SendConnections();
}
void Module::Outlet::SendConnections(){
  lo::Message m;
  m.add_int32(mod.sc_id);
  m.add_string(id);
  for(auto& b : buses) m.add_int32(b.lock()->GetID());
  if(buses.size() == 0) m.add_int32(99999999);
  SCLang::SendOSCCustom("/algaudioSC/connectoutlet", m);
}


Module::~Module() {
  if(enabled_by_factory) std::cout << "WARNING: a module " << templ->GetFullID() << " reference is lost, but it was not destroyed by the factory." << std::endl;
};

void Module::Connect(std::shared_ptr<Module::Outlet> o, std::shared_ptr<Module::Inlet> i){
  o->ConnectToInlet(i);
}

LateReturn<> Module::CreateIOFromTemplate(bool fake){
  auto r = Relay<>::Create();
  Sync s(templ->inlets.size());
  for(std::string id : templ->inlets){
    if(!fake){
      Inlet::Create(id,shared_from_this()).Then([=](std::shared_ptr<Inlet> inlet_ptr){
        inlets.emplace_back(inlet_ptr);
        s.Trigger();
      });
    }else{
      // Create a fake inlet
      Inlet::Create(id,shared_from_this(), true).Then([=](std::shared_ptr<Inlet> inlet_ptr){
        inlets.emplace_back(inlet_ptr);
        s.Trigger();
      });
    }
  }
  // Meanwhile, create outlets. These own no bus, so creation is instant.
  for(std::string id : templ->outlets)
    outlets.emplace_back(Outlet::Create(id,shared_from_this()));
  s.WhenAll([=](){
    //std::cout << "All IO READY!" << std::endl;
    r.Return();
  });
  return r;
}


void Module::PrepareParramControllers(){
  for(const std::shared_ptr<ParramTemplate> ptr : templ->parrams){
    auto controller = ParramController::Create(shared_from_this(), ptr);
    controller->Set(ptr->default_val);
    parram_controllers.push_back(controller);
    // TODO: custom defined actions on controller set
    subscriptions += controller->on_set.Subscribe([this, parramid = ptr->id](float val){
      SCLang::SendOSC("/algaudioSC/setparram", "isf", sc_id, parramid.c_str(), val);
    });
  }
}

/*
void Module::SetParram(std::string name, int value){
  SCLang::SendOSC("/algaudioSC/setparram", "isi", sc_id, name.c_str(), value);
}
void Module::SetParram(std::string name, std::list<int> values){
  lo::Message m;
  m.add_int32(sc_id);
  m.add_string(name);
  for(int i : values) m.add_int32(i);
  SCLang::SendOSCCustom("/algaudioSC/setparramlist", m);
}
void Module::SetParram(std::string name, double value){
  SCLang::SendOSC("/algaudioSC/setparram", "isd", sc_id, name.c_str(), value);
}
*/

std::shared_ptr<ModuleGUI> Module::GetGUI(){
  return modulegui.lock();
}

std::shared_ptr<ModuleGUI> Module::BuildGUI(std::shared_ptr<Window> parent_window){
  std::shared_ptr<ModuleGUI> gui;
  if(templ->guitype == "standard"){
    gui = StandardModuleGUI::CreateFromXML(parent_window, templ->guitree, shared_from_this());
  }else if(templ->guitype == "standard auto"){
    gui = StandardModuleGUI::CreateFromTemplate(parent_window, shared_from_this());
  }else if(templ->guitype == ""){
    throw GUIBuildException("This module has no gui defined");
  }else{
    throw GUIBuildException("Module gui type '" + templ->guitype + "' was not recognized");
  }
  modulegui = gui;
  on_gui_build(gui);
  return gui;
}

std::shared_ptr<Module::Inlet> Module::GetInletByID(std::string id) const{
  for(auto& i : inlets)
    if(i->id == id) return i;
  return nullptr;
}
std::shared_ptr<Module::Outlet> Module::GetOutletByID(std::string id) const{
  for(auto& o : outlets)
    if(o->id == id) return o;
  return nullptr;
}

} // namespace AlgAudio
