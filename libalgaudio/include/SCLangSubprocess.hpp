#ifndef SCLANGSUBPROCESS_HPP
#define SCLANGSUBPROCESS_HPP
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
#include <atomic>
#include <list>
#include <thread>
#include <memory>
#include <mutex>
#include "Subprocess.hpp"
#include "Signal.hpp"

namespace AlgAudio{

/** A wrapper for Subprocess, specialised in managing sclang{,.exe}. In
 *  particular, it can recognize comamnd prompt at SC output, pass events
 *  to main loop, and trigger some signals. */
class SCLangSubprocess{
public:
  /** Prepares to launch sclang with the given command. The process will not be
   *  started in the constructor. \see Start*/
  SCLangSubprocess(std::string command);
  ~SCLangSubprocess();
  /** Starts the managed sclang process and the subprocess I/O thread. \warning
   *  The thread cannot be started in the constructor.
   *  See https://rafalcieslak.wordpress.com/2014/05/16/c11-stdthreads-managed-by-a-designated-class/
   *  for details. */
  void Start();
  /** Stops sclang process. */
  void Stop();

  /** The I/O thread may not do any UI calls. They are all left for the main
   *  thread. The main thread should run the following method from time to time,
   *  it fires all signals that should. This way all subscribers are run by the
   *  main thread. */
  void TriggerSignals();

  Signal<std::string> on_any_line_received;
  Signal<> on_started;
  
  /** Sends an instruction to sclang subprocess (using its stdio). */
  void SendInstruction(std::string);
  /** Sends an instruction to sclang subprocess (using its stdio) and triggers
   *  the reply_action with the string that sclang outputted after sending that
   *  command and before the next prompt.*/
  void SendInstruction(std::string instruction, std::function<void(std::string)> reply_action);
private:
  std::atomic<bool> started, run;
  bool last_started = false; // Used by the MAIN thread only!
  std::string buffer;
  void ProcessBuffer();
  int prompts = 0;
  bool collecting_reply = false;
  std::string reply_buffer;

  std::string command;

  std::unique_ptr<Subprocess> subprocess;

  void ThreadMain();
  void Step();
  std::thread the_thread;
  std::recursive_mutex io_mutex;
  std::string WaitForReply(std::string);
  void WaitForPrompt();
  void SendInstructionRaw(std::string);
  void PollOutput();
  // std::atomic does not work with std::list, as it has no default noexcept constructor.
  std::list<std::string> lines_received;
  std::list<std::pair<std::string,std::function<void(std::string)>>> instructions_actions;
  //std::list<std::string> instructions;
  std::list<std::function<void()>> replies;
};

} // namespace AlgAudio

#endif // SCLANGSUBPROCESS_HPP
