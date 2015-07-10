#include "SDLMain.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

namespace AlgAudio{

std::map<unsigned int, std::shared_ptr<Window>> SDLMain::registered_windows;
std::atomic_bool SDLMain::running;

void SDLMain::Loop(){
  running = true;
  while(running){
    Step();
  }
}

void SDLMain::Step(){
  // TODO: Actual FPS limit
  SDL_Delay(18);
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
      }
    }else if(ev.type == SDL_MOUSEBUTTONDOWN || ev.type == SDL_MOUSEBUTTONUP){
      window->ProcessMouseButtonEvent( (ev.type == SDL_MOUSEBUTTONDOWN), ev.button.button, ev.button.x, ev.button.y);
    }else if(ev.type == SDL_MOUSEMOTION){
      window->ProcessMotionEvent(ev.motion.x, ev.motion.y);
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

} // namespace AlgAudio
