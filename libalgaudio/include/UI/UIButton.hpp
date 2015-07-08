#ifndef UIBUTTON_HPP
#define UIBUTTON_HPP
#include "UIWidget.hpp"

namespace AlgAudio{

class UIButton : public UIWidget{
public:
  static std::shared_ptr<UIButton> Create(std::string text);
  Signal<> on_clicked;
  virtual void CustomDraw(const DrawContext& c) override;
  virtual void OnMouseButton(bool,short,int,int) override;
  virtual void OnMotionEnter() override;
  virtual void OnMotionLeave() override;
private:
  UIButton(std::string text);
  std::string text;
  bool pressed = false;
  bool pointed = false;
};

} // namespace AlgAudio

#endif // UIBUTTON_HPP
