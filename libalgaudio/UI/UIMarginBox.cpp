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
#include "UI/UIMarginBox.hpp"

namespace AlgAudio{

UIMarginBox::UIMarginBox(std::weak_ptr<Window> w, int t, int r, int b, int l) :
  UIWidget(w), top(t), right(r), bottom(b), left(l)
{ }

std::shared_ptr<UIMarginBox> UIMarginBox::Create(std::weak_ptr<Window> w, int t, int r, int b, int l){
  std::shared_ptr<UIMarginBox> res(new UIMarginBox(w,t,r,b,l));
  return res;
}

void UIMarginBox::CustomDraw(DrawContext& c){
  c.Push(Point2D(left, top), c.Size() - Size2D(right+left, top+bottom));
  if(!c.HasZeroArea() && child)
    child->Draw(c);
  c.Pop();
}

void UIMarginBox::CustomResize(Size2D s){
  if(!child) return;
  child->Resize(Size2D(s.width - left - right, s.height - top - bottom));
}

void UIMarginBox::Insert(std::shared_ptr<UIWidget> ch){
  if(ch->HasParent()){
    std::cout << "WARNING: Insert to marginbox ignored, child already has a parent." << std::endl;
    return;
  }
  child = ch;
  child->parent = shared_from_this();
  TriggerFakeResize();

  OnChildRequestedSizeChanged();
}

void UIMarginBox::OnChildRequestedSizeChanged(){
  Size2D s = child->GetRequestedSize();
  SetMinimalSize(Size2D(s.width + right + left, s.height + top + bottom));
}
void UIMarginBox::OnChildVisibilityChanged(){
  Size2D s = child->GetRequestedSize();
  //if(child->IsVisible())
    SetMinimalSize(Size2D(s.width + right + left, s.height + top + bottom));
  //else
  //  SetMinimalSize(Size2D(top+bottom, left+right));
}

void UIMarginBox::CustomMousePress(bool down, short b,Point2D p){
  if(!child) return;
  if(IsInside(p))
    child->OnMousePress(down,b,p - Point2D(left,top));
}

bool UIMarginBox::IsInside(Point2D p) const{
  if(p.x < left || p.y < top || p.x > current_size.width - right || p.y > current_size.height - bottom) return false;
  else return true;
}

void UIMarginBox::CustomMouseMotion(Point2D p1, Point2D p2){
  if(!child) return;
  if(IsInside(p1) && IsInside(p2)) child->OnMouseMotion(p1 - Point2D(left,top), p2 - Point2D(left,top));
  else if(IsInside(p1)){
    // start inside, end ouside
    child->OnMouseLeave(p1 - Point2D(left,top));
  }else if(IsInside(p2)){
    // start outside, enter outside
    child->OnMouseEnter(p2 - Point2D(left,top));
  }else{
    // both outside, ignore
  }
}

void UIMarginBox::CustomMouseEnter(Point2D p){
  if(!child) return;
  if(IsInside(p)) child->OnMouseEnter(p - Point2D(left,top));
}
void UIMarginBox::CustomMouseLeave(Point2D p){
  if(!child) return;
  if(IsInside(p)) child->OnMouseLeave(p - Point2D(left,top));
}


} // namespace AlgAudio
