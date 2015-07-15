#include "UI/UIClickable.hpp"
#include <SDL2/SDL.h>

namespace AlgAudio{

UIClickable::UIClickable(std::weak_ptr<Window> w) : UIWidget(w){
}

void UIClickable::OnMouseButton(bool down, short b,Point2D){
  if(down == true && b == SDL_BUTTON_LEFT){
    pressed = true;
    on_pressed.Happen(true);
  }else if(down == false && b == SDL_BUTTON_LEFT && pressed == true){
    pressed = false;
    on_pressed.Happen(false);
    on_clicked.Happen();
  }
}

void UIClickable::OnMotionEnter(Point2D){
  pointed = true;
  on_pointed.Happen(true);
}

void UIClickable::OnMotionLeave(Point2D){
  if(pressed){
    pressed = false;
    on_pressed.Happen(false);
  }
  if(pointed){
    pointed = false;
    on_pointed.Happen(false);
  }
}

} // namespace AlgAudio
