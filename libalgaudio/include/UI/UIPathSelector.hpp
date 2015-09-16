#ifndef UIPATHSELECTOR_HPP
#define UIPATHSELECTOR_HPP
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

namespace AlgAudio{

class UIHBox;
class UITextEntry;
class UIButton;

class UIPathSelector : public UIHBox{
public:
  static std::shared_ptr<UIPathSelector> Create(std::weak_ptr<Window> parent_window, std::string path = "");
  Signal<std::string> on_selected;
  void SetPath(std::string);
  void OpenDialog();
  std::string GetPath() const {return path;}
private:
  UIPathSelector(std::weak_ptr<Window> parent_window, std::string path = "");
  void Init();
  std::string path;
  std::shared_ptr<UIButton> child_button;
  std::shared_ptr<UITextEntry> child_entry;
};

} // namespace AlgAudio

#endif // UIPATHSELECTOR_HPP
