#include <iostream>
#include "LibLoader.hpp"
#include "ModuleCollection.hpp"
#include "SDLHandle.hpp"

using namespace AlgAudio;

int main(){
  try{
    SDLHandle sdlhandle;
    ModuleCollectionBase::InstallFile("modules/example.xml");
    ModuleCollectionBase::InstallFile("modules/debug.xml");
    std::cout << ModuleCollectionBase::ListInstalledTemplates();
  }catch(CollectionParseException ex){
    std::cout << "Exception while loading the library: " << ex.what() << std::endl;
  }
}
