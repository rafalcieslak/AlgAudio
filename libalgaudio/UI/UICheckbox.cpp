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
#include "UI/UICheckbox.hpp"
#include "UI/UIBox.hpp"
#include "UI/UILabel.hpp"
#include "UI/UIButton.hpp"
#include "Theme.hpp"
#include <iostream>

namespace AlgAudio{

UICheckbox::UICheckbox(std::weak_ptr<Window> w, std::string t) : UIWidget(w), text(t){
}

std::shared_ptr<UICheckbox> UICheckbox::Create(std::weak_ptr<Window> w, std::string text){
  std::shared_ptr<UICheckbox> res(new UICheckbox(w,text));
  res->Init();
  return res;
}

void UICheckbox::CustomDraw(DrawContext& c){
  child_box->Draw(c);
}

void UICheckbox::Init(){
  child_box = UIHBox::Create(window);
  child_box->parent = shared_from_this();
  child_button = UIButton::Create(window,"   ");
  child_label = UILabel::Create(window,text,12);
  child_box->Insert(child_button, UIBox::PackMode::TIGHT);
  child_box->Insert(child_label, UIBox::PackMode::TIGHT);

  child_button->SetBorder(false);
  child_button->SetCustomSize(Size2D(20,20));
  child_button->SetInnerMargin(2);
  child_box->SetPadding(8);

  SetBackColor(Theme::Get("bg-main"));
  on_clicked.SubscribeForever([&](){
    if(active){
      active = false;
      child_button->SetText(" ");
      UpdateColors();
      on_toggled.Happen(false);
    }else{
      active = true;
      child_button->SetText("X");
      UpdateColors();
      on_toggled.Happen(true);
    };
  });
  on_pointed.SubscribeForever([&](bool){
    UpdateColors();
  });
}

void UICheckbox::UpdateColors(){
  Color button_bg;
  if(active) button_bg = Theme::Get("bg-button-positive");
  else       button_bg = Theme::Get("bg-button-neutral");

  if(pointed){
    SetBackColor(Theme::Get("bg-main").Lighter(0.04));
    button_bg = button_bg.Lighter(0.07);
  }else{
    SetBackColor(Theme::Get("bg-main"));
  }
  child_button->SetColors(Theme::Get("text-button"), button_bg);
}

void UICheckbox::CustomResize(Size2D s){
  child_box->Resize(s);
}

void UICheckbox::SetText(std::string t){
  child_label->SetText(t);
}
void UICheckbox::OnChildRequestedSizeChanged(){
  SetMinimalSize(child_box->GetRequestedSize());
}


} // namespace AlgAudio
