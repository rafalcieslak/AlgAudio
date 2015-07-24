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

UIList::UIList(std::weak_ptr<Window> parent_window) : UIWidget(parent_window){
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
  new_button->parent = shared_from_this();
  ids_to_buttons[id] = new_button;
  new_button->on_clicked.SubscribeForever([=](){
    on_clicked.Happen(id);
  });
  new_button->Resize(new_button->GetRequestedSize());
  UpdateMinimalSize();
}

void UIList::UpdateMinimalSize(){
  int totaly = 0, maxw = 0;
  for(auto& it : ids_to_buttons){
    Size2D s = it.second->GetRequestedSize();
    totaly += s.height;
    if(s.width > maxw) maxw = s.width;
  }
  SetMinimalSize(Size2D(maxw,totaly));
}

void UIList::CustomDraw(DrawContext& c){
  int totaly = 0;
  for(auto& it : ids_to_buttons){
    auto button = it.second;
    c.Push(Point2D(0,totaly), Size2D(c.Size().width, button->GetRequestedSize().height));
    button->Draw(c);
    c.Pop();
    totaly += button->GetRequestedSize().height;
  }
}



} // namespace AlgAudio
