#ifndef UIPOSITION_HPP
#define UIPOSITION_HPP
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

class UIPosition: public UIWidget{
public:
  static std::shared_ptr<UIPosition> Create(std::weak_ptr<Window> parent_window);
  void Insert(std::shared_ptr<UIWidget> child, Point2D pos);
  void Remove();
  virtual void CustomDraw(DrawContext& c) override;
  virtual void OnChildRequestedSizeChanged() override;
  virtual void OnChildVisibilityChanged() override;
  virtual bool CustomMousePress(bool down, short b,Point2D) override;
  virtual void CustomMouseMotion(Point2D,Point2D) override;
  virtual void CustomMouseEnter(Point2D) override;
  virtual void CustomMouseLeave(Point2D) override;
private:
  UIPosition(std::weak_ptr<Window> parent_window);
  inline bool IsInside(Point2D) const;
  Point2D position;
  std::shared_ptr<UIWidget> child;
};

} // namespace AlgAudio

#endif // UIPOSITIONS_HPP
