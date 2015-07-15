#ifndef UICLICKABLE_HPP
#define UICLICKABLE_HPP
#include "UIWidget.hpp"
#include "Theme.hpp"

namespace AlgAudio{

class UIClickable : public UIWidget{
public:
  Signal<> on_clicked;
  Signal<bool> on_pointed;
  Signal<bool> on_pressed;
  virtual void OnMouseButton(bool,short,Point2D) override;
  virtual void OnMotionEnter(Point2D) override;
  virtual void OnMotionLeave(Point2D) override;
protected:
  UIClickable(std::weak_ptr<Window> parent_window);
  bool pressed = false;
  bool pointed = false;
};

} // namespace AlgAudio

#endif // UICLICKABLE_HPP
