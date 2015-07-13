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

void UIMarginBox::OnMouseButton(bool down, short b,Point2D p){
  if(!child) return;
  if(IsInside(p))
    child->OnMouseButton(down,b,p - Point2D(left,top));
}

bool UIMarginBox::IsInside(Point2D p) const{
  if(p.x < left || p.y < top || p.x > current_size.width - right || p.y > current_size.height - bottom) return false;
  else return true;
}

void UIMarginBox::OnMotion(Point2D p1, Point2D p2){
  if(!child) return;
  if(IsInside(p1) && IsInside(p2)) child->OnMotion(p1 - Point2D(left,top), p2 - Point2D(left,top));
  else if(IsInside(p1)){
    // start inside, end ouside
    child->OnMotionLeave(p1 - Point2D(left,top));
  }else if(IsInside(p2)){
    // start outside, enter outside
    child->OnMotionEnter(p2 - Point2D(left,top));
  }else{
    // both outside, ignore
  }
}

void UIMarginBox::OnMotionEnter(Point2D p){
  if(!child) return;
  if(IsInside(p)) child->OnMotionEnter(p - Point2D(left,top));
}
void UIMarginBox::OnMotionLeave(Point2D p){
  if(!child) return;
  if(IsInside(p)) child->OnMotionLeave(p - Point2D(left,top));
}


} // namespace AlgAudio
