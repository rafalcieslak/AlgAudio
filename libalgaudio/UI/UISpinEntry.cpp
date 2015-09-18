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

#include "UI/UISpinEntry.hpp"
#include <cmath>
#include "UI/UIButton.hpp"
#include "UI/UITextEntry.hpp"
#include "UI/UISeparator.hpp"
#include "UI/UILabel.hpp"

namespace AlgAudio{
  
UISpinEntry::UISpinEntry(std::weak_ptr<Window> w) : UIHBox(w) {
}

void UISpinEntry::Init(std::string name, int v){
  child_sep = UISeparator::Create(window);
  child_label = UILabel::Create(window,name + " ",14);
  child_entry = UITextEntry::Create(window);
  child_entry->SetText( std::to_string(v) );
  child_entry->SetFontSize(16);
  child_entry->SetDigitsOnly(true);
  child_box_buttons = UIVBox::Create(window);
  child_box_buttons->SetPadding(2);
  child_button_up = UIButton::Create(window, "    /\\    ");
  child_button_up->SetFontSize(7);
  child_button_up->SetBorder(false);
  child_button_up->SetInnerMargin(0);
  child_button_down = UIButton::Create(window, "    \\/    ");
  child_button_down->SetFontSize(7);
  child_button_down->SetBorder(false);
  child_button_down->SetInnerMargin(0);
  Insert(child_sep, UIBox::PackMode::WIDE);
  Insert(child_label, UIBox::PackMode::TIGHT);
  Insert(child_entry, UIBox::PackMode::TIGHT);
  Insert(child_box_buttons, UIBox::PackMode::TIGHT);
  child_box_buttons->Insert(child_button_up  , UIBox::PackMode::WIDE);
  child_box_buttons->Insert(child_button_down, UIBox::PackMode::WIDE);
  
  subscriptions += child_entry->on_edit_complete.Subscribe([this](){
    FinalizeEdit();
  });
  subscriptions += child_button_up->on_clicked.Subscribe([this](){
    SetValue(value + 1);
  });
  subscriptions += child_button_down->on_clicked.Subscribe([this](){
    SetValue(value - 1);
  });
  
  value = v;
}

std::shared_ptr<UISpinEntry> UISpinEntry::Create(std::weak_ptr<Window> parent_window, std::string name, int v, int start, int end){
  auto res = std::shared_ptr<UISpinEntry>(new UISpinEntry(parent_window));
  res->Init(name, v);
  res->SetRange(start, end);
  return res;
}
  
int UISpinEntry::GetValue() const{
  return value;
}

void UISpinEntry::SetValue(int v){
  value = v;
  if(value < range_min) value = range_min;
  if(value > range_max) value = range_max;
  child_entry->SetText( std::to_string(value) );
}

void UISpinEntry::SetRange(int start, int end){
  int digits = floor(log10(end) + 1);
  child_entry->SetMaxLength(digits);
  child_entry->SetCustomSize(Size2D(digits * 9 + 2,0));
  range_min = start;
  range_max = end;
  SetValue(value); // Trims current value to new range.
}

void UISpinEntry::FinalizeEdit(){
  std::string text = child_entry->GetText();
  if(text == "") text = "0";
  SetValue(std::stoi(text));
}

} // namespace AlgAudio
