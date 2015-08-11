#ifndef SCLANUCHER_HPP
#define SCLANUCHER_HPP
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
#ifdef __unix__

#else
  #include <windows.h>
#endif
#include "Utilities.hpp"

namespace AlgAudio{

struct SubprocessException : public Exception{
  SubprocessException(std::string t) : Exception(t) {}
};

/* A universal class for launching and terminating subprocesses, as well as
 * communicating with them via stdio.
 */
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
    int pipe_child_stdout_fd[2];
    int pipe_child_stdin_fd[2];
    int pid;
  #else
    PROCESS_INFORMATION piProcInfo;
    HANDLE g_hChildStd_IN_Rd = NULL;
    HANDLE g_hChildStd_IN_Wr = NULL;
    HANDLE g_hChildStd_OUT_Rd = NULL;
    HANDLE g_hChildStd_OUT_Wr = NULL;
    static HANDLE job; // = NULL
  #endif
};

} // namespace AlgAudio

#endif // SCLANUCHER_HPP
