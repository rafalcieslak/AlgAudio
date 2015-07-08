#ifndef UIBUTTON_HPP
#define UIBUTTON_HPP
#include "UIWidget.hpp"

namespace AlgAudio{

class UIButton : public UIWidget{
public:
  static std::shared_ptr<UIButton> Create(std::string text);
  virtual void CustomDraw(const DrawContext& c) override;
  virtual void OnMouseButton(bool,short,int,int) override;
private:
  UIButton(std::string text);
  std::string text;
};

} // namespace AlgAudio

#endif // UIBUTTON_HPP
