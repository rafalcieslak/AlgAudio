#ifndef UIBUTTON_HPP
#define UIBUTTON_HPP
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

class UIButton : public UIClickable{
public:
  static std::shared_ptr<UIButton> Create(std::weak_ptr<Window> parent_window, std::string text);
  void SetText(std::string);
  void SetColors(Color text, Color background);
  virtual void CustomDraw(DrawContext& c) override;
  void SetFontSize(int fontsize);
  void SetBorder(bool enabled);
  void SetInnerMargin(int m);
private:
  UIButton(std::weak_ptr<Window> parent_window, std::string text);
  std::string text;
  int fontsize = 16;
  bool border_enabled = true;
  int inner_margin = 5;
  std::shared_ptr<SDLTextTexture> texture;
  Color bg_color;
  Color text_color;
  void UpdateTexture();
  Color GetBgColor() const;
};

} // namespace AlgAudio

#endif // UIBUTTON_HPP
