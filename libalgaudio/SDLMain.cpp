#include "SDLMain.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

namespace AlgAudio{

std::map<unsigned int, std::shared_ptr<UIWindow>> SDLMain::registered_windows;
std::atomic_bool SDLMain::keep_running;

void SDLMain::Run(){
  keep_running = true;
  while(keep_running){
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
}

void SDLMain::ProcessEvents(){
  SDL_Event ev;
  while(SDL_PollEvent(&ev)){
    if(ev.type == SDL_WINDOWEVENT){
      const SDL_WindowEvent& win_ev = ev.window;
      auto it = registered_windows.find(win_ev.windowID);
      if(it == registered_windows.end()){
        // TODO: Events for unregistered windows should be passed to an externally
        // visible signal, so that modules can subscribe to it.
        std::cout << "Warning: Received an event for an unregistered window " << win_ev.windowID << std::endl;
      }
      else{
        auto window = it->second;
        if(win_ev.event == SDL_WINDOWEVENT_CLOSE){
          window->ProcessCloseEvent();
        }
      }
    }else if(ev.type == SDL_QUIT){
      Quit();
    }else if(ev.type == SDL_MOUSEBUTTONDOWN || ev.type == SDL_MOUSEBUTTONUP){
      const SDL_MouseButtonEvent& butt_ev = ev.button;
      auto it = registered_windows.find(butt_ev.windowID);
      if(it == registered_windows.end()){
        // TODO: Events for unregistered windows should be passed to an externally
        // visible signal, so that modules can subscribe to it.
        std::cout << "Warning: Received an event for an unregistered window " << butt_ev.windowID << std::endl;
      }else{
        auto window = it->second;
        window->ProcessMouseButtonEvent( (ev.type == SDL_MOUSEBUTTONDOWN), butt_ev.button, butt_ev.x, butt_ev.y);
      }
    }
  }
}

void SDLMain::Quit(){
  keep_running = false;
}

void SDLMain::RegisterWindow(std::shared_ptr<UIWindow> w){
  registered_windows[w->GetID()] = w;
}

void SDLMain::UnregisterWindow(std::shared_ptr<UIWindow> w){
  registered_windows.erase(w->GetID());
}

} // namespace AlgAudio
