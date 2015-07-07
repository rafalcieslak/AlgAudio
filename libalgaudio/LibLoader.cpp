#include "LibLoader.hpp"
#include <iostream>

namespace AlgAudio{

std::map<std::string, std::shared_ptr<LibLoader>> LibLoader::libs_by_path;

LibLoader::LibLoader(std::string filename) : path(filename){
   hLib = LoadLibrary(filename.c_str());
   if(hLib == NULL) throw LibLoadingException(filename, "Failed to open library");
   create_instance_func = nullptr;
   create_instance_func = (create_instance_func_t*) GetProcAddress(hLib,"create_instance");
   if(create_instance_func == nullptr) throw LibLoadingException(filename, "Failed to get create_instance");
   deleter_func = (deleter_t*) GetProcAddress(hLib,"delete_instance");
   if(deleter_func == nullptr) throw LibLoadingException(filename, "Failed to get delete_instance");
   std::cout << "Loaded" << std::endl;
}

LibLoader::~LibLoader(){
  FreeLibrary(hLib);
}

Module* LibLoader::AskForInstance(std::string name){
  auto ptr = reinterpret_cast<Module*>(create_instance_func(name.c_str()));
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
  auto lib_ptr = std::make_shared<LibLoader>(path);
  libs_by_path[path] = lib_ptr;
  return lib_ptr;
}

} // namespace AlgAudio
