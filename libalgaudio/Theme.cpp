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
#include "Theme.hpp"
#include <iostream>

namespace AlgAudio{

std::map<std::string, Color> Theme::thememap;
Color no_color(0,0,0);

void Theme::Init(){
  // A possible example theme
  thememap["bg-main"] = Color(0x424c5fff);
  thememap["bg-main-alt"] = Color(0x363f4fff);
  thememap["bg-button-positive"] = Color(0x5c786cff);
  thememap["bg-button-negative"] = Color(0x785c6aff);
  thememap["bg-button-neutral"] = Color(0x415a8aff);
  thememap["mg_main"] = Color(0x5c6878ff);
  thememap["text-generic"] = Color(0xaaaeb8ff);
  thememap["text-button"] = Color(0x212227ff);
  thememap["text-error"] = Color(0xdc8c96ff);
  thememap["progress-bar"] = Color(0x346d97ff);
}

const Color& Theme::Get(const std::string& id){
  auto it = thememap.find(id);
  if(it == thememap.end()){
    std::cout << "Warning: Theme " << id << " not found." << std::endl;
    return no_color; // reference to global static
  }
  return it->second;
}

}
