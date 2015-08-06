#ifndef UICONTAINER_HPP
#define UICONTAINER_HPP
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

namespace AlgAudio{

// This file contains interfaces for

// This is an interface for widgets which act as a container for a single child,
// like UICentered or UIMarginBox
class UIContainerSingle : public UIWidget{
public:
  virtual void Insert(std::shared_ptr<UIWidget>) = 0;
  virtual Point2D GetChildPos() const = 0;
  virtual void RemoveChild() = 0;
protected:
  UIContainerSingle(std::weak_ptr<Window> parent_window) : UIWidget(parent_window) {}
  std::shared_ptr<UIWidget> child;
};

} // namespace AlgAudio

#endif // UICONTAINER_HPP
