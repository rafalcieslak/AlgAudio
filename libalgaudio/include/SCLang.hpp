#ifndef SCLANG_HPP
#define SCLANG_HPP
#include <memory>
#include <set>
#include "Signal.hpp"

namespace AlgAudio{

class SCLangSubprocess;
class ModuleTemplate;

// The static interface to a single global sclang process
class SCLang{
  SCLang() = delete; // static class
public:
  static void Start(std::string command);
  static void Restart(std::string command);
  /* It is recommended to call Stop() manually before the global library
  destructor kicks in. Otherwise, the on_line_received signal may fire for some
  already destroyed objects. */
  static void Stop();
  static bool IsRunning() { return subprocess != nullptr; }
  static void Poll();
  static Signal<std::string> on_line_received;
  static Signal<> on_start_completed;
  static void SendInstruction(std::string);
  static void SendOSCSimple(std::string);
  static void InstallTemplate(const ModuleTemplate&);
  static bool WasInstalled(const std::string&);
  static void SetOSCDebug(bool enabled);
  static void SendOSC(const std::string &path, const std::string &tag, ...);
private:
  static void Start2();
  static std::unique_ptr<SCLangSubprocess> subprocess;
  static std::set<std::string> installed_templates;
  static bool osc_debug;
};

} // namespace AlgAudio

#endif // SCLANG_HPP
