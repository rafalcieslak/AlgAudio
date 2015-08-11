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
#include "LibLoader.hpp"
#include <iostream>

#ifdef __unix__
  #include <dlfcn.h>
#endif

namespace AlgAudio{

std::map<std::string, std::shared_ptr<LibLoader>> LibLoader::libs_by_path;

LibLoader::LibLoader(std::string filename) : path(filename){
#ifdef __unix__
  handle = dlopen(filename.c_str(), RTLD_NOW);
  if(handle == NULL) throw LibLoadingException(filename, "Failed to open library");
  create_instance_func = (create_instance_func_t*) dlsym(handle,"create_instance");
  deleter_func = (deleter_t*) dlsym(handle,"delete_instance");
#else
  hLib = LoadLibrary(filename.c_str());
  if(hLib == NULL) throw LibLoadingException(filename, "Failed to open library");
  create_instance_func = (create_instance_func_t*) GetProcAddress(hLib,"create_instance");
  deleter_func = (deleter_t*) GetProcAddress(hLib,"delete_instance");
#endif
  if(create_instance_func == nullptr) throw LibLoadingException(filename, "Failed to get create_instance");
  if(deleter_func == nullptr) throw LibLoadingException(filename, "Failed to get delete_instance");
}

LibLoader::~LibLoader(){
#ifdef __unix__
  dlclose(handle);
#else
  FreeLibrary(hLib);
#endif
}

Module* LibLoader::AskForInstance(std::string name){
  auto ptr = reinterpret_cast<Module*>(create_instance_func(name.c_str()));
  if(ptr == nullptr) return nullptr;
  ptr->SetDeleter(deleter_func);
  return ptr;
}

std::shared_ptr<LibLoader> LibLoader::GetByPath(std::string path){
  auto it = libs_by_path.find(path);
  if(it == libs_by_path.end()){
    std::cout << "Library " << path << " was not used till now, loading." << std::endl;
    return Preload(path);
  }
  return it->second;
}

std::shared_ptr<LibLoader> LibLoader::Preload(std::string path){
  auto lib_ptr = std::shared_ptr<LibLoader>( new LibLoader(path) );
  libs_by_path[path] = lib_ptr;
  return lib_ptr;
}

} // namespace AlgAudio
