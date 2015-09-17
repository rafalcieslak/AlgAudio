#ifndef UIVISIBILITYBASE_HPP
#define UIVISIBILITYBASE_HPP
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
namespace AlgAudio{

class UIVisibilityBase{
public:
  enum class DisplayMode{
    Visible, /**< A Visible widget is drawn just normally. */
    EmptySpace, /**< An EmptySpace widget is not drawn, but it takes as much space as it would normally take. */
    Invisible, /**< An Invisible widget is not drawn, and it takes zero area. */
  };
  /** Sets widget display mode. \see DisplayModes */
  void SetDisplayMode(DisplayMode m){
    if(m == display_mode) return;
    display_mode = m;
    on_display_mode_changed.Happen();
  }
  /** Returns true if the contents of the widget are supposed to be drawn, i.e.
   *  whether display mode is 'visible'. When implementing a custom widget,
   *  do do not need to test for being drawn in CustomDraw, if a widget is not
   *  supposed to be drawn, CustomDraw will never be called. */
  inline bool IsDrawn() const {return display_mode == DisplayMode::Visible;}
  /** Returns true if this widget is marked as invisible. */
  inline bool IsInvisible() const {return display_mode == DisplayMode::Invisible; }
  
  /** Triggered when visibility changes. */
  Signal<> on_display_mode_changed;
  
protected:
  UIVisibilityBase() {} // Only construcible when inherited
  
  DisplayMode display_mode = DisplayMode::Visible;
};

} // namespace AlgAudio

#endif // UIVISIBILITYBASE_HPP
