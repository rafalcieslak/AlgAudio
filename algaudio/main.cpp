#include "LibLoader.hpp"
#include <iostream>

int main(){
  try{
    LibLoader ll("modules_base.aa.dll");
    Module* m1 = ll.AskForInstance("Base1");
    m1->on_init();
  }catch(LibLoadingException ex){
    std::cout << "Exception while loading the library: " << ex.text << std::endl;
  }
}
