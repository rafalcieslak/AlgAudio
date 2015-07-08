#include "UI/UIButton.hpp"

namespace AlgAudio{

UIButton::UIButton(std::string t) : text(t){
}

void UIButton::Draw(DrawContext& c){
  c.SetColor(50,50,50);
  c.DrawLine(0,0,100,100);
}

} // namespace AlgAudio
