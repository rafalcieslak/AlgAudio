#include "UI/UIWidget.hpp"

namespace AlgAudio{

void UIWidget::Draw(DrawContext& c){
  if(visible)
    CustomDraw(c);
}

} // namespace AlgAudio
