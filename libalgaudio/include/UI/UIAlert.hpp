#ifndef UIALERT_HPP
#define UIALERT_HPP
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
#include "UIBox.hpp"
#include "UIMarginBox.hpp"
#include "Theme.hpp"
#include "Alertable.hpp"

namespace AlgAudio{

class UIHBox;
class UIVBox;
class UILabel;
class UIButton;
class Window;

class UIAlert : public UIMarginBox{
public:
  static std::shared_ptr<UIAlert> Create(std::weak_ptr<Window> parent_window, std::string text);
  void SetText(std::string);
  void SetType(AlertType);
  // Arg: pressed button id
  Signal<ButtonID> on_button_pressed;
  struct ButtonData{
    std::string name;
    ButtonID id;
    Color color;
    ButtonData(std::string t, ButtonID id_, Color c = Theme::Get("bg-button-neutral"))
      : name(t), id(id_), color(c)
      {}
  };
  void SetButtons(std::initializer_list<ButtonData>);
private:
  UIAlert(std::weak_ptr<Window> parent_window);
  void Init(std::string);
  std::shared_ptr<UIVBox> main_box;
  std::shared_ptr<UIHBox> child_buttons_box;
  std::shared_ptr<UILabel> child_label;
  std::vector<std::shared_ptr<UIButton>> buttons;
};

} // namespace AlgAudio

#endif // UIALERT_HPP
