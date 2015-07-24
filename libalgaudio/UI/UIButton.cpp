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
#include "UI/UIButton.hpp"
#include <iostream>
#include "TextRenderer.hpp"
#include "Theme.hpp"

namespace AlgAudio{

UIButton::UIButton(std::weak_ptr<Window> w, std::string t) : UIClickable(w), text(t){
  bg_color = Theme::Get("bg-button-neutral");
  text_color = Theme::Get("text-button");
  UpdateTexture();

  // UIClickable events
  on_pointed.SubscribeForever([&](bool){
    SetNeedsRedrawing();
  });
  on_pressed.SubscribeForever([&](bool){
    SetNeedsRedrawing();
  });
}

std::shared_ptr<UIButton> UIButton::Create(std::weak_ptr<Window> w, std::string text){
  std::shared_ptr<UIButton> res(new UIButton(w,text));
  return res;
}

Color UIButton::GetBgColor() const{
  if(pressed)
    return bg_color.Lighter(0.16);
  else if(pointed)
    return bg_color.Lighter(0.07);
  else
    return bg_color;
}

void UIButton::CustomDraw(DrawContext& c){
  c.SetColor(GetBgColor());
  int w = c.Size().width;
  int h = c.Size().height;
  c.DrawRect(0,0,w,h);

  if(border_enabled){
    c.SetColor(bg_color.Darker(0.15));
    c.DrawLine(0,0,w-1,0);
    c.DrawLine(0,0,0,h-1);
    c.DrawLine(w-1,0,w-1,h-1);
    c.DrawLine(0,h-1,w-1,h-1);
    c.DrawLine(1,1,w-2,1);
    c.DrawLine(1,1,1,h-2);
    c.DrawLine(w-2,1,w-2,h-2);
    c.DrawLine(1,h-2,w-2,h-2);
  }
  
  c.SetColor(text_color);
  c.DrawText(
    texture,
    text_color,
    w/2  - texture->GetSize().width/2  + ((pressed)?2:0),
    h/2 - texture->GetSize().height/2 + ((pressed)?1:0)
  );
}

void UIButton::SetText(std::string t){
  text = t;
  UpdateTexture();
}

void UIButton::SetColors(Color text, Color background){
  text_color = text;
  bg_color = background;
  UpdateTexture();
}

void UIButton::SetFontSize(int fs){
  fontsize = fs;
  UpdateTexture();
}

void UIButton::UpdateTexture(){
  // Todo: Blended render, so that the same text texture can be used for any BG
  texture = TextRenderer::Render(window, FontParrams("Dosis-Regular",fontsize), text);
  SetRequestedSize(texture->GetSize() + Size2D(10,10));
  SetNeedsRedrawing();
}

void UIButton::SetBorder(bool enabled){
  border_enabled = enabled;
  SetNeedsRedrawing();
}

} // namespace AlgAudio
