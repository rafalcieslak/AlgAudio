#include "UI/UITextArea.hpp"
#include <SDL2/SDL.h>
#include "TextRenderer.hpp"
#include "Utilities.hpp"

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
  c.Clear();
  const int spacing = 12;
  if(!bottom_alligned){
    unsigned int n = 0;
    for(int y = 2; y < c.height && n < text.size(); y += spacing, n++){
      c.DrawTexture(textures[n], 2, y);
    }
  }else{
    int n = text.size()-1;
    for(int y = c.height-2; y > 0 - spacing && n >= 0; y -= 12, n--){
      c.DrawTexture(textures[n], 2, y);
    }
  }
}
void UITextArea::Push(std::string s){
  auto vs = Utilities::SplitString(s,"\n");
  for(auto& l : vs) PushLine(l);
}
void UITextArea::PushLine(std::string s){
  text.push_back(s);
  textures.push_back( TextRenderer::Render(window, FontParrams("FiraMono-Regular",10), s, c_fg) );
  SetNeedsRedrawing();
}

} // namespace AlgAudio
