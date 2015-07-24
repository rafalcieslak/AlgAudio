#ifndef UIANIMDRAWER_HPP
#define UIANIMDRAWER_HPP
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

class UIAnimDrawer: public UIWidget{
public:
  static std::shared_ptr<UIAnimDrawer> Create(std::weak_ptr<Window> parent_window, Direction dir);
  void Insert(std::shared_ptr<UIWidget> child);
  void StartShow(float time_to_show);
  void StartHide(float time_to_hide);
  virtual void CustomDraw(DrawContext& c) override;
  virtual void CustomResize(Size2D s) override;
  virtual void OnChildRequestedSizeChanged() override;
  virtual void OnChildVisibilityChanged() override;
  virtual void OnMouseButton(bool down, short b,Point2D) override;
  virtual void OnMotion(Point2D,Point2D) override;
  virtual void OnMotionEnter(Point2D) override;
  virtual void OnMotionLeave(Point2D) override;

  Signal<> on_show_complete;
  Signal<> on_hide_complete;
private:
  UIAnimDrawer(std::weak_ptr<Window> parent_window, Direction dir);

  void Step(float delta);
  Point2D GetCurrentOffset() const;
  Size2D GetInnerSize() const;
  void UpdateRequestedSize();

  Direction direction;
  float phase;
  // 0 = stationary, 1 - showing, -1 - hiding
  int state = 0;
  float time_to_finish;
  std::shared_ptr<Subscription> anim = std::make_shared<Subscription>();

  std::shared_ptr<UIWidget> child;
};

} // namespace AlgAudio

#endif // UIANIMDRAWER_HPP
