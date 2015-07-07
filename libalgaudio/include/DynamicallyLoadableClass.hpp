#ifndef DYNAMICALLY_LOADABLE_CLASS_HPP
#define DYNAMICALLY_LOADABLE_CLASS_HPP
#include <iostream>

namespace AlgAudio{

class DynamicallyLoadableClass{
  void (*deleter)(void*) = nullptr;
public:
  DynamicallyLoadableClass(void (*d)(void*)) : deleter(d) {}
  DynamicallyLoadableClass(){}
  void SetDeleter(void (*d)(void*)){ deleter = d; }
  void SelfDestruct(){
    if(deleter == nullptr){
      // This case shouldn't happen. Local objects do not use SelfDestruct.
      // std::cout << "Deleting a locally created object. " << std::endl;
      delete this;
    }else{
      // std::cout << "Deleting a foreign object. " << std::endl;
      deleter(this);
    }
  };
  virtual ~DynamicallyLoadableClass(){};
};

} //namespace AlgAudio
#endif //DYNAMICALLY_LOADABLE_CLASS_HPP
