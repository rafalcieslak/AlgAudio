#include "UI/UIButton.hpp"
#include <SDL2/SDL.h>
#include <iostream>
#include "TextRenderer.hpp"

namespace AlgAudio{

UIButton::UIButton(std::weak_ptr<Window> w, std::string t) : UIWidget(w), text(t){
  UpdateTexture();
}

std::shared_ptr<UIButton> UIButton::Create(std::weak_ptr<Window> w, std::string text){
  std::shared_ptr<UIButton> res(new UIButton(w,text));
  return res;
}

void UIButton::CustomDraw(DrawContext& c){
  c.SetColor(255,255,255);
  c.Clear();

  if(pressed)
    c.SetColor(220,90,90);
  else if(pointed)
    c.SetColor(100,50,250);
  else
    c.SetColor(50,50,50);

  c.DrawLine(0,0,c.width-1,0);
  c.DrawLine(0,0,0,c.height-1);
  c.DrawLine(0,0,c.width-1,c.height-1);
  c.DrawLine(0,c.height-1,c.width-1,0);
  c.DrawLine(c.width-1,0,c.width-1,c.height-1);
  c.DrawLine(0,c.height-1,c.width-1,c.height-1);
  c.DrawTexture(texture, 5, 5);
}

void UIButton::SetText(std::string t){
  text = t;
  UpdateTexture();
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

void UIButton::UpdateTexture(){
  texture = TextRenderer::Render(window, FontParrams("Dosis-Regular",16), text, SDL_Color{0,0,0,255});
  SetRequestedSize(texture->GetSize() + Size2D(10,10));
  SetNeedsRedrawing();
}

void UIButton::OnMotionEnter(int, int){
  //std::cout << "Button entered" << std::endl;
  pointed = true;
  SetNeedsRedrawing();
}

void UIButton::OnMotionLeave(int, int){
  //std::cout << "Button left" << std::endl;
  pressed = 0;
  pointed = 0;
  SetNeedsRedrawing();
}

} // namespace AlgAudio
