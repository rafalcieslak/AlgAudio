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
#include "ModuleTemplate.hpp"
#include "SCLang.hpp"

namespace AlgAudio{

Bus::Bus(int i) : id(i) {}

Bus::~Bus(){
  /// TODO: Ask SC to remove the bus
}

LateReturn<std::shared_ptr<Bus>> Bus::CreateNew(){
  auto r = Relay<std::shared_ptr<Bus>>::Create();
  SCLang::SendOSCWithReply<int>("/algaudioSC/newbus").Then( [r](int id){
    r.Return( std::shared_ptr<Bus>(new Bus(id)) );
  });
  return r;
}

std::shared_ptr<Module::Outlet> Module::Outlet::Create(std::string id, Module& mod){
  return std::shared_ptr<Module::Outlet>( new Module::Outlet(id, mod));
}

LateReturn<std::shared_ptr<Module::Inlet>> Module::Inlet::Create(std::string id, Module& mod){
  auto r = Relay<std::shared_ptr<Module::Inlet>>::Create();
  Bus::CreateNew().Then([=](std::shared_ptr<Bus> b)mutable{
    mod.SetParram(id, b->GetID());
    r.Return(std::shared_ptr<Module::Inlet>( new Module::Inlet(id, mod, b)));
  });
  return r;
}

void Module::Connect(std::shared_ptr<Module::Outlet> o, std::shared_ptr<Module::Inlet> i){
  o->bus = i->bus;
  o->mod.SetParram(o->id, i->bus->GetID());
}

LateReturn<> Module::CreateIOFromTemplate(){
  auto r = Relay<>::Create();
  Sync s(templ->inlets.size());
  for(std::string id : templ->inlets){
    Inlet::Create(id,*this).Then([=](std::shared_ptr<Inlet> inlet_ptr){
      inlets.emplace_back(inlet_ptr);
      s.Trigger();
    });
  }
  // Meanwhile, create outlets. These own no bus, so creation is instant.
  for(std::string id : templ->outlets)
    outlets.emplace_back(Outlet::Create(id,*this));
  s.WhenAll([=](){
    std::cout << "All IO READY!" << std::endl;
    r.Return();
  });
  return r;
}

void Module::SetParram(std::string name, int value){
  SCLang::SendOSC("/algaudioSC/setparram", "isi", sc_id, name.c_str(), value);
}
void Module::SetParram(std::string name, double value){
  SCLang::SendOSC("/algaudioSC/setparram", "isd", sc_id, name.c_str(), value);
}

} // namespace AlgAudio
