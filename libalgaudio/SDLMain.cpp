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

namespace AlgAudio{

std::map<unsigned int, std::shared_ptr<Window>> SDLMain::registered_windows;
std::atomic_bool SDLMain::running;
int SDLMain::last_draw_time = -1000000;

void SDLMain::Loop(){
  running = true;
  while(running){
    Step();
  }
}

void SDLMain::Step(){
  // Temporary CPU limiter.
  // Eventually this will have to be rewritten to waiting on a conditional variable
  // guarding a global events (sdl, osc, subprocess) queue, so that the main loop
  // only wakes up when it's needed.
  SDL_Delay(2);
  // Milliseconds from start
  int newtime = SDL_GetTicks();
  if(newtime > last_draw_time + 30){
    last_draw_time = newtime;
    // Process user input
    ProcessEvents();
    // Redraw registered windows
    for(auto& it : registered_windows){
      it.second->Render();
    }
    // Temporarily, by default, stop the main loop if there are no registered
    // windows left.
    if(registered_windows.size() == 0) Quit();
  }
  // Call the global idle, allowing all subsribers to do whatever they need
  Utilities::global_idle.Happen();
}

void SDLMain::ProcessEvents(){
  SDL_Event ev;
  while(SDL_PollEvent(&ev)){
    if(ev.type == SDL_QUIT){
      Quit();
      return;
    }


    unsigned int window_id = ev.window.windowID;
    auto it = registered_windows.find(window_id);
    if(it == registered_windows.end()){
      // TODO: Events for unregistered windows should be passed to an externally
      // visible signal, so that modules can subscribe to it.
      std::cout << "Warning: Received an event for an unregistered window " << window_id << std::endl;
      continue;
    }
    auto window = it->second;
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
      window->ProcessMouseButtonEvent( (ev.type == SDL_MOUSEBUTTONDOWN), ev.button.button, Point2D(ev.button.x, ev.button.y));
    }else if(ev.type == SDL_MOUSEMOTION){
      window->ProcessMotionEvent(Point2D(ev.motion.x, ev.motion.y));
    } // if(ev.type = ...)
  } // while PollEvent
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

} // namespace AlgAudio
