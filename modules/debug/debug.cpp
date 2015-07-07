#include "Module.hpp"
#include <iostream>
#include <cstring>

class HelloWorld : public AlgAudio::Module{
public:
  void on_init(){
    std::cout << "Hello world!" << std::endl;
  }
};

extern "C"{
void* create_instance(const char* name){
  if(strcmp(name,"HelloWorld")==0) return new HelloWorld();
  else return nullptr;
}
} // extern C
