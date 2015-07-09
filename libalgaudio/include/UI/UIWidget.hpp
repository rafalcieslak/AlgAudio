#ifndef UIWIDGET_HPP
#define UIWIDGET_HPP
#include <memory>
#include "DrawContext.hpp"
#include "Signal.hpp"
#include "Utilities.hpp"
#include "SDLTexture.hpp"

namespace AlgAudio{

class UIWindow;

// shared_from_this is required for proper parent tracking
class UIWidget : public std::enable_shared_from_this<UIWidget>{
  // Pure abstract
public:
  UIWidget(std::weak_ptr<UIWindow> parent_window)
    : window(parent_window) {
      cache_texture = std::make_shared<SDLTexture>(parent_window, Size2D(1,1));
    };

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
  void Draw(DrawContext& c);
  virtual void CustomDraw(DrawContext& c) = 0; // pure abstract

  // Arguments: down, button, x, y
  virtual void OnMouseButton(bool, short, int, int) {}
  // Arguments: fromx, fromy, tox, toy
  virtual void OnMotion(int, int, int, int) {}
  virtual void OnMotionLeave() {}
  virtual void OnMotionEnter() {}


  std::weak_ptr<UIWidget> parent;
  std::weak_ptr<UIWindow> window;

  // Toggles widget display
  bool visible = true;

protected:
  Size2D last_drawn_size;
  void SetNeedsRedrawing();
private:
  bool needs_redrawing = true;
  std::shared_ptr<SDLTexture> cache_texture;
  void RedrawToCache(Size2D size);
};

} // namespace AlgAudio

#endif //UIWIDGET_HPP
