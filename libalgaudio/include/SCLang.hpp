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

namespace Exceptions{
struct SCLang : public Exception{
  SCLang(std::string t) : Exception(t) {}
};
} // namespace Exceptions

/** The static interface to a single global sclang process.
 *  Manages all interaction with the SuperCollider instance, including intepteter
 *  starting and halting, server configuration and booting, OSC messaging.
 */
class SCLang{
  SCLang() = delete; // static class
public:
  /** Launches SuperCollider. Starts the interpreter, prepares connection, boots
   *  the server etc. Uses the configuration from global config. \see Config */
  static void Start();
  /** Reboots SuperCollider. */
  static void Restart();
  /** Stops SuperCollider, closing the interpreter and stopping the subprocess. */
  static void Stop();
  /** Returns true iff SuperCollider was started and is ready to work. */
  static bool IsRunning() { return subprocess != nullptr; }
  /** This function will process any subprocess output. It shall be
   *  called only by the main thread. The main thread is notified about
   *  awaiting data by an SDL_UserEvent send to the main queue. It then calls
   *  this method, which in turn triggers all signals and callbacks that
   *  waited for sclang subprocess output. */
  static void PollSubprocess();
  /** This function will process any incoming OSC data. It shall be
   *  called only by the main thread. The main thread is notified about
   *  awaiting data by an SDL_UserEvent send to the main queue. It then calls
   *  this method, which in turn triggers all signals and callbacks that
   *  waited for OSC events. */
  static void PollOSC();
  /** Happens once for each line of sclang subprocess stdout. */
  static Signal<std::string> on_line_received;
  /** Happens when Start completes. Carries bool marking whether the start
   *  was sucessful, if not, the string contains an error message. */
  static Signal<bool, std::string> on_start_completed;
  /** Happens when the SC server started. Carries bool marking whether the
   *  start was successful. */
  static Signal<bool> on_server_started;
  /** Happens multiple times during start process. Each time it carries a number
   *  which increases during the start procedure (this is intended for
   *  progressbars), and a string with current status message, */
  static Signal<int,std::string> on_start_progress;
  /** Passes a text instruction to sclang subprocess via its stdin.
   *  \param instr The instruction to send. */
  static void SendInstruction(std::string instr);
  /** Performs SC synth template (SynthDef) installation on the server. */
  static LateReturn<> InstallTemplate(const std::shared_ptr<ModuleTemplate> templ);
  /** Returns true iff the template was already installed in the server.
   *  \param id The template id. */
  static bool WasInstalled(const std::string& id);
  /** Writes the list of all templates that were installed to stdout. */
  static void DebugQueryInstalled();
  /** Asks SuperCollider to s.QueryAllNodes. sclang subprocess will then print
   *  out a graph of active synths and groups hierarchy - useful for debugging.
   *  Observe on_line_received for result. */
  static void QueryAllNodes();

  /** Happens whenever sclang receives some MIDI event. */
  static Signal<MidiMessage> on_midi_message_received;

  ///@{
  /** Various functions and type templates for OSC communication. */
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
  ///@}

  /** Returns the new reply id the catcher will use. Afterwards one should
   *  set the SC Synth's arg to that returned value, so that it will start
   *  sending replies with the right id. */
  static int RegisterSendReply(int synth_id, std::weak_ptr<SendReplyController>);
  /** Unregisters a sendreply catcher that was previously created with
   *  RegisterSendReply(). */
  static void UnregisterSendReply(int synth_id, int reply_id);

  /** Boots the supercollider server. Uses global configuration. \see Config*/
  static void BootServer();
  /** Quits the supercollider server. */
  static void StopServer();
  
  /** True if the supercollider subprocess is ready to work. */
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
  Relay<Q...> r;
  SendOSCWithLOReply(path,args...).Then([=](lo::Message msg){
    r.Return( UnpackLOMessage<Q...>(msg,0) );
  });
  return r;
}
template <typename... Q>
inline LateReturn<Q...> SCLang::SendOSCCustomWithReply(const std::string& path, const lo::Message &m){
  static_assert(is_nonempty<Q...>::value, "If you wish to use SendOSCWithReply with no return types, use SendOSCWithEmptyReply instead.");
  Relay<Q...> r;
  SendOSCCustomWithLOReply(path,m).Then([=](lo::Message msg){
    r.Return( UnpackLOMessage<Q...>(msg,0) );
  });
  return r;
}
template <typename... Rest>
inline LateReturn<> SCLang::SendOSCWithEmptyReply(const std::string& path, Rest... args){
  Relay<> r;
  SendOSCWithLOReply(path,args...).Then([=](lo::Message){
    r.Return();
  });
  return r;
}

} // namespace AlgAudio

#endif // SCLANG_HPP
