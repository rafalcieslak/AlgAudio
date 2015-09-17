#ifndef UIMOUSEEVENTSBASE_HPP
#define UIMOUSEEVENTSBASE_HPP
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
#include "Utilities.hpp"
#include "UI/UIVisibilityBase.hpp"

namespace AlgAudio{

class UIMouseEventsBase : public UIVisibilityBase{
public:
  Signal<> on_clicked;
  Signal<bool> on_pointed;
  Signal<bool> on_pressed;
  Signal<Point2D> on_motion;
  // Returns true, if the press event was captured (performed anything
  // meaningful), and false in any other case.
  bool OnMousePress(bool,MouseButton,Point2D);
  void OnMouseEnter(Point2D);
  void OnMouseLeave(Point2D);
  void OnMouseMotion(Point2D,Point2D);
  virtual bool CustomMousePress(bool,MouseButton,Point2D) {return false;}
  virtual void CustomMouseEnter(Point2D) {}
  virtual void CustomMouseLeave(Point2D) {}
  virtual void CustomMouseMotion(Point2D,Point2D) {}

  virtual void RequestFocus() = 0;

  void SetFocusable(bool f){focusable = f;}
  Point2D last_mouse_pos;
protected:
  UIMouseEventsBase() {} // Only construcible when inherited
  bool pressed = false;
  bool pointed = false;
  // If a child is focusable, clicking on it will call RequestFocus().
  bool focusable  = true;
};

} // namespace AlgAudio

#endif // UIMOUSEEVENTSBASE_HPP
