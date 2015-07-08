#ifndef UIMARGINBOX_HPP
#define UIMARGINBOX_HPP
#include "UIWidget.hpp"

namespace AlgAudio{

class UIMarginBox : public UIWidget{
public:
  static std::shared_ptr<UIMarginBox> Create(int top, int right, int bottom, int left);
  virtual void CustomDraw(const DrawContext& c) override;
  virtual void Insert(std::shared_ptr<UIWidget>);
  virtual void OnMouseButton(bool down, short b,int x,int y) override;
private:
  UIMarginBox(int top, int right, int bottom, int left);
  int top, right, bottom, left;
  std::shared_ptr<UIWidget> child;
};

} // namespace AlgAudio

#endif // UIMARGINBOX_HPP
