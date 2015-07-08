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
  if(pressed)
    c.SetColor(220,90,90);
  else if(pointed)
    c.SetColor(100,50,250);
  else
    c.SetColor(50,50,50);

  c.DrawLine(0,0,c.width,0);
  c.DrawLine(0,0,0,c.height);
  c.DrawLine(0,0,c.width,c.height);
  c.DrawLine(0,c.height,c.width,0);
  c.DrawLine(c.width,0,c.width,c.height);
  c.DrawLine(0,c.height,c.width,c.height);
}

void UIButton::OnMouseButton(bool down, short b,int,int){
  if(down == true && b == SDL_BUTTON_LEFT){
    pressed = 1;
    SetNeedsRedrawing();
  }else if(down == false && b == SDL_BUTTON_LEFT && pressed == 1){
    pressed = 0;
    SetNeedsRedrawing();
    on_clicked.Happen();
  }
}

void UIButton::OnMotionEnter(){
  //std::cout << "Button entered" << std::endl;
  pointed = true;
  SetNeedsRedrawing();
}

void UIButton::OnMotionLeave(){
  //std::cout << "Button left" << std::endl;
  pressed = 0;
  pointed = 0;
  SetNeedsRedrawing();
}

} // namespace AlgAudio
