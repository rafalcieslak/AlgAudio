#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP
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

#include <string>

namespace AlgAudio{
  
namespace Exceptions{
  
class Exception{
public:
  Exception(std::string t) : text(t){
#ifdef SILLY_GDB
    // if your GDB cannot break
    std::cout << "Builtin trap to mark exception creation stack for exception: `" << text << "`" << std::endl;
    // *((int*)nullptr) = 0;
    __builtin_trap();
#endif // SILLY_GDB
  }
  virtual std::string what() {return text;}
  virtual ~Exception() {}
protected:
  std::string text;
};

struct Unimplemented : public Exception{
  Unimplemented(std::string t) : Exception(t){}
};

} // namespace Exceptions
} // namespcae AlgAudio

#endif // EXCEPTION_HPP
