#ifndef UICLICKABLE_HPP
#define UICLICKABLE_HPP
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
#include "UIWidget.hpp"
#include "Theme.hpp"

namespace AlgAudio{

class UIClickable : public UIWidget{
public:
  Signal<> on_clicked;
  Signal<bool> on_pointed;
  Signal<bool> on_pressed;
  virtual void OnMouseButton(bool,short,Point2D) override;
  virtual void OnMotionEnter(Point2D) override;
  virtual void OnMotionLeave(Point2D) override;
protected:
  UIClickable(std::weak_ptr<Window> parent_window);
  bool pressed = false;
  bool pointed = false;
};

} // namespace AlgAudio

#endif // UICLICKABLE_HPP
