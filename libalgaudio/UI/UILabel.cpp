#include "UI/UILabel.hpp"
#include <SDL2/SDL.h>
#include <iostream>
#include "TextRenderer.hpp"

namespace AlgAudio{

UILabel::UILabel(std::weak_ptr<Window> w, std::string t, int size) : UIWidget(w), text(t), fontsize(size){
  UpdateTexture();
}

std::shared_ptr<UILabel> UILabel::Create(std::weak_ptr<Window> w, std::string text, int size){
  std::shared_ptr<UILabel> res(new UILabel(w,text,size));
  return res;
}

void UILabel::CustomDraw(DrawContext& c){
  c.SetColor(255,255,255);
  c.Clear();
  c.DrawTexture(texture, c.width/2 - texture->GetSize().width/2, c.height/2 - texture->GetSize().height/2);
}

void UILabel::SetText(std::string t){
  text = t;
  UpdateTexture();
}

void UILabel::UpdateTexture(){
  texture = TextRenderer::Render(window, FontParrams("Dosis-Regular",fontsize), text, SDL_Color{0,0,0,255});
  SetRequestedSize(texture->GetSize() + Size2D(10,10));
  SetNeedsRedrawing();
}

} // namespace AlgAudio
