#ifndef SCLANGSUBPROCESS_HPP
#define SCLANGSUBPROCESS_HPP
#include "Subprocess.hpp"
#include "Signal.hpp"

namespace AlgAudio{

class SCLangSubprocess : private Subprocess{
public:
  SCLangSubprocess(std::string command);
  ~SCLangSubprocess();
  void SendInstruction(std::string);
  // The on_line_received signal will NOT fire automatically.
  // Data has to be read from subprocess to see if there is a new line.
  // To read the data and potentially trigger on_line_received, use
  // the folling PollOutput method.
  void PollOutput();
  Signal<std::string> on_line_received;
  void WaitForPrompt();
  std::string WaitForReply(std::string command);
private:
  std::string buffer;
  void ProcessBuffer();
  bool at_prompt = false;
  bool collecting_reply = false;
  std::string reply_buffer;
};

} // namespace AlgAudio

#endif // SCLANGSUBPROCESS_HPP
