#ifndef UILIST_HPP
#define UILIST_HPP
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
#include "UIButton.hpp"
#include "UIBox.hpp"

namespace AlgAudio{

class UIList : public UIVBox{
public:
  static std::shared_ptr<UIList> Create(std::weak_ptr<Window> parent_window);
  void AddItem(std::string id, std::string text);
  void Clear();
  void SetColors(Color standard, Color highlight);
  void SetHighlight(std::string id);
  Signal<std::string> on_pointed;
  Signal<std::string> on_clicked;
private:
  UIList(std::weak_ptr<Window> parent_window);
  std::map<std::string, std::shared_ptr<UIButton>> ids_to_buttons;
  std::shared_ptr<UIButton> highlighted = nullptr;
  Color standard_color = Theme::Get("bg-button-neutral");
  Color highlight_color = Theme::Get("bg-button-positive");
};

} // namespace AlgAudio

#endif // UILIST_HPP
