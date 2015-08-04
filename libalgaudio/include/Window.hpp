#ifndef WINDOW_HPP
#define WINDOW_HPP
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
#include <memory>
#include <SDL2/SDL.h>
#include "SDLHandle.hpp"
#include "Signal.hpp"
#include "Theme.hpp"
#include "LateReturn.hpp"
#include "Alertable.hpp"

struct SDL_Window;
struct SDL_Renderer;

namespace AlgAudio{

class UIWidget;

class Window : public SubscriptionsManager, public IAlertable, public std::enable_shared_from_this<Window>{
private:
  SDLHandle h;
public:
  static std::shared_ptr<Window> Create(std::string title = "AlgAudio", int w = 350, int h = 300, bool centered = true);
  ~Window();

  // explicitly forbid copying windows
  Window(const Window&) = delete;
  Window& operator=(const Window&) = delete;

  void Render();

  void Insert(std::shared_ptr<UIWidget> child);

  void SetNeedsRedrawing();

  void ProcessCloseEvent();
  void ProcessMouseButtonEvent(bool down, short button, Point2D);
  void ProcessMotionEvent(Point2D);
  void ProcessEnterEvent();
  void ProcessLeaveEvent();
  void ProcessResizeEvent();

  template<class W, typename... Args>
  std::shared_ptr<W> Create(Args... args){
    return W::Create(shared_from_this(), args...);
  }

  Signal<> on_close;

  unsigned int GetID() const {return id;}
  Size2D GetSize() const;
  SDL_Renderer* GetRenderer() const {return renderer;}
  SDL_Window* GetWindow() const {return window;}

  // Empty implementation.
  virtual LateReturn<int> ShowSimpleAlert( std::string,std::string,std::string,AlertType,Color,Color)
  {throw AlertableException("This window is not capable of displaying alerts.");}
  virtual LateReturn<> ShowErrorAlert(std::string, std::string)
  {throw  AlertableException("This window is not capable of displaying alerts.");}
protected:
  Window(std::string title, int w, int h, bool centered = true);
private:
  std::string title;
  int width;
  int height;
  unsigned int id;
  SDL_Window* window;
  SDL_Renderer* renderer;
  // Note: SDL_GLContext is defined as a typedef for void*, so it's a pointer type.
  SDL_GLContext context;

  Point2D prev_motion = Point2D(-1,-1);
  bool mouse_just_entered = false;

  bool needs_redrawing = true;

  std::shared_ptr<UIWidget> child;
};

} //namespace AlgAudio
#endif // WINDOW_HPP
