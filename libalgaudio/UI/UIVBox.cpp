#include "UI/UIVBox.hpp"

namespace AlgAudio{

UIVBox::UIVBox(std::weak_ptr<Window> w) : UIWidget(w){
}

std::shared_ptr<UIVBox> UIVBox::Create(std::weak_ptr<Window> w){
  std::shared_ptr<UIVBox> res(new UIVBox(w));
  return res;
}

void UIVBox::CustomDraw(DrawContext& c){
  unsigned int totaly = 0;
  for(auto& pd : children){
    c.Push(0, totaly ,current_size.width, pd.size);
    pd.child->Draw(c);
    totaly += pd.size;
    c.Pop();
  }
}

void UIVBox::Insert(std::shared_ptr<UIWidget> w, PackMode m){
  children.push_back(PackData{w,m,50});
  w->window = window;
  w->parent = shared_from_this();
}


} // namespace AlgAudio
