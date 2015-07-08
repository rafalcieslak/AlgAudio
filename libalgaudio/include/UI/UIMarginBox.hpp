#ifndef UIMARGINBOX_HPP
#define UIMARGINBOX_HPP
#include "UIWidget.hpp"

namespace AlgAudio{

class UIMarginBox : public UIWidget{
public:
  UIMarginBox(int top, int right, int bottom, int left);
  virtual void Draw(DrawContext& c) override;
  virtual void Insert(std::shared_ptr<UIWidget>);
private:
  int top, right, bottom, left;
  std::shared_ptr<UIWidget> child;
};

} // namespace AlgAudio

#endif // UIMARGINBOX_HPP
