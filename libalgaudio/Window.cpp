#include "Window.hpp"
#include <SDL2/SDL.h>
#include <iostream>
#include "UI/DrawContext.hpp"
#include "UI/UIWidget.hpp"
#include "SDLMain.hpp"

namespace AlgAudio{

Window::Window(std::string t, int w, int h) :
  title(t), width(w), height(h)
{
  //std::cout << "Creating a new window" << std::endl;
  // TODO: investigate SDL_WINDOW_INPUT_GRABBED
  window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_RESIZABLE);
  if(!window) throw SDLException("Unable to create a window");
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_ACCELERATED);
  //std::cout << "new renderer" << renderer << std::endl;
  if(!window) throw SDLException("Unable to create a renderer");
  id = SDL_GetWindowID(window);
}

std::shared_ptr<Window> Window::Create(std::string title, int w, int h){
  std::shared_ptr<Window> res(new Window(title,w,h));
  return res;
}

Window::~Window(){
  std::cout << "Destructing window " << title << std::endl;
  //std::cout << "destroying renderer" << renderer << std::endl;
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
}

void Window::Render(){
  if(!needs_redrawing) return;

  std::cout << "Redrawing window." << std::endl;

  Size2D size = GetSize();
  DrawContext c(renderer, 0, 0, size.width, size.height); // Full window DC
  // TODO: Themeset color
  c.SetColor(255,255,255);

	SDL_RenderClear(renderer);
  if(child) child->Draw(c);
	SDL_RenderPresent(renderer);

  needs_redrawing = false;
}

void Window::Insert(std::shared_ptr<UIWidget> ch){
  child = ch;
  child->window = shared_from_this();
  child->parent.reset();

  child->Resize(GetSize());
}

void Window::SetNeedsRedrawing(){
  needs_redrawing = true;
}

void Window::ProcessCloseEvent(){
  on_close.Happen();
  SDL_HideWindow(window);
}

void Window::ProcessEnterEvent(){
  if(!child) return;
  // The SDL enter event has no mouse position, but is always followed by a
  // motion event. We'll mark that we an enter event, and we will send it to the
  // widgets once we receive the next motion event.
  mouse_just_entered = true;
}

void Window::ProcessMotionEvent(Point2D p){
  if(child){
    if(mouse_just_entered){
      child->OnMotionEnter(p);
      mouse_just_entered = false;
    }
    child->OnMotion(prev_motion, p);
  }
  prev_motion = p;
}


void Window::ProcessLeaveEvent(){
  if(!child) return;
  // The SDL leave event has no mouse position, but is always preceded by a
  // motion event
  child->OnMotionLeave(prev_motion);
}

void Window::ProcessResizeEvent(){
  if(!child) return;
  child->Resize(GetSize());
  needs_redrawing = true;
}

void Window::ProcessMouseButtonEvent(bool d, short b, Point2D p){
  if(child)
    child->OnMouseButton(d,b,p);
}

Size2D Window::GetSize() const{
  int w,h;
  SDL_GetWindowSize(window, &w, &h);
  return Size2D(w,h);
}

} // namespace AlgAudio
