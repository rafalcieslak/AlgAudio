#include "UI/UIButton.hpp"

namespace AlgAudio{

UIButton::UIButton(std::string t) : text(t){
}

void UIButton::Draw(DrawContext& c){
  c.SetColor(50,50,50);
  c.DrawLine(0,0,c.width,0);
  c.DrawLine(0,0,0,c.height);
  c.DrawLine(0,0,c.width,c.height);
  c.DrawLine(0,c.height,c.width,0);
  c.DrawLine(c.width,0,c.width,c.height);
  c.DrawLine(0,c.height,c.width,c.height);
}

} // namespace AlgAudio
