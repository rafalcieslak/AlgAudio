#ifndef UIBUTTON_HPP
#define UIBUTTON_HPP
#include "UIWidget.hpp"
#include "Theme.hpp"

namespace AlgAudio{

class UIButton : public UIWidget{
public:
  static std::shared_ptr<UIButton> Create(std::weak_ptr<Window> parent_window, std::string text);
  Signal<> on_clicked;
  void SetText(std::string);
  void SetColors(Color text, Color background);
  virtual void CustomDraw(DrawContext& c) override;
  virtual void OnMouseButton(bool,short,Point2D) override;
  virtual void OnMotionEnter(Point2D) override;
  virtual void OnMotionLeave(Point2D) override;
private:
  UIButton(std::weak_ptr<Window> parent_window, std::string text);
  std::string text;
  std::shared_ptr<SDLTexture> texture;
  Color bg_color;
  Color text_color;
  void UpdateTexture();
  bool pressed = false;
  bool pointed = false;
};

} // namespace AlgAudio

#endif // UIBUTTON_HPP
