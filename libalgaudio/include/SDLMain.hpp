#ifndef SDLMAIN_HPP
#define SDLMAIN_HPP
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
#include <map>
#include <memory>
#include <atomic>

#include "Window.hpp"

union SDL_Event;

namespace AlgAudio{

/** The static interface implementing the main loop, event processing and window
 *  rendering.
 */
class SDLMain{
private:
  SDLMain() = delete; // static class
public:
  /** Init() must be caled before any other calls to this class members. */
  static void Init();
  /** Stops the main loop. */
  static void Quit();
  /** This function enters the main loop, which keeps procesing events and
   *  triggering their routines. Note that it's not reentrant.
   *  \warning This is a blocking call. This function will not return until
   *  Quit() is called.
   */
  static void Loop();
  /** Executes a single step of the main loop. This call is not blocking. */
  static void Step();
  /** A registered window is managed and rendered as SDLMain sees fit.
   *  This is handy it you wish to create a window and forget about it,
   *  as SDLMain will then sustain the ownership of the window, and it will keep
   *  redrawing it when needed.
   *  However, if you wish to redraw a window on your own, you should not
   *  register it.
   */
  static void RegisterWindow(std::shared_ptr<Window>);
  /** Unregisters a window. It will no longer be managed by SDLMain, you will
   *  have to render it on your own, and manually process its events. 
   *  \see RegisterWindow */
  static void UnregisterWindow(std::shared_ptr<Window>);
  /** Unregisters all registered windows. \see UnregisterWindow */
  static void UnregisterAll();
  /** Returns the number of currently registered windows. */
  static unsigned int GetWindowNum() {return registered_windows.size();}

  /** Hook your code to this signal if you wish to perform animations.
   *  This signal will be triggered every time a fram is drawn, just before
   *  rendering happens.
   *  The float argument is the time delta (in seconds) from the time where
   *  the last frame was drawn. 
   */
  static Signal<float> on_before_frame;

  // These two funcitions are used by i/o threads to notify the main loop that
  // there is some input ready to be processed. These functions are thread-safe.
  // They send a custom SDL_UserEvent to the main event queue, effectivelly
  // waking up the main thread.
  static void PushNotifySubprocessEvent();
  static void PushNotifyOSCEvent();
  static std::atomic<int> notify_event_id;

  static void SetTextInput(bool);

  static std::atomic_bool running;
  
  enum CustomEventCodes{
    NOTIFY_SUBPROCESS,
    NOTIFY_OSC,
    NOTIFY_TIMER,
  };
private:
  static std::map<unsigned int, std::shared_ptr<Window>> registered_windows;
  static void ProcessEvent(const SDL_Event&);
  static int last_draw_time;

  // These flags indicate whether a notify event is already in SDL queue.
  // This allows limiting the number of such events in queue to 1, to avoid
  // flooding the queue with same events, in case multile osc messages arrive
  // simultaneously.
  static std::atomic_flag ev_flag_notify_osc_already_pushed;
  static std::atomic_flag ev_flag_notify_subprocess_already_pushed;
};

} // namespace AlgAudio

#endif // SDLMAIN_HPP
