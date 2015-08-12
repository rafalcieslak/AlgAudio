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
  //thememap["bg-button-neutral"] = Color(0x3d4f67ff);
  thememap["mg_main"] = Color(0x5c6878ff);
  thememap["text-generic"] = Color(0xaaaeb8ff); // White
  //thememap["text-generic"] = Color(0x1b2737ff); // Dark blue
  thememap["text-button"] = Color(0x212227ff);
  thememap["text-error"] = Color(0xdc8c96ff);
  thememap["progress-bar"] = Color(0x346d97ff);
  thememap["selector-button-normal"] = Color(0x52607cff);
  thememap["selector-button-highlight"] = Color(0x415a8aff);

  thememap["standardbox-bg"] = Color(0x3e4f6cff);
  thememap["standardbox-border"] = Color(0x293b53ff);
  // thememap["standardbox-caption"] = Color(0x94acccff); // Whiteish blue
  thememap["standardbox-caption"] = Color(0x070a0eff); // Blueish black
  thememap["standardbox-inlet"] = Color(0x52607cff);
  thememap["standardbox-outlet"] = Color(0x52607cff);

  thememap["bg-alert"] = Color(0x52607cff);
  thememap["alert-border-none"] = Color(0x52607cff);
  thememap["alert-border-info"] = Color(0x3a945eff);
  thememap["alert-border-warning"] = Color(0x94913aff);
  thememap["alert-border-error"] = Color(0x943a3aff);

  thememap["canvas-connection-audio"] = Color(0x1b2737ff);
  thememap["canvas-connection-data"] = Color(0x2c104eff);
  thememap["canvas-connection-new"] = Color(0x37a025ff);
  thememap["canvas-connection-remove"] = Color(0xb92b2bff);

  thememap["slider-bg"] = Color(0x5a6e8dff);
  thememap["slider-bg-display"] = Color(0x475872ff);
  thememap["slider-border"] = Color(0x293b53ff);
  thememap["slider-marker"] = Color(0x063373ff);
  thememap["slider-connector"] = Color(0x666d96ff);
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
