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
#include "UI/UICentered.hpp"

namespace AlgAudio{

UICentered::UICentered(std::weak_ptr<Window> w) :
  UIWidget(w)
{ }

std::shared_ptr<UICentered> UICentered::Create(std::weak_ptr<Window> w){
  std::shared_ptr<UICentered> res(new UICentered(w));
  return res;
}

void UICentered::CustomDraw(DrawContext& c){
  if(!child) return;
  c.Push(current_child_pos, current_child_size);
  child->Draw(c);
  c.Pop();
}

void UICentered::CustomResize(Size2D s){
  if(!child) return;
  current_child_pos = Point2D(s.width/2 - current_child_size.width/2, s.height/2 - current_child_size.height/2);
}

void UICentered::Insert(std::shared_ptr<UIWidget> ch){
  if(ch->HasParent()){
    std::cout << "WARNING: Insert to UICentered ignored, child already has a parent." << std::endl;
    return;
  }
  child = ch;
  child->parent = shared_from_this();

  OnChildRequestedSizeChanged();
}

void UICentered::OnChildRequestedSizeChanged(){
  current_child_size = child->GetRequestedSize();
  current_child_pos = Point2D(current_size.width/2 - current_child_size.width/2, current_size.height/2 - current_child_size.height/2);
  child->Resize(current_child_size);
  SetMinimalSize(current_child_size);
}
void UICentered::OnChildVisibilityChanged(){
  current_child_size = child->GetRequestedSize();
  current_child_pos = Point2D(current_size.width/2 - current_child_size.width/2, current_size.height/2 - current_child_size.height/2);
  child->Resize(current_child_size);
  SetMinimalSize(current_child_size);
}

void UICentered::CustomMousePress(bool down, short b,Point2D p){
  if(!child) return;
  if(IsInside(p))
    child->OnMousePress(down,b,p - current_child_pos);
}

bool UICentered::IsInside(Point2D p) const{
  if(p.x < current_child_pos.x || p.y < current_child_pos.y || p.x > current_child_pos.x + current_child_size.width || p.y > current_child_pos.y + current_child_size.height) return false;
  else return true;
}

void UICentered::CustomMouseMotion(Point2D p1, Point2D p2){
  if(!child) return;
  if(IsInside(p1) && IsInside(p2)) child->OnMouseMotion(p1 - current_child_pos, p2 - current_child_pos);
  else if(IsInside(p1)){
    // start inside, end ouside
    child->OnMouseLeave(p1 - current_child_pos);
  }else if(IsInside(p2)){
    // start outside, enter outside
    child->OnMouseEnter(p2 - current_child_pos);
  }else{
    // both outside, ignore
  }
}

void UICentered::CustomMouseEnter(Point2D p){
  if(!child) return;
  if(IsInside(p)) child->OnMouseEnter(p - current_child_pos);
}
void UICentered::CustomMouseLeave(Point2D p){
  if(!child) return;
  if(IsInside(p)) child->OnMouseLeave(p - current_child_pos);
}

void UICentered::RemoveChild(){
  if(!child) return;
  child->parent.reset();
  child = nullptr;
}

} // namespace AlgAudio
