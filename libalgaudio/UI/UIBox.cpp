#include "UI/UIBox.hpp"
#include <iostream>

namespace AlgAudio{

UIBox::UIBox(std::weak_ptr<Window> w) : UIWidget(w){
}
UIVBox::UIVBox(std::weak_ptr<Window> w) : UIBox(w){
}
UIHBox::UIHBox(std::weak_ptr<Window> w) : UIBox(w){
}

std::shared_ptr<UIVBox> UIVBox::Create(std::weak_ptr<Window> w){
  std::shared_ptr<UIVBox> res(new UIVBox(w));
  return res;
}
std::shared_ptr<UIHBox> UIHBox::Create(std::weak_ptr<Window> w){
  std::shared_ptr<UIHBox> res(new UIHBox(w));
  return res;
}

void UIBox::CustomDraw(DrawContext& c){
  for(unsigned int n = 0; n < children.size(); n++){
    c.Push(GetChildLocation(n), GetChildSize(n));
    children[n].child->Draw(c);
    c.Pop();
  }
}

void UIBox::Insert(std::shared_ptr<UIWidget> w, PackMode m){
  children.push_back(PackData{w,m,50});
  w->window = window;
  w->parent = shared_from_this();
  RecalculateChildSizes(DirectionalDimension(current_size));
  TriggerChildResizes();
  SetRequestedSize(DirectionalSize2D(GetTotalSize(),GetChildMaxContra()));
}

void UIBox::RecalculateChildSizes(unsigned int available){
  // Begin by removing the space taken up by padding.
  available -= padding*(children.size() - 1);

  // Then, each tightly packed child has to be given exactly as much space as
  // the requested. Also count the space left, as well as the number of loosely
  // packed children.
  int left = available;
  unsigned int loose_children = 0;
  for(unsigned int n = 0; n < children.size(); n++){
    if(children[n].mode == PackMode::TIGHT){
      unsigned int q = DirectionalDimension(children[n].child->GetRequestedSize());
      children[n].size = q;
      left  -= q;
    }else{
      loose_children++;
    }
  }
  if(left  < 0) left  = 0; // Sigh.
  // Finally, split the space that is left more or less equally among the other
  // children.
  for(unsigned int n = 0; n < children.size(); n++){
    if(children[n].mode == PackMode::TIGHT){
      // ...
    }else{
      // The trick is to decrease left space gradually instead of
      // assigining left /n space to each child, thanks to it the box
      // will fit perfectly even if the size is not divisable by the number
      // of children
      unsigned int q = left  / loose_children;
      loose_children--;
      left  -= q;
      children[n].size = q;
    }
  }
}

Size2D UIBox::GetChildSize(unsigned int n){
  return DirectionalSize2D(children[n].size, ContradirectionalDimension(current_size));
}

void UIBox::SetPadding(unsigned int p){
  padding = p;
  RecalculateChildSizes(DirectionalDimension(current_size));
  TriggerChildResizes();
  SetRequestedSize(DirectionalSize2D(GetTotalSize(),GetChildMaxContra()));
}

void UIBox::TriggerChildResizes(){
  for(unsigned int n = 0; n < children.size(); n++){
    children[n].child->Resize(GetChildSize(n));
  }
}

void UIBox::OnChildRequestedSizeChanged(){
  RecalculateChildSizes(DirectionalDimension(current_size));
  TriggerChildResizes();
  SetRequestedSize(DirectionalSize2D(GetTotalSize(), GetChildMaxContra()));
}

void UIBox::CustomResize(Size2D newsize){
  RecalculateChildSizes(DirectionalDimension(newsize));
  current_size = newsize; // Manually setting this before triggerchildresizes
  TriggerChildResizes();
  // DO NOT set requested size here!
}

unsigned int UIBox::GetTotalSize(){
  unsigned int total = 0;
  for(unsigned int n = 0; n < children.size(); n++){
    total += children[n].size;
    total += padding;
  }
  return total - padding;
}

unsigned int UIBox::GetChildMaxContra(){
  unsigned int max = 0;
  for(unsigned int n = 0; n < children.size(); n++){
    unsigned int w = ContradirectionalDimension(children[n].child->GetRequestedSize());
    if(w > max) max = w;
  }
  return max;
}

Point2D UIBox::GetChildLocation(unsigned int m){
  unsigned int total = 0;
  for(unsigned int n = 0; n < m; n++){
    total += children[n].size;
    total += padding;
  }
  return DirectionalPoint2D(total, 0);
}

int UIBox::InWhich(Point2D p){
  int q = DirectionalDimension(p);
  int totaly = 0;
  if(q < totaly) return -1;
  for(unsigned int n = 0; n < children.size(); n++){
    totaly += children[n].size;
    if(q < totaly) return n;
    totaly += padding;
    if(q < totaly) return -1;
  }
  return -1;
}

void UIBox::OnMouseButton(bool down, short b,Point2D p){
  int n = InWhich(p);
  if(n<0) return;
  children[n].child->OnMouseButton(down,b,p - GetChildLocation(n));
}

void UIBox::OnMotion(Point2D p1, Point2D p2){
  int n1 = InWhich(p1);
  int n2 = InWhich(p2);
  if(n1 < 0 && n2 < 0){
    // Ignore.
  }else if(n1 < 0 && n2 >= 0){
    // Start outside, end inside
    children[n2].child->OnMotionEnter(p2 - GetChildLocation(n2));
  }else if(n1 >= 0 && n2 < 0){
    // Start inside, end outside
    children[n1].child->OnMotionLeave(p1 - GetChildLocation(n1));
  }else if(n1 >= 0 && n2 >= 0){
    // Both inside
    if(n1 == n2){
      // Movement inside a widget
      children[n1].child->OnMotion(p1 - GetChildLocation(n1), p2 - GetChildLocation(n1));
    }else{
      // Movement from a widget to another
      children[n1].child->OnMotionLeave(p1 - GetChildLocation(n1));
      children[n2].child->OnMotionEnter(p2 - GetChildLocation(n2));
    }
  }
}

void UIBox::OnMotionEnter(Point2D p){
  int n = InWhich(p);
  if(n < 0) return;
  children[n].child->OnMotionEnter(p - GetChildLocation(n));
}
void UIBox::OnMotionLeave(Point2D p){
  int n = InWhich(p);
  if(n < 0) return;
  children[n].child->OnMotionLeave(p - GetChildLocation(n));
}

} // namespace AlgAudio
