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

std::shared_ptr<UIWindow> UIWindow::Create(std::string title, int w, int h){
  std::shared_ptr<UIWindow> res(new UIWindow(title,w,h));
  return res;
}


UIWindow::~UIWindow(){
  std::cout << "Destructing window" << std::endl;
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
}

void UIWindow::Render(){
  if(!needs_redrawing) return;

  std::cout << "Redrawing window." << std::endl;

  // TODO: Window size
  DrawContext c(renderer, 0, 0, 350, 300); // Full window DC
  // TODO: Themeset color
  c.SetColor(255,255,255);

	SDL_RenderClear(renderer);
  if(child) child->Draw(c);
	SDL_RenderPresent(renderer);

  needs_redrawing = false;
}

void UIWindow::Insert(std::shared_ptr<UIWidget> ch){
  child = ch;
  child->window = shared_from_this();
  child->parent.reset();
}

void UIWindow::SetNeedsRedrawing(){
  needs_redrawing = true;
}

void UIWindow::ProcessCloseEvent(){
  SDL_HideWindow(window);
  // If the window was not registered, the following has no effect.
  SDLMain::UnregisterWindow(shared_from_this());
}

void UIWindow::ProcessMotionEvent(int x, int y){
  if(child)
    child->OnMotion(prev_motion_x, prev_motion_y, x, y);
  prev_motion_x = x;
  prev_motion_y = y;
}

void UIWindow::ProcessEnterEvent(){
  if(child)
    child->OnMotionEnter();
}

void UIWindow::ProcessLeaveEvent(){
  if(child)
    child->OnMotionLeave();
}

void UIWindow::ProcessMouseButtonEvent(bool d, short b, int x, int y){
  if(child)
    child->OnMouseButton(d,b,x,y);
}

} // namespace AlgAudio
