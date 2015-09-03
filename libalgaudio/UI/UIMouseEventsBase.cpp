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
#include "UI/UIMouseEventsBase.hpp"
#include "UI/UIWidget.hpp"

namespace AlgAudio{

bool UIMouseEventsBase::OnMousePress(bool down, MouseButton b,Point2D p){
  // Prolong instance lifetime in case this widget would get destroyed as a
  // result of a subscriber
  std::shared_ptr<UIWidget> ptr;
  auto widg = dynamic_cast<UIWidget*>(this);
  if(widg) ptr = widg->shared_from_this();
  
  bool result = CustomMousePress(down,b,p);
  (void)result;
  //if(result) return true; // The custom handler captured the event
  // In other case, execute our own code
  if(down == true && b == MouseButton::Left){
    pressed = true;
    if(focusable) RequestFocus();
    return on_pressed.Happen(true);
  }else if(down == false && b == MouseButton::Left && pressed == true){
    pressed = false;
    result = on_pressed.Happen(false);
    //if(result) return true;
    return on_clicked.Happen();
  }
  return false;
}

void UIMouseEventsBase::OnMouseEnter(Point2D p){
  CustomMouseEnter(p);
  pointed = true;
  last_mouse_pos = p;
  on_pointed.Happen(true);
}

void UIMouseEventsBase::OnMouseLeave(Point2D p){
  CustomMouseLeave(p);
  last_mouse_pos = p;
  if(pressed){
    pressed = false;
    on_pressed.Happen(false);
  }
  if(pointed){
    pointed = false;
    on_pointed.Happen(false);
  }
}

void UIMouseEventsBase::OnMouseMotion(Point2D p1, Point2D p2){
  CustomMouseMotion(p1,p2);
  last_mouse_pos = p2;
  on_motion.Happen(p2);
}

} // namespace AlgAudio
