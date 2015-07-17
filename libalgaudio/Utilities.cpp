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
#ifdef __unix__
  #include <unistd.h>
#else
  #include <windows.h>
#endif

namespace AlgAudio {

#ifdef __unix__
  const char Utilities::OSDirSeparator = '/';
  const std::string Utilities::OSLibSuffix = ".so";
#else
  const char Utilities::OSDirSeparator = '\\';
  const std::string Utilities::OSLibSuffix = ".dll";
#endif

Signal<> Utilities::global_idle;

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

std::string Utilities::ConvertUnipathToOSPath(const std::string& unipath){
  std::string result = unipath;
  std::replace(result.begin(), result.end(), '/', OSDirSeparator);
  return result;
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

} // namespace AlgAudio
