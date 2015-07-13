#ifndef UIMARGINBOX_HPP
#define UIMARGINBOX_HPP
#include "UIWidget.hpp"

namespace AlgAudio{

class UIMarginBox : public UIWidget{
public:
  static std::shared_ptr<UIMarginBox> Create(std::weak_ptr<Window> parent_window, int top, int right, int bottom, int left);
  virtual void CustomDraw(DrawContext& c) override;
  virtual void CustomResize(Size2D s) override;
  virtual void OnChildRequestedSizeChanged() override;
  virtual void Insert(std::shared_ptr<UIWidget>);
  virtual void OnMouseButton(bool down, short b,Point2D) override;
  virtual void OnMotion(Point2D,Point2D) override;
  virtual void OnMotionEnter(Point2D) override;
  virtual void OnMotionLeave(Point2D) override;
private:
  UIMarginBox(std::weak_ptr<Window> parent_window, int top, int right, int bottom, int left);
  inline bool IsInside(Point2D) const;
  int top, right, bottom, left;
  std::shared_ptr<UIWidget> child;
};

} // namespace AlgAudio

#endif // UIMARGINBOX_HPP
