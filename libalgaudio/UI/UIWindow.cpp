#include "UI/UIWindow.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include "SDLMain.hpp"

namespace AlgAudio{

UIWindow::UIWindow(std::string t, int w, int h) :
  title(t), width(w), height(h)
{
  // TODO: investigate SDL_WINDOW_INPUT_GRABBED
  window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
  if(!window) throw SDLException("Unable to create a window");
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_ACCELERATED);
  if(!window) throw SDLException("Unable to create a renderer");
  id = SDL_GetWindowID(window);
}

UIWindow::~UIWindow(){
  std::cout << "Destructing window" << std::endl;
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
}

void UIWindow::Render(){
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);
}

void UIWindow::ProcessCloseEvent(){
  std::cout << "Window close event" << std::endl;
  SDL_HideWindow(window);
  SDLMain::UnregisterWindow(shared_from_this());
}

} // namespace AlgAudio
