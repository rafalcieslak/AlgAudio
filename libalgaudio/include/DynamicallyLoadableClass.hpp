#ifndef DYNAMICALLY_LOADABLE_CLASS_HPP
#define DYNAMICALLY_LOADABLE_CLASS_HPP
/*
This file is part of AlgAudio.

AlgAudio, Copyright (C) 2015 CeTA - Audiovisual Technology Center

AlgAudio is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

AlgAudio is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with AlgAudio.  If not, see <http://www.gnu.org/licenses/>.
*/
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
