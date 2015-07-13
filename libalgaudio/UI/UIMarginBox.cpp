#include "UI/UIMarginBox.hpp"

namespace AlgAudio{

UIMarginBox::UIMarginBox(std::weak_ptr<Window> w, int t, int r, int b, int l) :
  UIWidget(w), top(t), right(r), bottom(b), left(l)
{ }

std::shared_ptr<UIMarginBox> UIMarginBox::Create(std::weak_ptr<Window> w, int t, int r, int b, int l){
  std::shared_ptr<UIMarginBox> res(new UIMarginBox(w,t,r,b,l));
  return res;
}

void UIMarginBox::CustomDraw(DrawContext& c){
  c.Push(left, top, c.width - right - left, c.height - top - bottom);
  if(!c.HasZeroArea() && child)
    child->Draw(c);
  c.Pop();
}

void UIMarginBox::CustomResize(Size2D s){
  if(!child) return;
  child->Resize(Size2D(s.width - top - bottom, s.height - left - right));
}

void UIMarginBox::Insert(std::shared_ptr<UIWidget> ch){
  child = ch;
  child->window = window;
  child->parent = shared_from_this();
  TriggerFakeResize();

  OnChildRequestedSizeChanged();
}

void UIMarginBox::OnChildRequestedSizeChanged(){
  Size2D s = child->GetRequestedSize();
  SetRequestedSize(Size2D(s.width + top + bottom, s.height + left + right));
}

void UIMarginBox::OnMouseButton(bool down, short b,int x,int y){
  if(!child) return;
  if(IsInside(x,y))
    child->OnMouseButton(down,b,x-left,y-top);
}

bool UIMarginBox::IsInside(int x, int y) const{
  if(x < left || y < top || x > current_size.width - right || y > current_size.height - bottom) return false;
  else return true;
}

void UIMarginBox::OnMotion(int x1, int y1, int x2, int y2){
  if(!child) return;
  if(IsInside(x1,y1) && IsInside(x2,y2)) child->OnMotion(x1 - left, y1 - top, x2 - left, y2 - top);
  else if(IsInside(x1,y1)){
    // start inside, end ouside
    child->OnMotionLeave(x1 - left, y1 - top);
  }else if(IsInside(x2,y2)){
    // start outside, enter outside
    child->OnMotionEnter(x2 - left, y2 - top);
  }else{
    // both outside, ignore
  }
}

void UIMarginBox::OnMotionEnter(int x, int y){
  if(!child) return;
  if(IsInside(x,y)) child->OnMotionEnter(x - left, y - top);
}
void UIMarginBox::OnMotionLeave(int x, int y){
  if(!child) return;
  if(IsInside(x,y)) child->OnMotionLeave(x - left, y - top);
}


} // namespace AlgAudio
