#ifndef SCLANG_HPP
#define SCLANG_HPP
#include <memory>
#include "Signal.hpp"

namespace AlgAudio{

class SCLangSubprocess;

// The static interface to a single global sclang process
class SCLang{
  SCLang() = delete; // static class
public:
  static void Start(std::string command);
  static void Restart(std::string command);
  static void Stop();
  static bool IsRunning() { return subprocess != nullptr; }
  static void Poll();
  static Signal<std::string> on_line_received;
  static void SendInstruction(std::string);
private:
  static std::unique_ptr<SCLangSubprocess> subprocess;
};

} // namespace AlgAudio

#endif // SCLANG_HPP
