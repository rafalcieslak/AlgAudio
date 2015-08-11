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
#include "UI/UITextArea.hpp"
#include <SDL2/SDL.h>
#include "TextRenderer.hpp"
#include "Color.hpp"

namespace AlgAudio{

UITextArea::UITextArea(std::weak_ptr<Window> w, Color cfg, Color cbg) : UIWidget(w), c_fg(cfg), c_bg(cbg){
}

std::shared_ptr<UITextArea> UITextArea::Create(std::weak_ptr<Window> w, Color c_fg, Color c_bg){
  std::shared_ptr<UITextArea> res(new UITextArea(w, c_fg, c_bg));
  return res;
}

void UITextArea::Clear(){
  text.clear();
  textures.clear();
}
void UITextArea::CustomDraw(DrawContext& c){
  c.SetColor(c_bg);
  c.Fill();
  const int spacing = 12;
  if(!bottom_alligned){
    unsigned int n = 0;
    for(int y = 2; y < c.Size().height && n < text.size(); y += spacing, n++){
      c.DrawText(textures[n], c_fg, 2, y);
    }
  }else{
    int n = text.size()-1;
    for(int y = c.Size().height-2-spacing; y > 0 - spacing && n >= 0; y -= spacing, n--){
      c.DrawText(textures[n], c_fg, 2, y);
    }
  }
}
void UITextArea::Push(std::string s){
  auto vs = Utilities::SplitString(s,"\n");
  for(auto& l : vs) PushLine(l);
}
void UITextArea::PushLine(std::string s){
  text.push_back(s);
  textures.push_back( TextRenderer::Render(window, FontParams("FiraMono-Regular",10), s) );
  SetNeedsRedrawing();
}

std::string UITextArea::GetAllText(){
  return Utilities::JoinString(text,"\n");
}

} // namespace AlgAudio
