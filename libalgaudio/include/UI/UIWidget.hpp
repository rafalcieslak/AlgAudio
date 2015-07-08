#ifndef UIWIDGET_HPP
#define UIWIDGET_HPP
#include <memory>
#include "DrawContext.hpp"

namespace AlgAudio{

class UIWindow;

// shared_from_this is required for proper parent tracking
class UIWidget : public std::enable_shared_from_this<UIWidget>{
  // Pure abstract
public:
  UIWidget() {};
  virtual ~UIWidget() {}
  // TODO: Draw cache
  virtual void Draw(DrawContext& c) = 0; // pure abstract
  // Arguments: down, button, x, y
  virtual void OnMouseButton(bool, short, int, int) {}
  std::weak_ptr<UIWidget> parent;
  std::weak_ptr<UIWindow> window;
  
/* It is recommended for widgets to implement Create static method,
   which returns a new shared_ptr of that object type, which simplifies
   the syntax for interface building. It shall be remembered, however,
   that a separate init() function may be destignated to be run once the
   constructor finishes, so that shared_from_this can work correctly
   (in case the this pointer is needed during initialisation, e.g. if a widget
   wishes to build a subinterface and parent pointers have to be filled).
*/
};

} // namespace AlgAudio

#endif //UIWIDGET_HPP
