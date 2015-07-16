#include "Module.hpp"
#include <cstring>
#include <iostream>

// Empty ATM

extern "C"{
void delete_instance(void* obj){
  delete reinterpret_cast<AlgAudio::DynamicallyLoadableClass*>(obj);
}
void* create_instance(const char* name){
   return nullptr;
}

} // extern C
