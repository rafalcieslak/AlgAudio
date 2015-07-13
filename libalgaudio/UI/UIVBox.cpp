#include "UI/UIVBox.hpp"
#include <iostream>

namespace AlgAudio{

UIVBox::UIVBox(std::weak_ptr<Window> w) : UIWidget(w){
}

std::shared_ptr<UIVBox> UIVBox::Create(std::weak_ptr<Window> w){
  std::shared_ptr<UIVBox> res(new UIVBox(w));
  return res;
}

void UIVBox::CustomDraw(DrawContext& c){
  for(unsigned int n = 0; n < children.size(); n++){
    c.Push(0, GetChildLocation(n), current_size.width, children[n].size);
    children[n].child->Draw(c);
    c.Pop();
  }
}

void UIVBox::Insert(std::shared_ptr<UIWidget> w, PackMode m){
  children.push_back(PackData{w,m,50});
  w->window = window;
  w->parent = shared_from_this();
  RecalculateChildSizes(current_size.height);
  TriggerChildResizes();
  SetRequestedSize(Size2D(GetChildMaxWidth(), GetTotalSize()));
}

void UIVBox::RecalculateChildSizes(unsigned int available){
  // Begin by removing the space taken up by padding.
  available -= padding*(children.size() - 1);

  // Then, each tightly packed child has to be given exactly as much space as
  // the requested. Also count the space left, as well as the number of loosely
  // packed children.
  int left = available;
  unsigned int loose_children = 0;
  for(unsigned int n = 0; n < children.size(); n++){
    if(children[n].mode == PackMode::TIGHT){
      unsigned int q = children[n].child->GetRequestedSize().height;
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

Size2D UIVBox::GetChildSize(unsigned int n){
  return Size2D(current_size.width, children[n].size);
}

void UIVBox::SetPadding(unsigned int p){
  padding = p;
  RecalculateChildSizes(current_size.height);
  TriggerChildResizes();
  SetRequestedSize(Size2D(GetChildMaxWidth(), GetTotalSize()));
}

void UIVBox::TriggerChildResizes(){
  for(unsigned int n = 0; n < children.size(); n++){
    children[n].child->Resize(GetChildSize(n));
  }
}

void UIVBox::OnChildRequestedSizeChanged(){
  RecalculateChildSizes(current_size.height);
  TriggerChildResizes();
  SetRequestedSize(Size2D(GetChildMaxWidth(), GetTotalSize()));
}

void UIVBox::CustomResize(Size2D newsize){
  std::cout << "Custom resize for vbox" << std::endl;
  RecalculateChildSizes(newsize.height);
  current_size = newsize; // Manually setting this before triggerchildresizes
  TriggerChildResizes();
  // DO NOT set requested size here!
}

unsigned int UIVBox::GetTotalSize(){
  unsigned int total = 0;
  for(unsigned int n = 0; n < children.size(); n++){
    total += children[n].size;
    total += padding;
  }
  return total - padding;
}

unsigned int UIVBox::GetChildMaxWidth(){
  unsigned int max = 0;
  for(unsigned int n = 0; n < children.size(); n++){
    unsigned int w = children[n].child->GetRequestedSize().width;
    if(w > max) max = w;
  }
  return max;
}

unsigned int UIVBox::GetChildLocation(unsigned int m){
  unsigned int total = 0;
  for(unsigned int n = 0; n < m; n++){
    total += children[n].size;
    total += padding;
  }
  return total;
}

} // namespace AlgAudio
