#include <iostream>
#include "LibLoader.hpp"
#include "ModuleCollection.hpp"
#include "SDLHandle.hpp"
#include "ModuleFactory.hpp"
#include "ModuleTemplate.hpp"

using namespace AlgAudio;

int main(){
  try{
    SDLHandle sdlhandle;
    ModuleCollectionBase::InstallDir("modules");
    std::cout << ModuleCollectionBase::ListInstalledTemplates();
    auto module1 = ModuleFactory::CreateNewInstance("debug/helloworld");
    auto module2 = ModuleFactory::CreateNewInstance("debug/pipe");
  }catch(Exception ex){
    std::cout << "An unhandled exception occured: " << ex.what() << std::endl;
  }
}
