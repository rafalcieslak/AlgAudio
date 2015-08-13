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

#include "UI/UITextEntry.hpp"
#include "TextRenderer.hpp"
#include "SDLMain.hpp"

namespace AlgAudio{

UITextEntry::UITextEntry(std::shared_ptr<Window> w) : UIWidget(w) {
  SetMinimalSize(Size2D(30,18));
}

std::shared_ptr<UITextEntry> UITextEntry::Create(std::shared_ptr<Window> parent_window){
  return std::shared_ptr<UITextEntry>(new UITextEntry(parent_window));
}

void UITextEntry::CustomDraw(DrawContext& c){
  Color bg_color = Theme::Get("textentry-bg");
  if(GetIsFocused()) bg_color = bg_color.Lighter(0.1);
  c.SetColor(bg_color);
  int w = c.Size().width;
  int h = c.Size().height;
  c.DrawRect(0,0,w,h);

  c.DrawText(text_texture, Theme::Get("textentry-text"), 1,0);
}

void UITextEntry::SetText(std::string t){
  text = t;
  text_texture = TextRenderer::Render(window, FontParams("Molengo-Regular",14), text);
}

void UITextEntry::OnKeyboard(KeyData k){
  if(k.pressed == false) return; // Ignore key up events
  if(k.type == KeyData::KeyType::Text){
    text += k.symbol;
    text_texture = TextRenderer::Render(window, FontParams("Molengo-Regular",14), text);
    SetNeedsRedrawing();
  }else if(k.type == KeyData::KeyType::Backspace){
    if(text.length() >= 1){
      text.pop_back();
      text_texture = TextRenderer::Render(window, FontParams("Molengo-Regular",14), text);
      SetNeedsRedrawing();
    }
  }
}

void UITextEntry::OnFocusChanged(){
  SDLMain::SetTextInput( GetIsFocused() );
  SetNeedsRedrawing();
}

} // namespace AlgAudio
