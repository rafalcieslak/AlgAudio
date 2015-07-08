#include "UI/UIWidget.hpp"
#include "UI/UIWindow.hpp"

namespace AlgAudio{

void UIWidget::Draw(const DrawContext& c){
  if(visible){
    CustomDraw(c);
    needs_redrawing = false;
    last_drawn_size = WidgetSize(c.width,c.height);
  }
}

void UIWidget::SetNeedsRedrawing(){
  needs_redrawing = true;
  auto p = parent.lock();
  auto w = window.lock();
  if(p) p->SetNeedsRedrawing();
  else if(w) w->SetNeedsRedrawing();
}

} // namespace AlgAudio
