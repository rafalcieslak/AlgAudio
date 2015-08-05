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
#include "UI/UISlider.hpp"
#include "Module.hpp"
#include "ModuleTemplate.hpp"

namespace AlgAudio{

UISlider::UISlider(std::weak_ptr<Window> parent_window, std::shared_ptr<ParramController> c) : UIWidget(parent_window), controller(c) {
  SetMinimalSize(Size2D(50,12));
}

std::shared_ptr<UISlider> UISlider::Create(std::weak_ptr<Window> parent_window, std::shared_ptr<ParramController> controller){
  auto res = std::shared_ptr<UISlider>(new UISlider(parent_window, controller));
  if(controller->templ->name == "Frequency"){
    controller->Set(60.0);
  }
  return res;
}


void UISlider::CustomDraw(DrawContext& c){
  c.SetColor(Color(0,0,0));
  c.DrawRect(0,0,c.Size().width,c.Size().height);
}

} // namespace AlgAudio
