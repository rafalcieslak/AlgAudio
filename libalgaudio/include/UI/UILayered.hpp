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
#include "UIWidget.hpp"

namespace AlgAudio{

class UILayered: public UIWidget{
public:
  static std::shared_ptr<UILayered> Create(std::weak_ptr<Window> parent_window);
  void Insert(std::shared_ptr<UIWidget> child);
  virtual void CustomDraw(DrawContext& c) override;
  virtual void CustomResize(Size2D) override;
  virtual void OnChildRequestedSizeChanged() override;
private:
  UILayered(std::weak_ptr<Window> parent_window);
  struct ChildData{
    std::shared_ptr<UIWidget> child;
    bool visible;
  }
  std::list<std::shared_ptr<UIWidget>> children;
};

} // namespace AlgAudio

#endif // UILAYERED_HPP
