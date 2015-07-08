#include "UI/UIWidget.hpp"

namespace AlgAudio{

void UIWidget::Draw(const DrawContext& c){
  if(visible){
    CustomDraw(c);
    last_drawn_size = WidgetSize(c.width,c.height);
  }
}

} // namespace AlgAudio
