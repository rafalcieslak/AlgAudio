#ifndef SDLHANDLE_HPP
#define SDLHANDLE_HPP

#include "Utilities.hpp"

namespace AlgAudio{

struct SDLException : public Exception{
  SDLException(std::string t);
};

/* A wrapper class for RAII SDL init/cleanup.
   All objects that require access to SDL should aquire such handle.
   Most importantly, all top-level independend windows need to keep one.
   Also: modules which use SDL to e.g. get mouse position should also take one.
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
