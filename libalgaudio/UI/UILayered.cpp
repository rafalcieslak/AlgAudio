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
#include "UI/UILayered.hpp"

namespace AlgAudio{

UILayered::UILayered(std::weak_ptr<Window> w) : UIWidget(w){

}

std::shared_ptr<UILayered> UILayered::Create(std::weak_ptr<Window> w){
  return std::shared_ptr<UILayered>(new UILayered(w));
}

void UILayered::Insert(const std::shared_ptr<UIWidget>& child){
  children.push_back(child); // copies the pointer
  child->parent = shared_from_this();
  RecalculateSize();
  SetNeedsRedrawing();
}
void UILayered::CustomDraw(DrawContext& c){
  // No need to adjust context size etc.
  // Just draw them all stacked each on another.
  for(auto& child : children)
    if(child->IsVisible())
      child->Draw(c);
}
void UILayered::CustomResize(Size2D size){
  for(auto& child : children){
    // All children get our full size
    child->Resize(size);
  }
}
void UILayered::OnChildRequestedSizeChanged(){
  RecalculateSize();
}
void UILayered::OnChildVisibilityChanged(){
  SetNeedsRedrawing();
}
void UILayered::RecalculateSize(){
  int maxw = 0, maxh = 0;
  for(auto& child : children){
    Size2D s = child->GetRequestedSize();
    if(s.width > maxw) maxw = s.width;
    if(s.height > maxh) maxh = s.height;
  }
  SetMinimalSize(Size2D(maxw,maxh));
}


std::shared_ptr<UIWidget> UILayered::GetTopChild() const{
  for(auto it = children.rbegin(); it != children.rend(); it++)
    if((*it)->IsVisible()) return *it;
  return nullptr;
}

void UILayered::CustomMousePress(bool down, short b,Point2D p){
  auto w = GetTopChild();
  if(w) w->OnMousePress(down, b, p);
}

void UILayered::CustomMouseEnter(Point2D p){
  auto w = GetTopChild();
  if(w) w->OnMouseEnter(p);
}

void UILayered::CustomMouseLeave(Point2D p){
  auto w = GetTopChild();
  if(w) w->OnMouseLeave(p);
}
void UILayered::CustomMouseMotion(Point2D p1, Point2D p2){
  auto w = GetTopChild();
  if(w) w->OnMouseMotion(p1,p2);
}

} // namespace AlgAudio
