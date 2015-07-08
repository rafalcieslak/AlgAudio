#ifndef UIBUTTON_HPP
#define UIBUTTON_HPP
#include "UIWidget.hpp"

namespace AlgAudio{

class UIButton : public UIWidget{
public:
  UIButton(std::string text);
  virtual void Draw(DrawContext& c) override;
  virtual void OnMouseButton(bool,short,int,int) override;
private:
  std::string text;
};

} // namespace AlgAudio

#endif // UIBUTTON_HPP
