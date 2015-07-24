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
#include "Window.hpp"

namespace AlgAudio{

void UIWidget::Draw(DrawContext& c){
  if(visible){
    const Size2D drawsize = c.Size();
    if(drawsize != current_size){
      std::cout << "WARNING: Implicitly calling Resize, because the draw"
      "context size is incoherent with widget size!" << std::endl;
      Resize(drawsize);
    }
    if(needs_redrawing){
      //std::cout << "Needs redrawing. " << cache_texture->GetSize().ToString() << std::endl;
      c.Push(cache_texture, drawsize.width, drawsize.height);
      c.Clear(clear_color);
      CustomDraw(c);
      c.SetColor(overlay_color);
      c.Fill();
      c.Pop();
    }else{
      //std::cout << "No need to redraw. " << cache_texture->GetSize().ToString() << std::endl;
    }
    c.DrawTexture(cache_texture);
    current_size = drawsize;
    needs_redrawing = false;
  }
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
  if(!visible) return;
  needs_redrawing = true;
  auto p = parent.lock();
  auto w = window.lock();
  if(p) p->SetNeedsRedrawing();
  else if(w) w->SetNeedsRedrawing();
}

void UIWidget::SetVisible(bool v){
  if(v == visible) return;
  visible = v;
  if(v) SetNeedsRedrawing();
  auto p = parent.lock();
  if(p) p->OnChildVisibilityChanged();
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
Size2D UIWidget::GetRequestedSize() const{
  if(!visible) return Size2D(0,0);
  return Size2D(
      std::max(minimal_size.width,  custom_size.width ),
      std::max(minimal_size.height, custom_size.height)
    );
}

} // namespace AlgAudio
