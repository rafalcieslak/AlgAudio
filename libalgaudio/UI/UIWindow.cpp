#include "UI/UIWindow.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

namespace AlgAudio{

UIWindow::UIWindow(std::string t, int w, int h) :
  title(t), width(w), height(h)
{
  // TODO: investigate SDL_WINDOW_INPUT_GRABBED
  window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
  if(!window) throw SDLException("Unable to create a window");
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_ACCELERATED);
  if(!window) throw SDLException("Unable to create a renderer");
}

UIWindow::~UIWindow(){
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
}

} // namespace AlgAudio
