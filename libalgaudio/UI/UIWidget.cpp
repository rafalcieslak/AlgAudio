#include "UI/UIWidget.hpp"

namespace AlgAudio{

void UIWidget::Draw(const DrawContext& c){
  if(visible)
    CustomDraw(c);
}

} // namespace AlgAudio
