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
#include <type_traits>

#include "OSC.hpp"
#include "Signal.hpp"
#include "LateReturn.hpp"

namespace AlgAudio{

class SCLangSubprocess;
class ModuleTemplate;
class SendReplyController;

struct SCLangException : public Exception{
  SCLangException(std::string t) : Exception(t) {}
};

/* The static interface to a single global sclang process.
 * Manages all interaction with the SuperCollider instance, including intepteter
 * starting and halting, server configuration and booting, OSC messaging.
 */
class SCLang{
  SCLang() = delete; // static class
public:
  // Interpreter state controls.
  static void Start(std::string command, bool supernova = false);
  static void Restart(std::string command);
  static void Stop();
  static bool IsRunning() { return subprocess != nullptr; }
  // These function will process any subprocess or OSC input. They shall be
  // called only by the main thread. The main thread is notified about
  // awaiting input by an SDL_UserEvent send to the main queue. When
  // corresponding event is taken from the queue, the
  static void PollSubprocess();
  static void PollOSC();
  static Signal<std::string> on_line_received;
  // Arg: bool: success, string: message
  static Signal<bool, std::string> on_start_completed;
  // Arg: bool: success
  static Signal<bool> on_server_started;
  static Signal<int,std::string> on_start_progress;
  static void SendInstruction(std::string);
  static LateReturn<> InstallTemplate(const std::shared_ptr<ModuleTemplate> templ);
  static bool WasInstalled(const std::string&);
  static void DebugQueryInstalled();

  static Signal<MidiMessage> on_midi_message_received;

  // Various functions and type templates for OSC communication.
  static void SetOSCDebug(bool enabled);
  static void SendOSC(const std::string& path);
  static void SendOSC(const std::string& path, std::string tag, ...);
  static void SendOSCCustom(const std::string& path, const lo::Message& m);
  static LateReturn<lo::Message> SendOSCWithLOReply(const std::string& path);
  static LateReturn<lo::Message> SendOSCWithLOReply(const std::string& path, std::string tag, ...);
  static LateReturn<lo::Message> SendOSCCustomWithLOReply(const std::string& path, const lo::Message& m);
  template <typename... Q, typename... Rest>
  inline static LateReturn<Q...> SendOSCWithReply(const std::string& path, Rest... args);
  template <typename... Q>
  inline static LateReturn<Q...> SendOSCCustomWithReply(const std::string& path, const lo::Message& m);
  // The above function cannot be partially speciallised... Thus we need to use another name for the case
  // when Q = {}.
  template <typename... Rest>
  inline static LateReturn<> SendOSCWithEmptyReply(const std::string& path, Rest... args);

  // Returns the new reply id the catcher will use. Afterwards it is logical
  // to set the SC Synth arg to that returned value, so that it will start
  // sending replies with the right id.
  static int RegisterSendReply(int synth_id, std::weak_ptr<SendReplyController>);
  static void UnregisterSendReply(int synth_id, int reply_id);

  // Controls SuperCollider server.
  static void BootServer(bool supernova = false);
  static void StopServer();
  static bool ready;
private:
  static std::unique_ptr<SCLangSubprocess> subprocess;
  static std::set<std::string> installed_templates;
  static bool osc_debug;
  static std::unique_ptr<OSC> osc;
  static void SendReplyCatcher(int synth_id, int reply_id, float value);
  static void ProcessMIDIInput(lo::Message);
  static std::map<std::pair<int,int>, std::weak_ptr<SendReplyController>> sendreply_map;
  static int sendreply_id;
};

template <typename... T>
struct is_nonempty : std::true_type {};
template <>
struct is_nonempty<> : std::false_type {};

template <typename... Q, typename... Rest>
inline LateReturn<Q...> SCLang::SendOSCWithReply(const std::string& path, Rest... args){
  static_assert(is_nonempty<Q...>::value, "If you wish to use SendOSCWithReply with no return types, use SendOSCWithEmptyReply instead.");
  auto r = Relay<Q...>::Create();
  SendOSCWithLOReply(path,args...).Then([=](lo::Message msg){
    r.Return( UnpackLOMessage<Q...>(msg,0) );
  });
  return r;
}
template <typename... Q>
inline LateReturn<Q...> SCLang::SendOSCCustomWithReply(const std::string& path, const lo::Message &m){
  static_assert(is_nonempty<Q...>::value, "If you wish to use SendOSCWithReply with no return types, use SendOSCWithEmptyReply instead.");
  auto r = Relay<Q...>::Create();
  SendOSCCustomWithLOReply(path,m).Then([=](lo::Message msg){
    r.Return( UnpackLOMessage<Q...>(msg,0) );
  });
  return r;
}
template <typename... Rest>
inline LateReturn<> SCLang::SendOSCWithEmptyReply(const std::string& path, Rest... args){
  auto r = Relay<>::Create();
  SendOSCWithLOReply(path,args...).Then([=](lo::Message){
    r.Return();
  });
  return r;
}

} // namespace AlgAudio

#endif // SCLANG_HPP
