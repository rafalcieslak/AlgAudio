#include "SDLMain.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

namespace AlgAudio{

std::list<std::shared_ptr<UIWindow>> SDLMain::registered_windows;
std::atomic_bool SDLMain::keep_running;

void SDLMain::Run(){
  keep_running = true;
  while(keep_running){
    SDL_Delay(18);
    // Process user input

    // Redraw registered windows
  }
}

void SDLMain::Quit(){
  keep_running = false;
}

void SDLMain::RegisterWindow(std::shared_ptr<UIWindow> w){
  registered_windows.push_back(w);
}

void SDLMain::UnregisterWindow(std::shared_ptr<UIWindow> w){
  registered_windows.remove(w);
}

} // namespace AlgAudio
