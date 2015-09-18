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
#include "UI/UIPosition.hpp"

namespace AlgAudio{

UIPosition::UIPosition(std::weak_ptr<Window> w) : UIContainerSingle(w)
{ }

std::shared_ptr<UIPosition> UIPosition::Create(std::weak_ptr<Window> w){
  std::shared_ptr<UIPosition> res(new UIPosition(w));
  return res;
}

void UIPosition::CustomDraw(DrawContext& c){
  if(!child) return;
  c.Push(position, child->GetRequestedSize());
  child->Draw(c);
  c.Pop();
}

void UIPosition::Insert(std::shared_ptr<UIWidget> ch, Point2D pos){
  if(ch->HasParent()){
    std::cout << "WARNING: Insert to UIPosition ignored, child already has a parent." << std::endl;
    return;
  }
  child = ch;
  child->parent = shared_from_this();
  position = pos;
  OnChildRequestedSizeChanged();
}

void UIPosition::OnChildRequestedSizeChanged(){
  Size2D s = child->GetRequestedSize();
  child->Resize(s);
  SetMinimalSize(s);
}
void UIPosition::OnChildVisibilityChanged(){
  SetMinimalSize(child->GetRequestedSize()); 
}

bool UIPosition::CustomMousePress(bool down, MouseButton b,Point2D p){
  if(!child) return false;
  if(IsInside(p)){
    child->OnMousePress(down,b,p - position);
    return true;
  }
  return false;
}

bool UIPosition::IsInside(Point2D p) const{
  if(!child) return false;
  if(p.x < position.x || p.y < position.y || p.x > position.x + child->GetRequestedSize().width || p.y > position.y + child->GetRequestedSize().height) return false;
  else return true;
}

void UIPosition::CustomMouseMotion(Point2D p1, Point2D p2){
  if(!child) return;
  if(IsInside(p1) && IsInside(p2)) child->OnMouseMotion(p1 - position, p2 - position);
  else if(IsInside(p1)){
    // start inside, end ouside
    child->OnMouseLeave(p1 - position);
  }else if(IsInside(p2)){
    // start outside, enter outside
    child->OnMouseEnter(p2 - position);
  }else{
    // both outside, ignore
  }
}

void UIPosition::CustomMouseEnter(Point2D p){
  if(!child) return;
  if(IsInside(p)) child->OnMouseEnter(p - position);
}
void UIPosition::CustomMouseLeave(Point2D p){
  if(!child) return;
  if(IsInside(p)) child->OnMouseLeave(p - position);
}

void UIPosition::RemoveChild(){
  child = nullptr;
  SetMinimalSize(Size2D(0,0));
  SetNeedsRedrawing();
}

} // namespace AlgAudio
