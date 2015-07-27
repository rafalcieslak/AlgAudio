#ifndef UILABEL_HPP
#define UILABEL_HPP
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

class UILabel : public UIWidget{
public:
  static std::shared_ptr<UILabel> Create(std::weak_ptr<Window> parent_window, std::string text, int size = 16, std::string color = "text-generic");
  void SetText(std::string);
  void SetTextColor(std::string colorname);
  void SetTextSize(int fontsize);
  void SetBold(bool);
  void SetAlignment(HorizAlignment h, VertAlignment v = VertAlignment_CENTERED);
  virtual void CustomDraw(DrawContext& c) override;
private:
  UILabel(std::weak_ptr<Window> parent_window, std::string text, int size = 16, std::string color = "text-generic");
  std::string text;
  std::string color;
  HorizAlignment horiz_alignment = HorizAlignment_CENTERED;
  VertAlignment vert_alignment = VertAlignment_CENTERED;
  int fontsize;
  bool bold = false;
  std::shared_ptr<SDLTextTexture> texture;
  void UpdateTexture();
};

} // namespace AlgAudio

#endif // UILABEL_HPP
