#ifndef UILAYERED_HPP
#define UILAYERED_HPP
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

class UILayered: public UIContainerMultiple{
public:
  static std::shared_ptr<UILayered> Create(std::weak_ptr<Window> parent_window);
  void Insert(const std::shared_ptr<UIWidget>& child);
  virtual void Clear() override;
  virtual void CustomDraw(DrawContext& c) override;
  virtual void CustomResize(Size2D) override;
  virtual void OnChildRequestedSizeChanged() override;
  virtual void OnChildVisibilityChanged() override;
  virtual bool CustomMousePress(bool,MouseButton,Point2D) override;
  virtual void CustomMouseEnter(Point2D) override;
  virtual void CustomMouseLeave(Point2D) override;
  virtual void CustomMouseMotion(Point2D p1, Point2D p2) override;
  virtual Point2D GetChildPos(std::shared_ptr<UIWidget>) const override {return Point2D(0,0);}
private:
  UILayered(std::weak_ptr<Window> parent_window);
  std::list<std::shared_ptr<UIWidget>> children;
  void RecalculateSize();
  std::shared_ptr<UIWidget> GetTopChild() const;
};

} // namespace AlgAudio

#endif // UILAYERED_HPP
