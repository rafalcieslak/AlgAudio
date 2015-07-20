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

void Module::AddInlet(std::string s){
  inlets.emplace_back(Inlet(s,*this));
}
void Module::AddOutlet(std::string s){
  outlets.emplace_back(Outlet(s,*this));
}

void Module::CreateIOFromTemplate(){
  for(std::string id : templ->inlets) AddInlet(id);
  for(std::string id : templ->outlets) AddOutlet(id);
}

void Module::SetParram(std::string name, int value){
  SCLang::SendOSC("/algaudioSC/setparram", "isi", sc_id, name.c_str(), value);
}

} // namespace AlgAudio
