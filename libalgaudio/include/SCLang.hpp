#ifndef SCLANG_HPP
#define SCLANG_HPP
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
#include <memory>
#include <set>

#include "OSC.hpp"
#include "Signal.hpp"

namespace AlgAudio{

class SCLangSubprocess;
class ModuleTemplate;

struct SCLangException : public Exception{
  SCLangException(std::string t) : Exception(t) {}
};

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
  static Signal<> on_server_started;
  static Signal<int,std::string> on_start_progress;
  static void SendInstruction(std::string);
  static void InstallTemplate(const ModuleTemplate&);
  static bool WasInstalled(const std::string&);
  static void DebugQueryInstalled();
  static void SetOSCDebug(bool enabled);
  static void SendOSCSimple(std::string);
  static void SendOSCSimple(std::function<void(lo::Message)> f, std::string a);
  static void SendOSC(const std::string &path, const std::string &tag, ...);
  static void SendOSC(std::function<void(lo::Message)>, const std::string &path, const std::string &tag, ...);
  static void BootServer(bool supernova = false);
  static void StopServer();
  static bool ready;
private:
  static void Start2();
  static std::unique_ptr<SCLangSubprocess> subprocess;
  static std::set<std::string> installed_templates;
  static bool osc_debug;
  static std::unique_ptr<OSC> osc;
};

} // namespace AlgAudio

#endif // SCLANG_HPP
