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
  c.Clear();
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
