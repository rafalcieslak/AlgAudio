#include "LibLoader.hpp"

LibLoader::LibLoader(std::string filename) : path(filename){
   hLib = LoadLibrary(filename.c_str());
   if(hLib == NULL) throw LibLoadingException(filename, "Failed to open library");
   create_instance_func = nullptr;
   create_instance_func = (create_instance_func_t*) GetProcAddress(hLib,"create_instance");
   if(create_instance_func == nullptr) throw LibLoadingException(filename, "Failed to get create_instance");
}

LibLoader::~LibLoader(){
  FreeLibrary(hLib);
}

Module* LibLoader::AskForInstance(std::string name){
  return reinterpret_cast<Module*>(create_instance_func(name.c_str()));
}
