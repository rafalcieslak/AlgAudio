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
#include <cmath>
#include "ModuleTemplate.hpp"
#include "SCLang.hpp"
#include "ModuleUI/StandardModuleGUI.hpp"

namespace AlgAudio{

Bus::Bus(int i) : id(i) {}

Bus::~Bus(){
  /// TODO: Ask SC to remove the bus
  // std::cout << "Bus freed" << std::endl;
}


ParamController::ParamController(std::shared_ptr<Module> m, const std::shared_ptr<ParamTemplate> t)
  : id(t->id), templ(t), range_min(t->default_min), range_max(t->default_max), module(m){

}
std::shared_ptr<ParamController> ParamController::Create(std::shared_ptr<Module> m, std::shared_ptr<ParamTemplate> templ){
  return std::shared_ptr<ParamController>(new ParamController(m,templ));
}

void ParamController::Set(float value){
  current_val = value;
  float relative = GetRelative();
  on_set.Happen(value, relative);
  auto m = module.lock();
  if(m){
    if(templ->action == ParamTemplate::ParamAction::SC){
      SCLang::SendOSC("/algaudioSC/setparam", "isf", m->sc_id, templ->id.c_str(), value);
    }else if(templ->action == ParamTemplate::ParamAction::Custom){
      m->on_param_set(templ->id, value);
    }else if(templ->action == ParamTemplate::ParamAction::None){
      // NOP
    }
  }
  after_set.Happen(value, relative);
}

void ParamController::SetRelative(float q){
  if(templ->scale == ParamTemplate::ParamScale::Linear){
    Set(range_min + q*(range_max - range_min));
  }else if(templ->scale == ParamTemplate::ParamScale::Logarithmic){
    Set(range_min * pow(range_max / range_min, q));
  }else{
    // ??? Wil not happen.
  }
}

float ParamController::GetRelative() const {
  if(templ->scale == ParamTemplate::ParamScale::Linear){
    return (current_val - range_min)/(range_max - range_min);
  }else if(templ->scale == ParamTemplate::ParamScale::Logarithmic){
    return log(current_val / range_min) / log(range_max / range_min);
  }else{
    return 0.0; // ??? Will not happen.
  }
}

void ParamController::Reset(){
  Set(templ->default_val);
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

SendReplyController::SendReplyController(std::shared_ptr<Module> m, std::string i, std::shared_ptr<ParamController> ctrl) : id(i), controller(ctrl), module(m){
}
std::shared_ptr<SendReplyController> SendReplyController::Create(std::shared_ptr<Module> m, std::string id, std::shared_ptr<ParamController> ctrl){
  auto res = std::shared_ptr<SendReplyController>( new SendReplyController(m, id, ctrl) );
  res->module_id = m->sc_id;
  res->sendreply_id = SCLang::RegisterSendReply(m->sc_id, res);
  // TODO: Specialize /setparam into /bindsendreply
  SCLang::SendOSC("/algaudioSC/setparam", "isi", m->sc_id, id.c_str(), res->sendreply_id);
  return res;
}
SendReplyController::~SendReplyController(){
  SCLang::UnregisterSendReply(module_id, sendreply_id);
}

std::shared_ptr<Module::Outlet> Module::Outlet::Create(std::string id, std::string name, std::shared_ptr<Module> mod){
  return std::shared_ptr<Module::Outlet>( new Module::Outlet(id, name, mod));
}

LateReturn<std::shared_ptr<Module::Inlet>> Module::Inlet::Create(std::string id, std::string name, std::shared_ptr<Module> mod, bool fake){
  auto r = Relay<std::shared_ptr<Module::Inlet>>::Create();

  if(fake){
    r.Return(std::shared_ptr<Module::Inlet>( new Module::Inlet(id, name, mod, Bus::CreateFake())));
    return r;
  }

  Bus::CreateNew().Then([=](std::shared_ptr<Bus> b)mutable{
    SCLang::SendOSC("/algaudioSC/connectinlet", "isi", mod->sc_id, id.c_str(), b->GetID());
    r.Return(std::shared_ptr<Module::Inlet>( new Module::Inlet(id, name, mod, b)));
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
  if(buses.size() == 0) m.add_int32(-1);
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
  for(auto iolettempl : templ->inlets){
    if(!fake){
      Inlet::Create(iolettempl.id,iolettempl.name,shared_from_this()).Then([=](std::shared_ptr<Inlet> inlet_ptr){
        inlets.emplace_back(inlet_ptr);
        s.Trigger();
      });
    }else{
      // Create a fake inlet
      Inlet::Create(iolettempl.id,iolettempl.name,shared_from_this(), true).Then([=](std::shared_ptr<Inlet> inlet_ptr){
        inlets.emplace_back(inlet_ptr);
        s.Trigger();
      });
    }
  }
  // Meanwhile, create outlets. These own no bus, so creation is instant.
  for(auto iolettempl : templ->outlets)
    outlets.emplace_back(Outlet::Create(iolettempl.id,iolettempl.name,shared_from_this()));
  s.WhenAll([=](){
    //std::cout << "All IO READY!" << std::endl;
    r.Return();
  });
  return r;
}


void Module::PrepareParamControllers(){
  for(const std::shared_ptr<ParamTemplate> ptr : templ->params){
    auto controller = ParamController::Create(shared_from_this(), ptr);
    param_controllers.push_back(controller);
  }
  for(auto reply_pair : templ->replies){
    auto c = GetParamControllerByID(reply_pair.second);
    if(!c){
      std::cout << "WARNING: Reply '" << reply_pair.first << "' has unexisting param '" << reply_pair.second << "'" << std::endl;
      continue;
    }
    auto replycontroller = SendReplyController::Create(shared_from_this(), reply_pair.first, c);
    reply_controllers.push_back(replycontroller);
  }
}
void Module::ResetControllers(){
  for(auto controller : param_controllers)
    controller->Reset();
}

/*
void Module::SetParam(std::string name, int value){
  SCLang::SendOSC("/algaudioSC/setparam", "isi", sc_id, name.c_str(), value);
}
void Module::SetParam(std::string name, std::list<int> values){
  lo::Message m;
  m.add_int32(sc_id);
  m.add_string(name);
  for(int i : values) m.add_int32(i);
  SCLang::SendOSCCustom("/algaudioSC/setparamlist", m);
}
void Module::SetParam(std::string name, double value){
  SCLang::SendOSC("/algaudioSC/setparam", "isd", sc_id, name.c_str(), value);
}
*/

std::shared_ptr<ModuleGUI> Module::GetGUI() const{
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

std::shared_ptr<ParamController> Module::GetParamControllerByID(std::string id) const{
  for(const auto& p : param_controllers){
    if(p->id == id) return p;
  }
  return nullptr;
}

} // namespace AlgAudio
