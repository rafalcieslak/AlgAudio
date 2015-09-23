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
#include "UI/UIWidget.hpp"
#include <iostream>
#include <typeinfo>
#include "Window.hpp"
#include "UI/UIContainer.hpp"

namespace AlgAudio{
  
UIWidget::UIWidget(std::weak_ptr<Window> parent_window)
  : window(parent_window)
{
  cache_texture = std::make_shared<SDLTexture>(parent_window, Size2D(0,0));
  on_display_mode_changed.SubscribeForever([this](){
    if(display_mode != DisplayMode::Invisible)
      SetNeedsRedrawing();
    auto p = parent.lock();
    if(p) p->OnChildVisibilityChanged();
  });
}
    
void UIWidget::Draw(DrawContext& c){
  if(display_mode == DisplayMode::Invisible) return;

  const Size2D drawsize = c.Size();
  if(drawsize.IsEmpty()){
    needs_redrawing = false;
    return;
  }
  if(drawsize != current_size){
    std::cout << "WARNING: Implicitly calling Resize, because the draw "
    "context size " << drawsize.ToString() << " is incoherent with widget "
    "stored size " << current_size.ToString() << "!" << std::endl;
    //throw Exception("");
    Resize(drawsize);
  }
  if(needs_redrawing){
    //std::cout << "Needs redrawing. " << cache_texture->GetSize().ToString() << std::endl;
    c.Push(cache_texture, drawsize.width, drawsize.height);
    if(IsDrawn()){
      c.Clear(clear_color);
      CustomDraw(c);
      c.SetColor(overlay_color);
      c.Fill();
    }else{
      c.Clear();
    }
    c.Pop();
  }else{
    //std::cout << "No need to redraw. " << cache_texture->GetSize().ToString() << std::endl;
  }
  c.DrawTexture(cache_texture);
  current_size = drawsize;
  needs_redrawing = false;
  
}

void UIWidget::Resize(Size2D s){
  //std::cout << "Resizing from " << current_size.ToString() << " to " << s.ToString() << std::endl;
  if(current_size == s) return;
  //std::cout << "Performing" << std::endl;
  cache_texture->Resize(s);
  SetNeedsRedrawing();
  in_custom_resize = true;
  CustomResize(s);
  in_custom_resize = false;
  current_size = s;
}

void UIWidget::SetNeedsRedrawing(){
  if(needs_redrawing) return;
  needs_redrawing = true;
  auto p = parent.lock();
  auto w = window.lock();
  if(p) p->SetNeedsRedrawing();
  else if(w) w->SetNeedsRedrawing();
}

void UIWidget::SetMinimalSize(Size2D s){
  if(in_custom_resize){
    std::cout << "WARNING: A widget called SetMinimalSize while inside resize-chain. Change ignored." << std::endl;
    return;
  }
  minimal_size = s;
  auto p = parent.lock();
  if(p) p->OnChildRequestedSizeChanged();
}
void UIWidget::SetCustomSize(Size2D s){
  if(in_custom_resize){
    std::cout << "WARNING: A widget called SetCustomSize while inside resize-chain. Change ignored." << std::endl;
    return;
  }
  custom_size = s;
  auto p = parent.lock();
  if(p) p->OnChildRequestedSizeChanged();
}

Point2D UIWidget::GetPosInParent(std::shared_ptr<UIWidget> ancestor){
  auto p = parent.lock();
  if(!p){
    std::cout << "WARNING: GetPosInParent: The widget has no parent, but ancestor was not yet found." << std::endl;
    return Point2D(0,0);
  }
  Point2D res(0,0);
  auto contsingle = std::dynamic_pointer_cast<UIContainerSingle>(p);
  auto contmulti  = std::dynamic_pointer_cast<UIContainerMultiple>(p);
  if(contsingle){
    res = contsingle->GetChildPos();
  }else if(contmulti){
    res = contmulti->GetChildPos(shared_from_this());
  }else{
    std::cout << "WARNING: GetPosInParent: The parent is not a container. This widget is a " << typeid(*this).name() << " and the parent is " << typeid(*(p.get())).name() << std::endl;
    return Point2D(0,0);
  }
  if(p != ancestor){
    res = res + p->GetPosInParent(ancestor);
  }
  return res;
}

void UIWidget::RequestFocus(){
  auto p = parent.lock();
  if(!p) return;
  p->OnChildFocusRequested(shared_from_this());
}

bool UIWidget::IsFocused() const{
  auto p = parent.lock();
  if(!p) return IsRoot(); // Root widgets are always focused, but parentless widgets never.
  return p->OnChildFocusTested(shared_from_this());
}


bool UIWidget::IsRoot() const{
  auto w = window.lock();
  if(!w) return false;
  return w->GetRoot() == shared_from_this();
}

} // namespace AlgAudio
