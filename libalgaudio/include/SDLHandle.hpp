#ifndef SDLHANDLE_HPP
#define SDLHANDLE_HPP

#include "Utilities.hpp"

namespace AlgAudio{

struct SDLInitException : public Exception{
  SDLInitException(std::string t) : Exception(t) {}
};

// A wrapper class for RAII SDL init/cleanup
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
