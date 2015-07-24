#ifndef UICHECKBOX_HPP
#define UICHECKBOX_HPP
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
#include "UIClickable.hpp"
#include "Theme.hpp"

namespace AlgAudio{

class UIHBox;
class UILabel;
class UIButton;

class UICheckbox : public UIClickable{
public:
  static std::shared_ptr<UICheckbox> Create(std::weak_ptr<Window> parent_window, std::string text);
  Signal<bool> on_toggled;
  void SetText(std::string);
  virtual void CustomDraw(DrawContext& c) override;
  virtual void CustomResize(Size2D s) override;
  virtual void OnChildRequestedSizeChanged() override;
  bool active = false;
private:
  UICheckbox(std::weak_ptr<Window> parent_window, std::string text);
  void Init();
  void UpdateColors();
  std::string text;
  std::shared_ptr<UIHBox> child_box;
  std::shared_ptr<UIButton> child_button;
  std::shared_ptr<UILabel> child_label;
};

} // namespace AlgAudio

#endif // UICHECKBOX_HPP
