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
#include "UI/UIAnimDrawer.hpp"
#include "SDLMain.hpp"

namespace AlgAudio{

UIAnimDrawer::UIAnimDrawer(std::weak_ptr<Window> parent_window, Direction dir) :
    UIWidget(parent_window), direction(dir)
{

}

std::shared_ptr<UIAnimDrawer> UIAnimDrawer::Create(std::weak_ptr<Window> parent_window, Direction dir){
  return std::shared_ptr<UIAnimDrawer>( new UIAnimDrawer(parent_window,dir) );
}

void UIAnimDrawer::Insert(std::shared_ptr<UIWidget> ch){
  if(ch->HasParent()){
    std::cout << "WARNING: Insert to UIAnimDrawer ignored, child already has a parent." << std::endl;
    return;
  }
  child = ch;
  child->parent = shared_from_this();
}

Size2D UIAnimDrawer::GetInnerSize() const{
  Size2D s = child->GetRequestedSize();
  if(direction == Direction_TOP  || direction == Direction_BOTTOM) return Size2D(current_size.width, s.height);
  if(direction == Direction_LEFT || direction == Direction_RIGHT ) return Size2D(s.width, current_size.height);
  return Size2D(0,0);
}
Point2D UIAnimDrawer::GetCurrentOffset() const{
  Size2D in_size = GetInnerSize();
  if(direction == Direction_TOP)    return Point2D(0,(-1.0 + phase)*in_size.height);
  if(direction == Direction_BOTTOM) return Point2D(0,( 2.0 - phase)*in_size.height);
  if(direction == Direction_LEFT)   return Point2D((-1.0 + phase)*in_size.width, 0);
  if(direction == Direction_RIGHT)  return Point2D(( 2.0 - phase)*in_size.width, 0);
  return Point2D(0,0);
}

void UIAnimDrawer::CustomDraw(DrawContext& c){
  if(!child) return;
  Size2D in_size = GetInnerSize();
  c.Push(GetCurrentOffset(),in_size);
  child->Draw(c);
  c.Pop();
}
void UIAnimDrawer::CustomResize(Size2D s){
  if(!child) return;
  current_size = s;
  child->Resize(GetInnerSize());
}
void UIAnimDrawer::StartShow(float t){
  anim->Release();
  if(phase > 0.999 || t < 0.002){ // instant show
    phase = 1.0;
    on_show_complete.Happen();
    UpdateRequestedSize();
    return;
  }
  state = 1;
  time_to_finish = t;
  anim = SDLMain::on_before_frame.Subscribe(this, &UIAnimDrawer::Step);
}
void UIAnimDrawer::StartHide(float t){
  anim->Release();
  if(phase < 0.001 || t < 0.002){ // instant hide
    phase = 0.0;
    on_hide_complete.Happen();
    UpdateRequestedSize();
    return;
  }
  state = -1;
  time_to_finish = t;
  anim = SDLMain::on_before_frame.Subscribe(this, &UIAnimDrawer::Step);
}
void UIAnimDrawer::Step(float delta){
  if(state == 0) std::cout << "Warning! Step should be never called with state = 0." << std::endl;
  float phasedelta = delta/time_to_finish;
  phase += phasedelta*state;
  if(state == 1 && phase > 0.999){
    state = 0;
    phase = 1.0;
    anim->Release();
    on_show_complete.Happen();
  }else if(state == -1 && phase < 0.001){
    state = 0;
    phase = 0.0;
    anim->Release();
    on_hide_complete.Happen();
  }
  UpdateRequestedSize();
}

void UIAnimDrawer::UpdateRequestedSize(){
  Size2D s = child->GetRequestedSize();
  if(direction == Direction_TOP)    SetMinimalSize(Size2D(0,phase*s.height));
  if(direction == Direction_BOTTOM) SetMinimalSize(Size2D(0,phase*s.height));
  if(direction == Direction_LEFT)   SetMinimalSize(Size2D(phase*s.width, 0));
  if(direction == Direction_RIGHT)  SetMinimalSize(Size2D(phase*s.width, 0));
}

void UIAnimDrawer::OnChildRequestedSizeChanged(){
  UpdateRequestedSize();
}
void UIAnimDrawer::OnChildVisibilityChanged(){
  UpdateRequestedSize();
}

// TODO: Animated pointing over moving elements?
void UIAnimDrawer::CustomMousePress(bool down, short b,Point2D pos){
  if(!child || phase < 0.999) return;
  child->OnMousePress(down,b,pos);
}
void UIAnimDrawer::CustomMouseMotion(Point2D p1,Point2D p2){
  if(!child || phase < 0.999) return;
  child->OnMouseMotion(p1,p2);
}
void UIAnimDrawer::CustomMouseEnter(Point2D p2){
  if(!child || phase < 0.999) return;
  child->OnMouseEnter(p2);
}
void UIAnimDrawer::CustomMouseLeave(Point2D p1){
  if(!child || phase < 0.999) return;
  child->OnMouseLeave(p1);
}

} // namespace AlgAudio
