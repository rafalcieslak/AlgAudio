#include <iostream>
#include "LibLoader.hpp"
#include "ModuleCollection.hpp"
#include "SDLHandle.hpp"

using namespace AlgAudio;

int main(){
  try{
    SDLHandle sdlhandle;
    ModuleCollectionBase::InstallDir("modules");
    std::cout << ModuleCollectionBase::ListInstalledTemplates();
  }catch(CollectionParseException ex){
    std::cout << "Exception while loading the library: " << ex.what() << std::endl;
  }
}
