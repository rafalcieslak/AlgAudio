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
#include "SDLHandle.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <iostream>

namespace AlgAudio{

int SDLHandle::reference_counter = 0;

SDLException::SDLException(std::string t) :
   Exception(t + ": " + SDL_GetError()) {}

SDLHandle::SDLHandle(){
  if(reference_counter == 0){
    std::cout << "Starting SDL." << std::endl;
    if(SDL_Init(SDL_INIT_EVENTS) != 0)
      throw SDLException("SDL_Init failed");
    if(SDL_VideoInit(NULL) != 0)
      throw SDLException("SDL_VideoInit failed");
    //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,1);
    //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,16);
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
    SDL_VideoQuit();
    SDL_Quit();
  }
}

} //namespace AlgAudio
