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
#include "Canvas.hpp"
#include "ModuleFactory.hpp"
#include <algorithm>

namespace AlgAudio{

Canvas::Canvas(){

}

std::shared_ptr<Canvas> Canvas::CreateEmpty(){
  return std::shared_ptr<Canvas>( new Canvas() );
}

LateReturn<std::shared_ptr<Module>> Canvas::CreateModule(std::string id){
  Relay<std::shared_ptr<Module>> r;
  ModuleFactory::CreateNewInstance(id).Then([this,r](std::shared_ptr<Module> m){
    modules.emplace(m);
    m->canvas = shared_from_this();
    r.Return(m);
  });
  return r;
}

void Canvas::RemoveModule(std::shared_ptr<Module> m){
  if(!m) std::cout << "WARNING: Canvas asked to remove module (nullptr) " << std::endl;
  // First, remove all connections that start at this module.
  for(std::string &outid : m->templ->outlets){
    auto it = connections.find(IOID{m, outid});
    if(it != connections.end()){
      GetOutletByIOID(IOID{m, outid})->DetachFromAll();
      connections.erase(it);
    }
  }
  // Next, erase all connections that end at this module.
  for(std::string &inid : m->templ->inlets){
    for(auto& it : connections){
      IOID from = it.first;
      auto from_outlet = GetOutletByIOID(from);
      std::list<IOID>& tolist = it.second;
      for(IOID &to : tolist)
        if(to.iolet == inid)
          from_outlet->DetachFromInlet(GetInletByIOID(to));
      tolist.remove(IOID{m,inid});
    }
  }

  // Then, erase the module and destroy it via ModuleFactory.
  modules.erase(m);
  ModuleFactory::DestroyInstance(m);
}

std::shared_ptr<Module::Inlet>  Canvas::GetInletByIOID(IOID i) const{
  return i.module->GetInletByID(i.iolet);
}
std::shared_ptr<Module::Outlet>  Canvas::GetOutletByIOID(IOID i) const{
  return i.module->GetOutletByID(i.iolet);
}

void Canvas::Connect(IOID from, IOID to){
  std::shared_ptr<Module::Outlet> outlet = GetOutletByIOID(from);
  std::shared_ptr<Module::Inlet >  inlet = GetInletByIOID (to);
  if(!(inlet) || !(outlet)){
    std::cout << "WARNING: Invalid connection between unexisting inlet/outlet." << std::endl;
    return;
  }

  // TODO: check if creating connection does not create a loop!

  std::cout << "Connecting" << std::endl;
  outlet->ConnectToInlet(inlet);

  auto it = connections.find(from);
  if(it == connections.end()){
    // First connection from this inlet
    std::list<IOID> tmp;
    tmp.push_back(to);
    connections[from] = tmp;
  }else{
    it->second.push_back(to);
  }

}

Canvas::~Canvas(){
  for(auto& m : modules) ModuleFactory::DestroyInstance(m);
}

} // namespace AlgAudio
