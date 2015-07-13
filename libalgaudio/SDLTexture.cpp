#include "SDLTexture.hpp"
#include "Window.hpp"
#include <SDL2/SDL.h>
#include <iostream>

namespace AlgAudio{

SDLTexture::SDLTexture(std::weak_ptr<Window> w, Size2D s)
  : parent(w), size(s){
    //std::cout << "Creating a texture" << std::endl;
    auto window = parent.lock();
    texture = SDL_CreateTexture(
      window->GetRenderer(),
      SDL_PIXELFORMAT_RGBA8888,
      SDL_TEXTUREACCESS_TARGET,
      size.width, size.height
    );
    if(!texture) throw SDLException("CreateTexture failed");
}

SDLTexture::SDLTexture(std::weak_ptr<Window> w, SDL_Surface* surf)
  : parent(w) {
  //std::cout << "Creating a texture" << std::endl;
  auto window = parent.lock();
  texture = SDL_CreateTextureFromSurface(window->GetRenderer(), surf);
  if(!texture) throw SDLException("CreateTexture failed");
  size.width = surf->w;
  size.height = surf->h;
}

SDLTexture::~SDLTexture(){
  //std::cout << "Destroying a texture" << std::endl;
  SDL_DestroyTexture(texture);
}

void SDLTexture::Resize(Size2D s){
  size = s;
  auto window = parent.lock();
  SDL_DestroyTexture(texture);
  texture = SDL_CreateTexture(
    window->GetRenderer(),
    SDL_PIXELFORMAT_RGBA8888,
    SDL_TEXTUREACCESS_TARGET,
    size.width, size.height
  );
  if(!texture) throw SDLException("CreateTexture failed");
}

} // namespace AlgAudio
