#include "UI/UIButton.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

namespace AlgAudio{

UIButton::UIButton(std::string t) : text(t){
}

std::shared_ptr<UIButton> UIButton::Create(std::string text){
  std::shared_ptr<UIButton> res(new UIButton(text));
  return res;
}

void UIButton::CustomDraw(const DrawContext& c){
  c.SetColor(50,50,50);
  c.DrawLine(0,0,c.width,0);
  c.DrawLine(0,0,0,c.height);
  c.DrawLine(0,0,c.width,c.height);
  c.DrawLine(0,c.height,c.width,0);
  c.DrawLine(c.width,0,c.width,c.height);
  c.DrawLine(0,c.height,c.width,c.height);
}

void UIButton::OnMouseButton(bool down, short b,int x,int y){
  if(down == 1 && b == SDL_BUTTON_LEFT)
    std::cout << "Button pressed" << std::endl;
}

} // namespace AlgAudio
