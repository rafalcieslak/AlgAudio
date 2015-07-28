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
#include "UI/UIList.hpp"

namespace AlgAudio{

UIList::UIList(std::weak_ptr<Window> parent_window) : UIVBox(parent_window){
  SetPadding(1);
}

std::shared_ptr<UIList> UIList::Create(std::weak_ptr<Window> parent_window){
  return std::shared_ptr<UIList>(new UIList(parent_window));
}

void UIList::AddItem(std::string id, std::string text){
  auto it = ids_to_buttons.find(id);
  if(it != ids_to_buttons.end()){
    std::cout << "Duplicate ID in UIList, ignoring" << std::endl;
    return;
  }
  auto new_button = UIButton::Create(window, text);
  ids_to_buttons[id] = new_button;
  new_button->SetBorder(false);
  new_button->SetFontSize(14);
  new_button->SetInnerMargin(3);
  new_button->SetColor(standard_color);
  new_button->on_clicked.SubscribeForever([this, id](){
    on_clicked.Happen(id);
  });
  new_button->on_pointed.SubscribeForever([this, id](bool pointed){
    if(pointed) on_pointed.Happen(id);
    else on_pointed.Happen("");
  });
  Insert(new_button,UIVBox::PackMode::TIGHT);
}

void UIList::Clear(){
  ids_to_buttons.clear();
  UIBox::Clear();
}

void UIList::SetColors(Color standard, Color highlight){
  standard_color = standard;
  highlight_color = highlight;
  for(auto& it : ids_to_buttons){
    it.second->SetColor(standard_color);
  }
  if(highlighted) highlighted->SetColor(highlight_color);
}
void UIList::SetHighlight(std::string id){
  if(id == ""){
    // Reset highlight
    if(!highlighted) return;
    highlighted->SetColor(standard_color);
    highlighted = nullptr;
  }else{
    auto it = ids_to_buttons.find(id);
    if(it == ids_to_buttons.end()) return;
    if(highlighted) highlighted->SetColor(standard_color);
    highlighted = it->second;
    highlighted->SetColor(highlight_color);
  }
}

} // namespace AlgAudio
