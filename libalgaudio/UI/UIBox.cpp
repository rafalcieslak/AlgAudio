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
#include "UI/UIBox.hpp"
#include <iostream>

namespace AlgAudio{

UIBox::UIBox(std::weak_ptr<Window> w) : UIContainerMultiple(w){
}
UIVBox::UIVBox(std::weak_ptr<Window> w) : UIBox(w){
}
UIHBox::UIHBox(std::weak_ptr<Window> w) : UIBox(w){
}

std::shared_ptr<UIVBox> UIVBox::Create(std::weak_ptr<Window> w){
  std::shared_ptr<UIVBox> res(new UIVBox(w));
  return res;
}
std::shared_ptr<UIHBox> UIHBox::Create(std::weak_ptr<Window> w){
  std::shared_ptr<UIHBox> res(new UIHBox(w));
  return res;
}

void UIBox::CustomDraw(DrawContext& c){
  //std::cout << "Start" << std::endl;
  for(unsigned int n = 0; n < children.size(); n++){
    c.Push(GetChildLocation(n), GetChildSize(n));
    //std::cout << "Drawing child at " << GetChildLocation(n).ToString() << " size " << GetChildSize(n).ToString() << std::endl;
    children[n].child->Draw(c);
    c.Pop();
  }
  //std::cout << "End" << std::endl;
}

void UIBox::Insert(std::shared_ptr<UIWidget> w, PackMode m){
  if(w->HasParent()){
    std::cout << "WARNING: Insert to box ignored, child already has a parent." << std::endl;
    return;
  }
  children.push_back(PackData{w,m,50});
  w->parent = shared_from_this();
  RecalculateChildSizes(DirectionalDimension(current_size));
  TriggerChildResizes();
  SetMinimalSize(DirectionalSize2D(GetTotalSize(),GetChildMaxContra()));
}

void UIBox::Clear(){
  // Make your children orphans
  for(unsigned int n = 0; n < children.size(); n++)
    children[n].child->parent.reset();
  children.clear();
  SetMinimalSize(Size2D(0,0));
}

Point2D UIBox::GetChildPos(std::shared_ptr<UIWidget> w) const{
  for(unsigned int n = 0; n < children.size(); n++)
    if(children[n].child == w) return GetChildLocation(n);
  // Child not found:
  std::cout << "WARNING: Queried position of an unexisting child" << std::endl;
  return Point2D(0,0);
}

std::shared_ptr<UIWidget> UIBox::CustomFindChild(ID id) const{
  for(unsigned int n = 0; n < children.size(); n++){
    auto res = children[n].child->FindChild(id);
    if(res) return res;
  }
  return nullptr;
}

void UIBox::RecalculateChildSizes(unsigned int available){
  // Begin by removing the space taken up by padding.
  available -= padding*(children.size() - 1);

  // Then, each tightly packed child has to be given exactly as much space as
  // the requested. Also count the space left, as well as the number of loosely
  // packed children.
  int left = available;
  unsigned int loose_children = 0;
  for(unsigned int n = 0; n < children.size(); n++){
    if(children[n].child->IsVisible() == false){
      // Invisible children are given exactly 0 space, regardless of their pack
      // mode
      children[n].size = 0;
    }else if(children[n].mode == PackMode::TIGHT){
      unsigned int q = DirectionalDimension(children[n].child->GetRequestedSize());
      children[n].size = q;
      left  -= q;
    }else{
      loose_children++;
    }
  }
  if(left  < 0) left  = 0; // Sigh.
  // Finally, split the space that is left more or less equally among the other
  // children.
  for(unsigned int n = 0; n < children.size(); n++){
    if(children[n].child->IsVisible() == false){
      // ...
    }else if(children[n].mode == PackMode::TIGHT){
      // ...
    }else{
      // The trick is to decrease left space gradually instead of
      // assigining left /n space to each child, thanks to it the box
      // will fit perfectly even if the size is not divisable by the number
      // of children
      unsigned int q = left  / loose_children;
      loose_children--;
      left  -= q;
      children[n].size = q;
    }
  }
}

Size2D UIBox::GetChildSize(unsigned int n) const{
  return DirectionalSize2D(children[n].size, ContradirectionalDimension(current_size));
}

void UIBox::SetPadding(unsigned int p){
  padding = p;
  RecalculateChildSizes(DirectionalDimension(current_size));
  TriggerChildResizes();
  SetMinimalSize(DirectionalSize2D(GetTotalSize(),GetChildMaxContra()));
}

void UIBox::TriggerChildResizes(){
  for(unsigned int n = 0; n < children.size(); n++){
    children[n].child->Resize(GetChildSize(n));
  }
}

void UIBox::OnChildRequestedSizeChanged(){
  RecalculateChildSizes(DirectionalDimension(current_size));
  TriggerChildResizes();
  SetMinimalSize(DirectionalSize2D(GetTotalSize(), GetChildMaxContra()));
}
void UIBox::OnChildVisibilityChanged(){
  RecalculateChildSizes(DirectionalDimension(current_size));
  TriggerChildResizes();
  SetMinimalSize(DirectionalSize2D(GetTotalSize(), GetChildMaxContra()));
}

void UIBox::CustomResize(Size2D newsize){
  RecalculateChildSizes(DirectionalDimension(newsize));
  current_size = newsize; // Manually setting this before triggerchildresizes
  TriggerChildResizes();
  // DO NOT set requested size here!
}

unsigned int UIBox::GetTotalSize() const{
  unsigned int total = 0;
  for(unsigned int n = 0; n < children.size(); n++){
    total += std::max(children[n].size, DirectionalDimension(children[n].child->GetRequestedSize()));
    total += padding;
  }
  return total - padding;
}

unsigned int UIBox::GetChildMaxContra() const{
  unsigned int max = 0;
  for(unsigned int n = 0; n < children.size(); n++){
    unsigned int w = ContradirectionalDimension(children[n].child->GetRequestedSize());
    if(w > max) max = w;
  }
  return max;
}

Point2D UIBox::GetChildLocation(unsigned int m) const{
  unsigned int total = 0;
  for(unsigned int n = 0; n < m; n++){
    total += children[n].size;
    total += padding;
  }
  return DirectionalPoint2D(total, 0);
}

int UIBox::InWhich(Point2D p) const{
  int q = DirectionalDimension(p);
  int totaly = 0;
  if(q < totaly) return -1;
  for(unsigned int n = 0; n < children.size(); n++){
    totaly += children[n].size;
    if(q < totaly) return n;
    totaly += padding;
    if(q < totaly) return -1;
  }
  return -1;
}

bool UIBox::CustomMousePress(bool down, short b,Point2D p){
  int n = InWhich(p);
  if(n<0) return false;
  return children[n].child->OnMousePress(down,b,p - GetChildLocation(n));
}

void UIBox::CustomMouseMotion(Point2D p1, Point2D p2){
  int n1 = InWhich(p1);
  int n2 = InWhich(p2);
  if(n1 < 0 && n2 < 0){
    // Ignore.
  }else if(n1 < 0 && n2 >= 0){
    // Start outside, end inside
    children[n2].child->OnMouseEnter(p2 - GetChildLocation(n2));
  }else if(n1 >= 0 && n2 < 0){
    // Start inside, end outside
    children[n1].child->OnMouseLeave(p1 - GetChildLocation(n1));
  }else if(n1 >= 0 && n2 >= 0){
    // Both inside
    if(n1 == n2){
      // Movement inside a widget
      children[n1].child->OnMouseMotion(p1 - GetChildLocation(n1), p2 - GetChildLocation(n1));
    }else{
      // Movement from a widget to another
      children[n1].child->OnMouseLeave(p1 - GetChildLocation(n1));
      children[n2].child->OnMouseEnter(p2 - GetChildLocation(n2));
    }
  }
}

void UIBox::CustomMouseEnter(Point2D p){
  int n = InWhich(p);
  if(n < 0) return;
  children[n].child->OnMouseEnter(p - GetChildLocation(n));
}
void UIBox::CustomMouseLeave(Point2D p){
  int n = InWhich(p);
  if(n < 0) return;
  children[n].child->OnMouseLeave(p - GetChildLocation(n));
}

} // namespace AlgAudio
