#ifndef UIWIDGET_HPP
#define UIWIDGET_HPP
#include <memory>
#include "DrawContext.hpp"

namespace AlgAudio{

class UIWindow;

class UIWidget{
  // Pure abstract
public:
  UIWidget() {};
  virtual ~UIWidget() {}
  // TODO: Draw cache
  virtual void Draw(DrawContext& c) = 0; // pure abstract
  std::weak_ptr<UIWidget> parent;
  std::weak_ptr<UIWindow> window;
};

} // namespace AlgAudio

#endif //UIWIDGET_HPP
