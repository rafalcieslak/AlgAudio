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
#include "SDLTexture.hpp"
#include "Window.hpp"
#include <SDL2/SDL.h>
#include <iostream>

namespace AlgAudio{

SDLTexture::SDLTexture(std::weak_ptr<Window> w, Size2D s)
  : parent(w), size(s){
  //std::cout << "Creating a texture" << std::endl;
  if(s.IsEmpty()){
    // Invalid size
    valid = false;
    texture = nullptr;
  }else{
    valid = true;
    auto window = parent.lock();
    texture = SDL_CreateTexture(
      window->GetRenderer(),
      SDL_PIXELFORMAT_RGBA8888,
      SDL_TEXTUREACCESS_TARGET,
      size.width, size.height
    );
    if(!texture) throw Exceptions::SDLException("CreateTexture failed");
    // Set blendmode to an invalid value, in such case SDL will not interfere
    // and set blendmodes according to the texture config
    SDL_SetTextureBlendMode(texture, (SDL_BlendMode)99999999);
  }
}

SDLTexture::SDLTexture(std::weak_ptr<Window> w, SDL_Surface* surf)
  : parent(w) {
  //std::cout << "Creating a texture" << std::endl;
  valid = true;
  auto window = parent.lock();
  texture = SDL_CreateTextureFromSurface(window->GetRenderer(), surf);
  if(!texture) throw Exceptions::SDLException("CreateTexture failed");
  // Set blendmode to an invalid value, in such case SDL will not interfere
  // and set blendmodes according to the texture config
  SDL_SetTextureBlendMode(texture, (SDL_BlendMode)99999999);
  size.width = surf->w;
  size.height = surf->h;
  
}

/*
SDLTexture::SDLTexture(SDLTexture&& other) :
  handle(std::move(other.handle)),
  texture(std::move(other.texture)),
  valid(std::move(other.valid)),
  parent(std::move(other.parent)),
  size(std::move(other.size))
{
  other.valid = false;
  std::cout << "Texture " << texture << " move-constructed." << std::endl;
}

SDLTexture& SDLTexture::operator=(SDLTexture&& other){
  handle = std::move(other.handle);
  texture = std::move(other.texture);
  valid = std::move(other.valid);
  parent = std::move(other.parent);
  size = std::move(other.size);
  other.valid = false;
  std::cout << "Texture " << texture << " moved." << std::endl;
  return *this;
}
*/
SDLTexture::~SDLTexture(){
  if(valid && parent.lock()){
    SDL_DestroyTexture(texture);
  }
}

void SDLTexture::Resize(Size2D s){
  size = s;
  auto window = parent.lock();
  if(valid) SDL_DestroyTexture(texture);
  if(s.IsEmpty()){
    // Invalid size
    valid = false;
    texture = nullptr;
  }else{
    valid = true;
    texture = SDL_CreateTexture(
      window->GetRenderer(),
      SDL_PIXELFORMAT_RGBA8888,
      SDL_TEXTUREACCESS_TARGET,
      size.width, size.height
    );
    if(!texture) throw Exceptions::SDLException("CreateTexture failed");
    // Set blendmode to an invalid value, in such case SDL will not interfere
    // and set blendmodes according to the texture config
    SDL_SetTextureBlendMode(texture, (SDL_BlendMode)99999999);
  }
}

} // namespace AlgAudio
