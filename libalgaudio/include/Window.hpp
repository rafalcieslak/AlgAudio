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

/** This class represents a drawable on-screen window. */
class Window : public virtual SubscriptionsManager, public IAlertable, public std::enable_shared_from_this<Window>{
private:
  SDLHandle h;
public:
  /** Creates a new native window.
   *  \param title The window title to use.
   *  \param w Initial window width.
   *  \param h Initial window height.
   *  \param centered If true, window will be placed on the center of the screen.
   *  \param resizable If false, the user will not be able to manually change the dimentions of the window.
   *        Otherwise, the initial window position is undefined. */
  static std::shared_ptr<Window> Create(std::string title = "AlgAudio", int w = 350, int h = 300, bool centered = true, bool resizable = true);
  ~Window();

  /** Explicitly forbids copying windows. */
  Window(const Window&) = delete;
  Window& operator=(const Window&) = delete;

  /** Starts the render chain of this window, by asking the child widget to
   *  render itself on this window. */
  void Render();

  /** Places a child widget onto the window. This should be the topmost widget
   *  in a hierarchy. */
  void Insert(std::shared_ptr<UIWidget> child);

  /** Marks the window as dirty. It will be redrawn when it has a chance/ */
  void SetNeedsRedrawing();

  virtual void ProcessCloseEvent();
  void ProcessMouseButtonEvent(bool down, MouseButton button, Point2D);
  void ProcessWheelEvent(MouseButton button);
  void ProcessMotionEvent(Point2D);
  void ProcessEnterEvent();
  void ProcessLeaveEvent();
  void ProcessResizeEvent();
  virtual void ProcessKeyboardEvent(KeyData data);

  template<class W, typename... Args>
  std::shared_ptr<W> Create(Args... args){
    return W::Create(shared_from_this(), args...);
  }

  Signal<> on_close;

  /** Returns a unique window identifier, as defined by SDL. */
  unsigned int GetID() const {return id;}
  /** Returns current window size. */
  Size2D GetSize() const;
  /** Returns a pointer the the SDL_Renderer associated with this window. */
  SDL_Renderer* GetRenderer() const {return renderer;}
  /** Returns a pointer to the underlying SDL_Window. */
  SDL_Window* GetWindow() const {return window;}

  // Empty implementation.
  virtual LateReturn<int> ShowSimpleAlert( std::string,std::string,std::string,AlertType,Color,Color)
  {throw Exceptions::WindowNotAlertable("This window is not capable of displaying alerts.");}
  virtual LateReturn<> ShowErrorAlert(std::string, std::string)
  {throw  Exceptions::WindowNotAlertable("This window is not capable of displaying alerts.");}
  
  /** Returns the widget that is in the root of this window's widget hierarchy. */
  std::shared_ptr<UIWidget> GetRoot() const {return child;}
protected:
  Window(std::string title, int w, int h, bool centered = true, bool resizable = true);
private:
  std::string title;
  int width;
  int height;
  unsigned int id;
  SDL_Window* window;
  SDL_Renderer* renderer;
  // Note: SDL_GLContext is defined as a typedef for void*, so it's a pointer type.
  SDL_GLContext context;

  Point2D prev_motion = Point2D(0,0);
  bool mouse_just_entered = false;
  Point2D last_mouse_pos = Point2D(0,0);

  bool needs_redrawing = true;

  std::shared_ptr<UIWidget> child;
};

} //namespace AlgAudio
#endif // WINDOW_HPP
