#ifndef UICHECKBOX_HPP
#define UICHECKBOX_HPP
#include "UIClickable.hpp"
#include "Theme.hpp"

namespace AlgAudio{

class UIHBox;
class UILabel;
class UIButton;

class UICheckbox : public UIClickable{
public:
  static std::shared_ptr<UICheckbox> Create(std::weak_ptr<Window> parent_window, std::string text);
  Signal<bool> on_toggled;
  void SetText(std::string);
  virtual void CustomDraw(DrawContext& c) override;
  virtual void CustomResize(Size2D s) override;
  virtual void OnChildRequestedSizeChanged() override;
  bool active = false;
private:
  UICheckbox(std::weak_ptr<Window> parent_window, std::string text);
  void Init();
  std::string text;
  std::shared_ptr<UIHBox> child_box;
  std::shared_ptr<UIButton> child_button;
  std::shared_ptr<UILabel> child_label;
};

} // namespace AlgAudio

#endif // UICHECKBOX_HPP
