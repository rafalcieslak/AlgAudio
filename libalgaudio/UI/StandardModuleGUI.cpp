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
#include "UI/StandardModuleGUI.hpp"

namespace AlgAudio{

std::shared_ptr<StandardModuleGUI> StandardModuleGUI::CreateEmpty(std::shared_ptr<Window> w){
  return std::shared_ptr<StandardModuleGUI>( new StandardModuleGUI(w) );
}
std::shared_ptr<StandardModuleGUI> StandardModuleGUI::CreateFromXML(std::shared_ptr<Window> w, std::string xml_data){
  auto ptr = std::shared_ptr<StandardModuleGUI>( new StandardModuleGUI(w) );
  ptr->LoadFromXML(xml_data);
  return ptr;
}

void StandardModuleGUI::LoadFromXML(std::string xml_data){
  std::cout << "Loading data from XML " << std::endl;
}

void StandardModuleGUI::CustomDraw(DrawContext& c){

}

}
