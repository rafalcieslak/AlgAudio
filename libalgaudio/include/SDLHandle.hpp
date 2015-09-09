#ifndef SDLHANDLE_HPP
#define SDLHANDLE_HPP
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

#include "Utilities.hpp"

namespace AlgAudio{

namespace Exceptions{
struct SDLException : public Exception{
  SDLException(std::string t);
};
struct SDLExceptionNoDesc : public Exception{
  SDLExceptionNoDesc(std::string t) : Exception(t) {}
};
} // namesace Exceptions

/** A wrapper class for RAII SDL init/cleanup.
 *  All objects that require access to SDL should aquire such handle.
 *  Most importantly, all top-level independend windows need to keep one.
 *  Also: modules which use SDL to e.g. get mouse position should also take one.
*/
class SDLHandle{
private:
  static int reference_counter;
public:
  SDLHandle();
  ~SDLHandle();
  static int GetReferenceCount() {return reference_counter;}
};

} // namespace AlgAudio
#endif // SDLHANDLE_HPP
