#ifndef UILABEL_HPP
#define UILABEL_HPP
#include "UIWidget.hpp"

namespace AlgAudio{

class UILabel : public UIWidget{
public:
  static std::shared_ptr<UILabel> Create(std::weak_ptr<Window> parent_window, std::string text, int size = 16, std::string color = "text-generic");
  void SetText(std::string);
  virtual void CustomDraw(DrawContext& c) override;
private:
  UILabel(std::weak_ptr<Window> parent_window, std::string text, int size = 16, std::string color = "text-generic");
  std::string text;
  std::string color;
  int fontsize;
  std::shared_ptr<SDLTexture> texture;
  void UpdateTexture();
};

} // namespace AlgAudio

#endif // UILABEL_HPP