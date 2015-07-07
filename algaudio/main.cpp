#include "LibLoader.hpp"
#include "ModuleCollection.hpp"
#include <iostream>

using namespace AlgAudio;

int main(){
  try{
    ModuleCollectionBase::InstallFile("modules/example.xml");
    std::cout << ModuleCollectionBase::GetByID("example")->name << std::endl;
  }catch(CollectionParseException ex){
    std::cout << "Exception while loading the library: " << ex.what() << std::endl;
  }
}
