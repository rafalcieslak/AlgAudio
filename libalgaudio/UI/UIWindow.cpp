#include "UI/UIWindow.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include "UI/DrawContext.hpp"
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
  // TODO: Window size
  DrawContext c(renderer, 0, 0, 350, 300); // Full window DC
  // TODO: Themeset color
  c.SetColor(255,255,255);

	SDL_RenderClear(renderer);
  if(child){
    child->Draw(c);
  }
	SDL_RenderPresent(renderer);
}

void UIWindow::Insert(std::shared_ptr<UIWidget> ch){
  child = ch;
  child->window = shared_from_this();
  child->parent.reset();
}

void UIWindow::ProcessCloseEvent(){
  std::cout << "Window close event" << std::endl;
  SDL_HideWindow(window);
  // If the window was not registered, the following has no effect.
  SDLMain::UnregisterWindow(shared_from_this());
}

} // namespace AlgAudio
