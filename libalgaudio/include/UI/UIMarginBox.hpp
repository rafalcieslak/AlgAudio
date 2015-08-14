#ifndef UIMARGINBOX_HPP
#define UIMARGINBOX_HPP
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
#include "UIContainer.hpp"

namespace AlgAudio{

class UIMarginBox : public UIContainerSingle{
public:
  static std::shared_ptr<UIMarginBox> Create(std::weak_ptr<Window> parent_window, int top, int right, int bottom, int left);
  virtual void CustomDraw(DrawContext& c) override;
  virtual void CustomResize(Size2D s) override;
  virtual void OnChildRequestedSizeChanged() override;
  virtual void OnChildVisibilityChanged() override;
  virtual void Insert(std::shared_ptr<UIWidget>);
  virtual bool CustomMousePress(bool down, MouseButton b,Point2D) override;
  virtual void CustomMouseMotion(Point2D,Point2D) override;
  virtual void CustomMouseEnter(Point2D) override;
  virtual void CustomMouseLeave(Point2D) override;
  virtual Point2D GetChildPos() const override {return Point2D(left,top); }
  virtual void RemoveChild() override;
protected:
  UIMarginBox(std::weak_ptr<Window> parent_window, int top, int right, int bottom, int left);
private:
  inline bool IsInside(Point2D) const;
  int top, right, bottom, left;
};

} // namespace AlgAudio

#endif // UIMARGINBOX_HPP
