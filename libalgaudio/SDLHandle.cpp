#include "SDLHandle.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

namespace AlgAudio{

int SDLHandle::reference_counter = 0;

SDLHandle::SDLHandle(){
  if(reference_counter == 0){
    std::cout << "Starting SDL." << std::endl;
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0)
      throw SDLInitException(std::string("SDL_Init failed: ") + SDL_GetError());
    if(TTF_Init() != 0 )
      throw SDLInitException(std::string("TTF_Init failed: ") + SDL_GetError());
  }
  reference_counter++;
}

SDLHandle::~SDLHandle(){
  reference_counter--;
  if(reference_counter ==0){
    std::cout << "Stopping SDL." << std::endl;
    TTF_Quit();
    SDL_Quit();
  }
}

} //namespace AlgAudio
