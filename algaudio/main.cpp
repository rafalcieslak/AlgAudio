#include "LibLoader.hpp"
#include "ModuleCollection.hpp"
#include <iostream>

using namespace AlgAudio;

int main(){
  /*
  try{
    LibLoader ll("modules_base.aa.dll");
    Module* m1 = ll.AskForInstance("Base1");
    m1->on_init();
  }catch(LibLoadingException ex){
    std::cout << "Exception while loading the library: " << ex.text << std::endl;
  }
  */

  try{
    std::ifstream file("modules/example.xml");
    ModuleCollection col(file);
    std::cout << col.name << std::endl;
  }catch(CollectionParseException ex){
    std::cout << "Exception while loading the library: " << ex.what() << std::endl;
  }
}
