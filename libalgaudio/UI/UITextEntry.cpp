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

UITextEntry::UITextEntry(std::weak_ptr<Window> w, std::string t) : UIWidget(w), text(t) {
}

std::shared_ptr<UITextEntry> UITextEntry::Create(std::weak_ptr<Window> parent_window, std::string t){
  auto res = std::shared_ptr<UITextEntry>(new UITextEntry(parent_window, t));
  res->Init();
  return res;
}

void UITextEntry::Init(){
  SetMinimalSize(Size2D(30, fontsize + 2));
  UpdateText();
}

void UITextEntry::CustomDraw(DrawContext& c){
  Color bg_color = Theme::Get("textentry-bg");
  if(GetIsFocused()) bg_color = bg_color.Lighter(0.1);
  c.SetColor(bg_color);
  c.DrawRect(Rect(Point2D(0,0), c.Size()));

  c.DrawText(text_texture, Theme::Get( (text!="") ? "textentry-text" : "textentry-default"), Point2D( c.Size().height/2 - text_texture->GetSize().height/2 + 1 ,0));
}

void UITextEntry::SetText(std::string t){
  text = t;
  UpdateText();
}

void UITextEntry::OnKeyboard(KeyData k){
  if(k.pressed == false) return; // Ignore key up events
  if( (k.type == KeyData::KeyType::Text && digits_only == false) ||
       k.type == KeyData::KeyType::Digit
     ) {
    text += k.symbol;
    on_edited.Happen();
    UpdateText();
  }else if(k.type == KeyData::KeyType::Backspace){
    if(text.length() >= 1){
      text.pop_back();
      on_edited.Happen();
      UpdateText();
    }
  }
}

void UITextEntry::OnFocusChanged(){
  SDLMain::SetTextInput( GetIsFocused() );
  SetNeedsRedrawing();
}

void UITextEntry::SetFontSize(int size){
  fontsize = size;
  SetMinimalSize(Size2D(30, fontsize + 2));
  UpdateText();
}

void UITextEntry::UpdateText(){
  text_texture = TextRenderer::Render(window, FontParams("Dosis-Regular", fontsize), (text!="") ? text : default_text);
  SetNeedsRedrawing();
}

} // namespace AlgAudio
