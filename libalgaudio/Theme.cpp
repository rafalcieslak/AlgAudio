#include "Theme.hpp"
#include <iostream>

namespace AlgAudio{

std::map<std::string, Color> Theme::thememap;
Color no_color(0,0,0);

void Theme::Init(){
  // A possible example theme
  thememap["bg-main"] = Color(0x3f4752ff);
  thememap["bg-button-positive"] = Color(0x5c786cff);
  thememap["bg-button-negative"] = Color(0x785c6aff);
  thememap["bg-button-neutral"] = Color(0x3d4f67ff);
  thememap["mg_main"] = Color(0x5c6878ff);
  thememap["text-generic"] = Color(0xaaaeb8ff);
  thememap["text-button"] = Color(0x212227ff);
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
