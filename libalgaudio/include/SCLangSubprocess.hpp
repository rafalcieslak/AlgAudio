#ifndef SCLANGSUBPROCESS_HPP
#define SCLANGSUBPROCESS_HPP
#include <atomic>
#include <list>
#include <thread>
#include <memory>
#include <mutex>
#include "Subprocess.hpp"
#include "Signal.hpp"

namespace AlgAudio{

class SCLangSubprocess{
public:
  SCLangSubprocess(std::string command);
  ~SCLangSubprocess();
  // The thread cannot be started in the constructor.
  // See https://rafalcieslak.wordpress.com/2014/05/16/c11-stdthreads-managed-by-a-designated-class/
  // for details.
  void Start();
  void Stop();

  // The I/O thread may not do any UI calls. They are all left for the main
  // thread. The main thread should run the following method from time to time,
  // it fires all signals that should. This way all subscribers are run by the
  // main thread.
  void TriggerSignals();

  Signal<std::string> on_any_line_received;
  Signal<> on_started;
  void SendInstruction(std::string);
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
  std::list<std::string> instructions;
  std::list<std::function<void()>> replies;
};

} // namespace AlgAudio

#endif // SCLANGSUBPROCESS_HPP
