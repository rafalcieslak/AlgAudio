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
  c.SetColor(Theme::Get(color).ZeroAlpha());
  c.Fill();
  c.DrawTexture(texture, c.width/2 - texture->GetSize().width/2, c.height/2 - texture->GetSize().height/2);
}

void UILabel::SetText(std::string t){
  text = t;
  UpdateTexture();
}

void UILabel::UpdateTexture(){
  texture = TextRenderer::Render(window, FontParrams("Dosis-Regular",fontsize), text, Theme::Get(color), Theme::Get("bg-main"));
  SetRequestedSize(texture->GetSize() + Size2D(10,10));
  SetNeedsRedrawing();
}

} // namespace AlgAudio
