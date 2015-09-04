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
#include "Utilities.hpp"
#include "Module.hpp"
#include "ModuleTemplate.hpp"
#include <SDL2/SDL.h>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <unordered_map>
#ifdef __unix__
  #include <unistd.h>
#else
  #include <windows.h>
#endif

// #define SILLY_GDB

namespace AlgAudio {

static std::unordered_map<SDL_Keycode, std::pair<std::string, KeyData::KeyType>> keymap;

#ifdef __unix__
  const char Utilities::OSDirSeparator = '/';
  const std::string Utilities::OSLibSuffix = ".so";
#else
  const char Utilities::OSDirSeparator = '\\';
  const std::string Utilities::OSLibSuffix = ".dll";
#endif

void Utilities::Wait(int ms){
  SDL_Delay(ms);
}

void Utilities::WaitOS(int ms){
#ifdef __unix__
  usleep(ms*1000);
#else
  Sleep(ms);
#endif
}

bool Utilities::GetFileExists(std::string name)
{
  // For compilers that support C++14 experimental TS:
  // std::experimental::filesystem::exists(name);
  // For C++17:
  // std::filesystem::exists(name);
  return (bool)std::ifstream(name);
}

std::string Utilities::GetDir(std::string path){
  auto pos = path.rfind(OSDirSeparator);
  if(pos == std::string::npos){
    return std::string(".") + OSDirSeparator;
  }else{
    return path.substr(0,pos);
  }
}
std::string Utilities::GetFilename(std::string path){
  auto pos = path.rfind(OSDirSeparator);
  if(pos == std::string::npos){
    return path;
  }else{
    return path.substr(pos+1);
  }
}


std::string Utilities::GetCurrentDir(){
  char buffer[1000];
#ifdef __unix__
  getcwd(buffer,1000);
#else
  GetCurrentDirectory(1000,buffer);
#endif
  return std::string(buffer);
}

std::string Utilities::ConvertUnipathToOSPath(const std::string& unipath){
  std::string result = unipath;
  std::replace(result.begin(), result.end(), '/', OSDirSeparator);
  return result;
}
std::string Utilities::ConvertOSpathToUniPath(const std::string& ospath){
  std::string result = ospath;
  std::replace(result.begin(), result.end(), OSDirSeparator, '/');
  return result;
}

void Utilities::CopyToClipboard(std::string s){
  SDL_SetClipboardText(s.c_str());
}

std::vector<std::string> Utilities::SplitString(std::string str, std::string delimiter){
    std::vector<std::string> res;
    size_t pos = 0;
    std::string token;
    while ((pos = str.find(delimiter)) != std::string::npos) {
        token = str.substr(0, pos);
        res.push_back(token);
        str.erase(0, pos + delimiter.length());
    }
    res.push_back(str);
    return res;
}

std::string Utilities::JoinString(std::vector<std::string> str, std::string c){
	std::string buf = "";
	for(unsigned int i = 0; i < str.size(); i++){
		buf += str[i];
		if(i < str.size()-1) buf += c;
	}
	return buf;
}

void Utilities::Replace(std::string& str, const std::string& from, const std::string& to) {
    if(from.empty()) return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}

static inline std::string& ltrim(std::string& s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
  return s;
}

static inline std::string& rtrim(std::string& s) {
  s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
  return s;
}

static inline std::string& trim(std::string& s) {
  return ltrim(rtrim(s));
}

std::string Utilities::TrimAllLines(std::string s){
  std::vector<std::string> lines = SplitString(s, "\n");
  for(auto& line : lines)
    line = trim(line);
  std::string result = JoinString(lines,"\n");
  return trim(result); // Trim empty lines
}

Point2D Utilities::Align(HorizAlignment h, VertAlignment v, Size2D inner, Size2D outer){
  int x, y;

  if(h == HorizAlignment_LEFT) x = 0;
  else if(h == HorizAlignment_RIGHT) x = outer.width - inner.width;
  else if(h == HorizAlignment_CENTERED) x = outer.width/2 - inner.width/2;
  else x = 0;

  if(v == VertAlignment_TOP) y = 0;
  else if(v == VertAlignment_BOTTOM) y = outer.height - inner.height;
  else if(v == VertAlignment_CENTERED) y = outer.height/2 - inner.height/2;
  else y = 0;

  return Point2D(x,y);

}

std::string Utilities::PrettyFloat(float val){
  std::stringstream ss;
  //std::cout << "Float to prettify: " << val << std::endl;
  int s = (val == 0.0) ? 1 : floor(log10(fabs(val))) + 1;
  //std::cout << "s = " << s << std::endl;
  if(s < 0){
    float factor = pow(10.0f, s - 3);
    //std::cout << "factor = " << factor << std::endl;
    val = round(val/factor)*factor;
  }
  int precision = std::max(0, 3 - s);
  ss << std::setprecision(precision) << std::fixed << val;
  return ss.str();
}

KeyData::KeyData(const SDL_KeyboardEvent& k){
  SDL_Keycode kc = k.keysym.sym;
  shift = (k.keysym.mod & KMOD_LSHIFT) || (k.keysym.mod & KMOD_RSHIFT);
  ctrl = (k.keysym.mod & KMOD_LCTRL) || (k.keysym.mod & KMOD_RCTRL);
  alt = (k.keysym.mod & KMOD_LALT) || (k.keysym.mod & KMOD_RALT);
  auto it = keymap.find(kc);
  if(it == keymap.end()){
    symbol = "";
    type = KeyType::Unknown;
  }else{
    symbol = it->second.first;
    // If shift, uppercase the symbol.
    type = it->second.second;
  }
  repeat = k.repeat;
  pressed = k.state;
}

KeyData::KeyData(std::string s){
  type = KeyType::Text;
  symbol = s;
}

void KeyData::InitKeymap(){
  keymap[SDLK_0] = {"0", Digit}; keymap[SDLK_KP_0] = {"0", Digit};
  keymap[SDLK_1] = {"1", Digit}; keymap[SDLK_KP_1] = {"1", Digit};
  keymap[SDLK_2] = {"2", Digit}; keymap[SDLK_KP_2] = {"2", Digit};
  keymap[SDLK_3] = {"3", Digit}; keymap[SDLK_KP_3] = {"3", Digit};
  keymap[SDLK_4] = {"4", Digit}; keymap[SDLK_KP_4] = {"4", Digit};
  keymap[SDLK_5] = {"5", Digit}; keymap[SDLK_KP_5] = {"5", Digit};
  keymap[SDLK_6] = {"6", Digit}; keymap[SDLK_KP_6] = {"6", Digit};
  keymap[SDLK_7] = {"7", Digit}; keymap[SDLK_KP_7] = {"7", Digit};
  keymap[SDLK_8] = {"8", Digit}; keymap[SDLK_KP_8] = {"8", Digit};
  keymap[SDLK_9] = {"9", Digit}; keymap[SDLK_KP_9] = {"9", Digit};

  keymap[SDLK_RETURN] = {"\n", Return}; keymap[SDLK_RETURN2] = {"\n", Return};
  keymap[SDLK_BACKSPACE] = {"", Backspace};
  keymap[SDLK_DELETE] = {"", Delete};
  keymap[SDLK_RSHIFT] = {"RShift", Shift}; keymap[SDLK_LSHIFT] = {"LShift", Shift};
  keymap[SDLK_RCTRL] = {"RCtrl", Ctrl}; keymap[SDLK_LCTRL] = {"LCtrl", Ctrl};
  keymap[SDLK_RALT] = {"RAlt", Alt}; keymap[SDLK_LALT] = {"LAlt", Alt};
  
  keymap[SDLK_a] = {"a", Letter};
  keymap[SDLK_b] = {"c", Letter};
  keymap[SDLK_c] = {"c", Letter};
  keymap[SDLK_d] = {"d", Letter};
  keymap[SDLK_e] = {"e", Letter};
  keymap[SDLK_f] = {"f", Letter};
  keymap[SDLK_g] = {"g", Letter};
  keymap[SDLK_h] = {"h", Letter};
  keymap[SDLK_i] = {"i", Letter};
  keymap[SDLK_j] = {"j", Letter};
  keymap[SDLK_k] = {"k", Letter};
  keymap[SDLK_l] = {"l", Letter};
  keymap[SDLK_m] = {"m", Letter};
  keymap[SDLK_n] = {"n", Letter};
  keymap[SDLK_o] = {"o", Letter};
  keymap[SDLK_p] = {"p", Letter};
  keymap[SDLK_q] = {"q", Letter};
  keymap[SDLK_r] = {"r", Letter};
  keymap[SDLK_s] = {"s", Letter};
  keymap[SDLK_t] = {"t", Letter};
  keymap[SDLK_u] = {"u", Letter};
  keymap[SDLK_v] = {"v", Letter};
  keymap[SDLK_w] = {"w", Letter};
  keymap[SDLK_x] = {"x", Letter};
  keymap[SDLK_y] = {"y", Letter};
  keymap[SDLK_z] = {"z", Letter};
  
  keymap[SDLK_EQUALS] = {"=", Symbol};
  keymap[SDLK_MINUS] = {"-", Symbol};
  keymap[SDLK_KP_PLUS] = {"+", Symbol};
  keymap[SDLK_KP_MINUS] = {"-", Symbol};
  
  keymap[SDLK_ESCAPE] = {"Escape", Escape};
}


} // namespace AlgAudio
