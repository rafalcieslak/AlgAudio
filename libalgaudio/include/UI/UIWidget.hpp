#ifndef UIWIDGET_HPP
#define UIWIDGET_HPP
#include <memory>
#include "DrawContext.hpp"

namespace AlgAudio{

class UIWindow;

struct WidgetSize{
  WidgetSize(int w, int h) : width(w), height(h) {}
  int width, height;
};

// shared_from_this is required for proper parent tracking
class UIWidget : public std::enable_shared_from_this<UIWidget>{
  // Pure abstract
public:
  UIWidget() {};

/* It is recommended for widgets to implement Create static method,
   which returns a new shared_ptr of that object type, which simplifies
   the syntax for interface building. It shall be remembered, however,
   that a separate init() function may be destignated to be run once the
   constructor finishes, so that shared_from_this can work correctly
   (in case the this pointer is needed during initialisation, e.g. if a widget
   wishes to build a subinterface and parent pointers have to be filled).
*/

  virtual ~UIWidget() {}
  /*
  When implementing custom widgets you are expected to override CustomDraw
  with a specialized version. Do not implement your own Draw. If your widget
  has children and you wish to draw them too, use their Draw.
  Draw is basically a wrapper for CustomDraw, which performs visibility check,
  size clipping, resizing and texture caching. Use Draw, but write CustomDraw.
  */
  // TODO: Draw cache
  void Draw(const DrawContext& c);
  virtual void CustomDraw(const DrawContext& c) = 0; // pure abstract

  // Arguments: down, button, x, y
  virtual void OnMouseButton(bool, short, int, int) {}

  std::weak_ptr<UIWidget> parent;
  std::weak_ptr<UIWindow> window;

  // Toggles widget display
  bool visible = true;

private:
  int last_drawn_width, last_drawn_height;
};

} // namespace AlgAudio

#endif //UIWIDGET_HPP
