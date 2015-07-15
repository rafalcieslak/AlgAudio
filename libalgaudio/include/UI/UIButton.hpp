#ifndef UIBUTTON_HPP
#define UIBUTTON_HPP
#include "UIClickable.hpp"
#include "Theme.hpp"

namespace AlgAudio{

class UIButton : public UIClickable{
public:
  static std::shared_ptr<UIButton> Create(std::weak_ptr<Window> parent_window, std::string text);
  void SetText(std::string);
  void SetColors(Color text, Color background);
  virtual void CustomDraw(DrawContext& c) override;
private:
  UIButton(std::weak_ptr<Window> parent_window, std::string text);
  std::string text;
  std::shared_ptr<SDLTexture> texture;
  Color bg_color;
  Color text_color;
  void UpdateTexture();
  Color GetBgColor() const;
};

} // namespace AlgAudio

#endif // UIBUTTON_HPP
