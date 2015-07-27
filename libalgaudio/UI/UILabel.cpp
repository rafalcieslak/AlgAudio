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
#include "UI/UILabel.hpp"
#include <SDL2/SDL.h>
#include <iostream>
#include "Theme.hpp"
#include "TextRenderer.hpp"

namespace AlgAudio{

UILabel::UILabel(std::weak_ptr<Window> w, std::string t, int size, std::string c) : UIWidget(w), text(t), color(c), fontsize(size){
  UpdateTexture();
}

std::shared_ptr<UILabel> UILabel::Create(std::weak_ptr<Window> w, std::string text, int size, std::string color){
  std::shared_ptr<UILabel> res(new UILabel(w,text,size,color));
  return res;
}

void UILabel::CustomDraw(DrawContext& c){
  Point2D pos = Utilities::Align(horiz_alignment, vert_alignment, texture->GetSize(), c.Size());
  c.DrawText(texture, Theme::Get(color), pos.x, pos.y);
}

void UILabel::SetText(std::string t){
  text = t;
  UpdateTexture();
}
void UILabel::SetTextColor(std::string c){
  color = c;
  UpdateTexture();
}
void UILabel::SetTextSize(int size){
  fontsize = size;
  UpdateTexture();
}
void UILabel::SetBold(bool b){
  bold = b;
  UpdateTexture();
}
void UILabel::SetAlignment(HorizAlignment h, VertAlignment v){
  horiz_alignment = h;
  vert_alignment = v;
  SetNeedsRedrawing();
}
void UILabel::UpdateTexture(){
  texture = TextRenderer::Render(window, FontParrams((bold)?"Dosis-Bold":"Dosis-Regular",fontsize), text);
  SetMinimalSize(texture->GetSize());
  SetNeedsRedrawing();
}

} // namespace AlgAudio
