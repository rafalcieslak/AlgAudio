/*
This file is part of AlgAudio.

AlgAudio, Copyright (C) 2015 CeTA - Audiovisual Technology Center

AlgAudio is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

AlgAudio is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with AlgAudio.  If not, see <http://www.gnu.org/licenses/>.
*/
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
