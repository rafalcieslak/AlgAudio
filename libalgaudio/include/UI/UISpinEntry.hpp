#ifndef UISPINENTRY_HPP
#define UISPINENTRY_HPP
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

#include "UI/UIBox.hpp"

namespace AlgAudio{

class UIButton;
class UITextEntry;
class UISeparator;
class UILabel;

class UISpinEntry : public UIHBox{
public:
  static std::shared_ptr<UISpinEntry> Create(std::weak_ptr<Window> parent_window, std::string name = "", int value = 0);
  Signal<> on_changed;
  void SetValue(int v);
  int GetValue() const;
private:
  UISpinEntry(std::weak_ptr<Window>);
  void Init(std::string name = "", int value = 0);
  
  std::shared_ptr<UISeparator> child_sep;
  std::shared_ptr<UILabel> child_label;
  std::shared_ptr<UITextEntry> child_entry;
  std::shared_ptr<UIVBox> child_box_buttons;
    std::shared_ptr<UIButton> child_button_up;
    std::shared_ptr<UIButton> child_button_down;
};

} // namespace AlgAudio

#endif // UISPINENTRY_HPP
