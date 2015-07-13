#include "UI/UIButton.hpp"
#include <SDL2/SDL.h>
#include <iostream>
#include "TextRenderer.hpp"
#include "Theme.hpp"

namespace AlgAudio{

UIButton::UIButton(std::weak_ptr<Window> w, std::string t) : UIWidget(w), text(t){
  bg_color = Theme::Get("bg-button-neutral");
  text_color = Theme::Get("text-button");
  UpdateTexture();
}

std::shared_ptr<UIButton> UIButton::Create(std::weak_ptr<Window> w, std::string text){
  std::shared_ptr<UIButton> res(new UIButton(w,text));
  return res;
}

void UIButton::CustomDraw(DrawContext& c){
  c.Clear();

  if(pressed)
    c.SetColor(220,90,90);
  else if(pointed)
    c.SetColor(100,50,250);
  else
    c.SetColor(50,50,50);

  c.SetColor(bg_color);
  c.DrawRect(0,0,c.width,c.height);

  c.DrawLine(0,0,c.width-1,0);
  c.DrawLine(0,0,0,c.height-1);
  //c.DrawLine(0,0,c.width-1,c.height-1);
  //c.DrawLine(0,c.height-1,c.width-1,0);
  c.DrawLine(c.width-1,0,c.width-1,c.height-1);
  c.DrawLine(0,c.height-1,c.width-1,c.height-1);
  c.SetColor(text_color);
  c.DrawTexture(texture, c.width/2 - texture->GetSize().width/2, c.height/2 - texture->GetSize().height/2);
}

void UIButton::SetText(std::string t){
  text = t;
  UpdateTexture();
}

void UIButton::SetColors(Color text, Color background){
  text_color = text;
  bg_color = background;
  UpdateTexture();
  SetNeedsRedrawing();
}

void UIButton::OnMouseButton(bool down, short b,Point2D){
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
  texture = TextRenderer::Render(window, FontParrams("Dosis-Regular",16), text, text_color, bg_color);
  SetRequestedSize(texture->GetSize() + Size2D(10,10));
  SetNeedsRedrawing();
}

void UIButton::OnMotionEnter(Point2D){
  //std::cout << "Button entered" << std::endl;
  pointed = true;
  SetNeedsRedrawing();
}

void UIButton::OnMotionLeave(Point2D){
  //std::cout << "Button left" << std::endl;
  pressed = 0;
  pointed = 0;
  SetNeedsRedrawing();
}

} // namespace AlgAudio
