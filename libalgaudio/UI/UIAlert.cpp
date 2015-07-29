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

#include "UI/UIAlert.hpp"
#include "UI/UILabel.hpp"
#include "UI/UIButton.hpp"

namespace AlgAudio{


UIAlert::UIAlert(std::weak_ptr<Window> parent_window) : UIVBox(parent_window)
{
}

void UIAlert::Init(std::string text){
  child_buttons_box = UIHBox::Create(window);
  child_buttons_box->SetPadding(4);
  child_label = UILabel::Create(window, text, 14);
  Insert(child_label, PackMode::TIGHT);
  Insert(child_buttons_box, PackMode::TIGHT);
  SetBackColor(Theme::Get("bg-alert"));
  SetCustomSize(Size2D(300,160));
}

std::shared_ptr<UIAlert> UIAlert::Create(std::weak_ptr<Window> parent_window, std::string text){
  std::shared_ptr<UIAlert> res(new UIAlert(parent_window));
  res->Init(text);
  return res;
}


void UIAlert::SetButtons(std::initializer_list<ButtonData> list){
  buttons.clear();
  for(ButtonData bd : list){
    auto button = UIButton::Create(window,bd.name);
    button->SetColor(bd.color);
    button->on_clicked.SubscribeForever([id = bd.id,this](){
      on_button_pressed.Happen(id);
    });
    buttons.push_back(button);
  }
}

}
