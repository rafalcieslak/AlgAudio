#include "SDLTexture.hpp"
#include "UI/UIWindow.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

namespace AlgAudio{

SDLTexture::SDLTexture(std::weak_ptr<UIWindow> w, Size2D s)
  : parent(w), size(s){
    auto window = parent.lock();
    texture = SDL_CreateTexture(
      window->GetRenderer(),
      SDL_PIXELFORMAT_RGBA8888,
      SDL_TEXTUREACCESS_TARGET,
      size.width, size.height
    );
    if(!texture) throw SDLException("CreateTexture failed");
}

SDLTexture::~SDLTexture(){
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
