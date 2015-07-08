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
  virtual void OnMotion(int x1, int y1, int x2, int y2) override;
  virtual void OnMotionEnter() override;
  virtual void OnMotionLeave() override;
private:
  UIMarginBox(int top, int right, int bottom, int left);
  inline bool IsInside(int x, int y) const;
  int top, right, bottom, left;
  std::shared_ptr<UIWidget> child;
};

} // namespace AlgAudio

#endif // UIMARGINBOX_HPP
