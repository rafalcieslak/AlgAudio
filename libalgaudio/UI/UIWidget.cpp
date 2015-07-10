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
      c.Push(cache_texture, drawsize.width, drawsize.height);
      c.Clear();
      CustomDraw(c);
      c.Pop();
    }
    c.DrawTexture(cache_texture);
    current_size = drawsize;
    needs_redrawing = false;
  }
}

void UIWidget::Resize(Size2D s){
  if(current_size == s) return;
  cache_texture->Resize(s);
  in_custom_resize = true;
  SetNeedsRedrawing();
  in_custom_resize = false;
  CustomResize(s);
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

void UIWidget::SetRequestedSize(Size2D s){
  if(in_custom_resize){
    std::cout << "WARNING: A widget called SetRequestedSize while inside resize-chain. Change ignored." << std::endl;
    return;
  }
  requested_size = s;
  auto p = parent.lock();
  if(p) p->OnChildRequestedSizeChanged();
}

} // namespace AlgAudio
