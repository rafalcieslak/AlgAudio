#ifndef SCLANUCHER_HPP
#define SCLANUCHER_HPP
#include <string>
#ifndef __unix__
  #include <windows.h>
#else

#endif
#include "Utilities.hpp"

namespace AlgAudio{

struct SCLaunchException : public Exception{
  SCLaunchException(std::string t) : Exception(t) {}
};

class SCLauncher{
private:
  SCLauncher() = delete; // static class
public:
  static void Start();
  static void SendData(std::string);
  static std::string ReadData();
  static void Stop();
};

} // namespace AlgAudio

#endif // SCLANUCHER_HPP
