#ifndef UITEXTAREA_HPP
#define UITEXTAREA_HPP
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
#include <vector>
#include "UIWidget.hpp"

namespace AlgAudio{

class UITextArea : public UIWidget{
public:
  static std::shared_ptr<UITextArea> Create(std::weak_ptr<Window> parent_window, Color c_fg, Color c_bg = Color(0,0,0));
  void Clear();
  void Push(std::string);
  void PushLine(std::string);
  void SetBottomAligned(bool b){
    bottom_alligned = b;
  }
  virtual void CustomDraw(DrawContext& c) override;
  std::string GetAllText();
private:
  UITextArea(std::weak_ptr<Window> parent_window, Color c_fg, Color c_bg = Color(0,0,0));
  std::vector<std::string> text;
  std::vector<std::shared_ptr<SDLTextTexture>> textures;
  bool bottom_alligned = false;
  Color c_fg, c_bg;
};

} // namespace AlgAudio

#endif // UITEXTAREA_HPP
