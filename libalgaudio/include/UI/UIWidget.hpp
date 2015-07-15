#ifndef UIWIDGET_HPP
#define UIWIDGET_HPP
#include <memory>
#include "DrawContext.hpp"
#include "Signal.hpp"
#include "Color.hpp"
#include "SDLTexture.hpp"

namespace AlgAudio{

class Window;

// shared_from_this is required for proper parent tracking
class UIWidget : public std::enable_shared_from_this<UIWidget>{
  // Pure abstract
public:
  UIWidget(std::weak_ptr<Window> parent_window)
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

  When a widget gets drawn, the draw area size is guaranteed to be of exactly
  the same dimentions, as were set by the last call to CustomResize. Therefore
  you can use CustomResize to calculate your children size, but remeber to pass
  the resize event down to them, by using Resize.

  At the moment when CustomResize is called, the previous widget size is still
  stored in current_size, the new size is passed as an argument to CustomResize
  (but you don't need to correct current_size on your own).
  */
  void Draw(DrawContext& c);
  virtual void CustomDraw(DrawContext& c) = 0;
  void Resize(Size2D s);
  virtual void CustomResize(Size2D) {};

  // Starts the downward resize-chain without actually changing size.
  // Useful for propagating size to a newly inserted child.
  void TriggerFakeResize() { CustomResize(current_size); }

  // Arguments: down, button, position
  virtual void OnMouseButton(bool, short, Point2D) {}
  // Arguments: from, to
  virtual void OnMotion(Point2D, Point2D) {}
  virtual void OnMotionLeave(Point2D) {}
  virtual void OnMotionEnter(Point2D) {}


  std::weak_ptr<UIWidget> parent;
  /*
  std::shared_ptr<UIWidget> GetRoot(){
    auto p = parent.lock();
    if(!p) return shared_from_this();
    return p->GetRoot();
  };
  std::weak_ptr<Window> GetWindow(){
    return GetRoot()->window;
  };
  void SetWindow(std::weak_ptr<Window> w){window = w;}
  */
  // Toggles widget display
  bool visible = true;

  const Size2D& GetRequestedSize() const{ return requested_size; }

protected:
  Size2D current_size;
  void SetNeedsRedrawing();
  // Only the topmost widget should be bound to a window.
  std::weak_ptr<Window> window;

  /* If you are implementng a custom widget, you will want to use
  SetRequestedSize a lot, to notify parent widgets what is the minimal area
  size that will be large enough for your widget to draw completely.
  However, by no means should you use SetRequestedSize while inside
  CustomResize. This could lead to huge problems, including stack loops and
  undefined widget state. On the other hand, you are very welcome to
  Resize while reacting on child's requested size change in
  OnChildRequestedSizeChanged. */
  void SetRequestedSize(Size2D);

  /* This method will be called by a child when it changes the desired size. */
  virtual void OnChildRequestedSizeChanged() {}

private:
  bool needs_redrawing = true;
  Size2D requested_size = Size2D(0,0);

  // This flag is used to track incorrect usage of SetRequestedSize()
  bool in_custom_resize = false;

  std::shared_ptr<SDLTexture> cache_texture;
  void RedrawToCache(Size2D size);
};

} // namespace AlgAudio

#endif //UIWIDGET_HPP
