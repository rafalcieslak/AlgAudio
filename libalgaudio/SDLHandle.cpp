#include "SDLHandle.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

namespace AlgAudio{

int SDLHandle::reference_counter = 0;

SDLException::SDLException(std::string t) :
   Exception(t + ": " + SDL_GetError()) {}

SDLHandle::SDLHandle(){
  if(reference_counter == 0){
    std::cout << "Starting SDL." << std::endl;
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0)
      throw SDLException("SDL_Init failed");
    if(TTF_Init() != 0 )
      throw SDLException("TTF_Init failed");
  }
  reference_counter++;
  //std::cout << "Created a new SDLHandle " << reference_counter << std::endl;
}

SDLHandle::~SDLHandle(){
  //std::cout << "Destroying an SDLHandle " << reference_counter << std::endl;
  reference_counter--;
  if(reference_counter ==0){
    std::cout << "Stopping SDL." << std::endl;
    TTF_Quit();
    SDL_Quit();
  }
}

} //namespace AlgAudio
