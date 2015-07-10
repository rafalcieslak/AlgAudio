#ifndef SCLANUCHER_HPP
#define SCLANUCHER_HPP
#include <string>
#ifndef __unix__
  #include <windows.h>
#else

#endif
#include "Utilities.hpp"

namespace AlgAudio{

struct SubprocessException : public Exception{
  SubprocessException(std::string t) : Exception(t) {}
};

class Subprocess{
public:
  Subprocess(std::string command);
  Subprocess(const Subprocess&) = delete;
  ~Subprocess();
  Subprocess operator=(const Subprocess&) = delete;
  void SendData(const std::string&);
  std::string ReadData();
private:
  std::string command;
  #ifdef __unix__

  #else
    PROCESS_INFORMATION piProcInfo;
    HANDLE g_hChildStd_IN_Rd = NULL;
    HANDLE g_hChildStd_IN_Wr = NULL;
    HANDLE g_hChildStd_OUT_Rd = NULL;
    HANDLE g_hChildStd_OUT_Wr = NULL;
  #endif
};

} // namespace AlgAudio

#endif // SCLANUCHER_HPP
