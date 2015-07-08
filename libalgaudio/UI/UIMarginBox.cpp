#include "UI/UIMarginBox.hpp"

namespace AlgAudio{

UIMarginBox::UIMarginBox(int t, int r, int b, int l) :
  top(t), right(r), bottom(b), left(l)
{ }

std::shared_ptr<UIMarginBox> UIMarginBox::Create(int t, int r, int b, int l){
  std::shared_ptr<UIMarginBox> res(new UIMarginBox(t,r,b,l));
  return res;
}

void UIMarginBox::CustomDraw(DrawContext& c){
  DrawContext c2 = c.SubContext(left, top, c.width - right - left, c.height - top - bottom);
  if(!c2.HasZeroArea() && child)
    child->Draw(c2);
}

void UIMarginBox::Insert(std::shared_ptr<UIWidget> ch){
  child = ch;
  child->window = window;
  child->parent = shared_from_this();
}

void UIMarginBox::OnMouseButton(bool down, short b,int x,int y){
  if(!child) return;
  if(x < left || y < top) return;
  child->OnMouseButton(down,b,x,y);
}

} // namespace AlgAudio
