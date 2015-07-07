#include "Module.hpp"
#include <cstring>
#include <iostream>

class Base1 : public AlgAudio::Module{
public:
  void on_init() override;
};

extern "C"{
  void* create_instance(const char *);
}


void Base1::on_init(){
  std::cout << "Initing Base1!\n";
}

void* create_instance(const char* name){
  if(strcmp(name,"Base1")==0) return new Base1();
  else return nullptr;
}
