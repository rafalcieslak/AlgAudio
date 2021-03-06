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
#include "SDLMain.hpp"
#include <SDL2/SDL.h>
#include <iostream>
#include "SCLang.hpp"
#include "Timer.hpp"

namespace AlgAudio{

std::map<unsigned int, std::shared_ptr<Window>> SDLMain::registered_windows;
std::atomic_bool SDLMain::running;
Signal<float> SDLMain::on_before_frame;
int SDLMain::last_draw_time = -1000000;
std::atomic<int> SDLMain::notify_event_id;
std::atomic_flag SDLMain::ev_flag_notify_osc_already_pushed = ATOMIC_FLAG_INIT;
std::atomic_flag SDLMain::ev_flag_notify_subprocess_already_pushed = ATOMIC_FLAG_INIT;

void SDLMain::Init(){
  notify_event_id = SDL_RegisterEvents(1);
  KeyData::InitKeymap();
}

void SDLMain::Loop(){
  running = true;
  while(running){
    SDL_Event ev;
    // Sleep at most 2ms for new events, be it SDL event, or a custom
    // OSC/Subprocess notify messsage.
    // If any event has arrived, deal with it.
    if (SDL_WaitEventTimeout(&ev, 2)){
      ProcessEvent(ev);
      while(SDL_PollEvent(&ev)) ProcessEvent(ev);
    }
    // Regardless of whether there was an event or not, we redraw all windows
    // from time to time.

    // Milliseconds from start
    int newtime = SDL_GetTicks();
    int delta = newtime - last_draw_time;
    if(delta > 15){ // FPS limiter
      last_draw_time = newtime;
      // Process possible animations
      on_before_frame.Happen(delta/1000.0);
      // Redraw registered windows
      for(auto& it : registered_windows){
        it.second->Render();
      }
      // Temporarily, by default, stop the main loop if there are no registered
      // windows left.
      if(registered_windows.size() == 0) Quit();
    }
  }
}

void SDLMain::ProcessEvent(const SDL_Event& ev){
  if(ev.type == SDL_QUIT){
    Quit();
    return;
  }

  if(ev.type == (unsigned int)notify_event_id){
    if(ev.user.code == NOTIFY_SUBPROCESS){
      ev_flag_notify_subprocess_already_pushed.clear();
      SCLang::PollSubprocess();
    }else if(ev.user.code == NOTIFY_OSC){
      ev_flag_notify_osc_already_pushed.clear();
      SCLang::PollOSC();
    }else if(ev.user.code == NOTIFY_TIMER){
      Timer::Trigger(ev.user.data1);
    }
    return;
  }

  unsigned int window_id = ev.window.windowID;
  auto it = registered_windows.find(window_id);
  if(it == registered_windows.end()){
    // TODO: Events for unregistered windows should be passed to an externally
    // visible signal, so that modules can subscribe to it.
    std::cout << "Warning: Received an event for an unregistered window " << window_id << std::endl;
    return;
  }
  std::shared_ptr<Window> window = it->second;

  if(ev.type == SDL_WINDOWEVENT){
    if(ev.window.event == SDL_WINDOWEVENT_CLOSE){
      window->ProcessCloseEvent();
    }else if(ev.window.event == SDL_WINDOWEVENT_ENTER){
      window->ProcessEnterEvent();
    }else if(ev.window.event == SDL_WINDOWEVENT_LEAVE){
      window->ProcessLeaveEvent();
    }else if(ev.window.event == SDL_WINDOWEVENT_RESIZED){
      window->ProcessResizeEvent();
    }
  }else if(ev.type == SDL_MOUSEBUTTONDOWN || ev.type == SDL_MOUSEBUTTONUP){
    MouseButton b;
    if(ev.button.button == SDL_BUTTON_LEFT) b = MouseButton::Left;
    if(ev.button.button == SDL_BUTTON_RIGHT) b = MouseButton::Right;
    if(ev.button.button == SDL_BUTTON_MIDDLE) b = MouseButton::Middle;
    window->ProcessMouseButtonEvent( (ev.type == SDL_MOUSEBUTTONDOWN), b, Point2D(ev.button.x, ev.button.y));
  }else if(ev.type == SDL_MOUSEWHEEL){
    MouseButton b;
    if(ev.wheel.y > 0) b = MouseButton::WheelUp;
    else b = MouseButton::WheelDown;
    int n = ev.wheel.y > 0 ? ev.wheel.y : -ev.wheel.y; // abs
    while(n-->0) window->ProcessWheelEvent(b);
  }else if(ev.type == SDL_MOUSEMOTION){
    window->ProcessMotionEvent(Point2D(ev.motion.x, ev.motion.y));
  }else if(ev.type == SDL_KEYDOWN || ev.type == SDL_KEYUP){
    window->ProcessKeyboardEvent( KeyData(ev.key) );
  }else if(ev.type == SDL_TEXTINPUT){
    window->ProcessKeyboardEvent( KeyData(ev.text.text) );
  }else{
    std::cout << "Other event" << std::endl;
  }
}

void SDLMain::PushNotifySubprocessEvent(){
  // Do not push another notify event to the queue, is one is already present.
  if(ev_flag_notify_subprocess_already_pushed.test_and_set()) return;

  SDL_Event event;
  SDL_zero(event);
  event.type = notify_event_id;
  event.user.code = NOTIFY_SUBPROCESS;
  event.user.data1 = nullptr;
  event.user.data2 = nullptr;
  SDL_PushEvent(&event);
}
void SDLMain::PushNotifyOSCEvent(){
  // Do not push another notify event to the queue, is one is already present.
  if(ev_flag_notify_osc_already_pushed.test_and_set()) return;

  SDL_Event event;
  SDL_zero(event);
  event.type = notify_event_id;
  event.user.code = NOTIFY_OSC;
  event.user.data1 = nullptr;
  event.user.data2 = nullptr;
  SDL_PushEvent(&event);
}

void SDLMain::Quit(){
  running = false;
}

void SDLMain::RegisterWindow(std::shared_ptr<Window> w){
  registered_windows[w->GetID()] = w;
}

void SDLMain::UnregisterWindow(std::shared_ptr<Window> w){
  registered_windows.erase(w->GetID());
}
void SDLMain::UnregisterAll(){
  registered_windows.clear();
}

void SDLMain::SetTextInput(bool b){
  if(b){
    SDL_StartTextInput();
  }else{
    SDL_StopTextInput();
  }
}

} // namespace AlgAudio
