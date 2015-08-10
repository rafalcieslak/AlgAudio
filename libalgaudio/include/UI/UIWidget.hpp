#ifndef UIWIDGET_HPP
#define UIWIDGET_HPP
/*
This file is part of AlgAudio.

AlgAudio, Copyright (C) 2015 CeTA - Audiovisual Technology Center

AlgAudio is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

AlgAudio is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with AlgAudio.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <memory>
#include <iostream>
#include "DrawContext.hpp"
#include "Signal.hpp"
#include "Color.hpp"
#include "SDLTexture.hpp"
#include "UIMouseEventsBase.hpp"

namespace AlgAudio{

class Window;

// shared_from_this is required for proper parent tracking
class UIWidget : public UIMouseEventsBase, public virtual SubscriptionsManager, public std::enable_shared_from_this<UIWidget>{
protected:
  UIWidget(std::weak_ptr<Window> parent_window)
    : window(parent_window) {
      cache_texture = std::make_shared<SDLTexture>(parent_window, Size2D(0,0));
    };
public:
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

  std::weak_ptr<UIWidget> parent;

  // Toggles widget display
  void SetVisible(bool);
  bool IsVisible(){return visible;}

  /* The requested size depends on both minimal size and custom size.
     The minimal size is set by the particular widget implementation. For
     example, a Box container will set it's minimal size just large enough
     so that all it's children can be correctly drawn.
     The custom size is set by the widget's user. It is used if you want a
     widget to always take take the given area.
     The requested size is a dimention-wise maximum of minimal and custom sizes.
     Except for serval cases, a widget shall never be drawn smaller than its
     requested size. */
  Size2D GetRequestedSize() const;

  /* If you want to unset a custom-size, simply set the desired dimension(s) to
    0, so that minimal-size will always take precedence. */
  void SetCustomSize(Size2D size);

  void SetBackColor(const Color& c){
    clear_color = c;
    SetNeedsRedrawing();
  }
  void SetFrontColor(const Color& c){
    overlay_color = c;
    SetNeedsRedrawing();
  }

  bool HasParent(){
    // Conversion from shared_ptr to bool determines whether the
    // pointer is non-empty
    return (bool)(parent.lock());
  }

  // This function returns an offset at which this widget is drawn inside
  // ancestor. The argument can be either a diretct parent, or a transitive one.
  Point2D GetPosInParent(std::shared_ptr<UIWidget> ancestor);

  // The idea of this class is to make it impossible to accidentally
  // convert between a string and a widget ID.
  class ID{
  public:
    ID() : id("none") {}
    ID(const ID& other) : id(other.id) {}
    explicit ID(const std::string& s) : id(s) {}
    std::string ToString() const {return id;}
    bool operator==(const ID& other) const {return id == other.id;}
    bool operator!=(const ID& other) const {return id != other.id;}
    bool operator<(const ID& other) const {return id < other.id;}
    ID& operator=(const ID& other) {id = other.id; return *this;}
  private:
    std::string id;
  };
  ID widget_id;

  std::shared_ptr<UIWidget> FindChild(ID search_id){
    if(search_id == widget_id) return shared_from_this();
    else return CustomFindChild(search_id);
  }
  virtual std::shared_ptr<UIWidget> CustomFindChild(ID) const {return nullptr;}
protected:
  Size2D current_size;
  void SetNeedsRedrawing();
  // Only the topmost widget should be bound to a window.
  std::weak_ptr<Window> window;

  /* If you are implementng a custom widget, you will want to use
  SetMinimalSize a lot, to notify parent widgets what is the minimal area
  size that will be large enough for your widget to draw completely.
  However, by no means should you use SetMinimalSize while inside
  CustomResize. This could lead to huge problems, including stack loops and
  undefined widget state. On the other hand, you are very welcome to
  Resize while reacting on child's requested size change in
  OnChildRequestedSizeChanged. */
  void SetMinimalSize(Size2D);

  /* This method will be called by a child when it changes the desired size. */
  virtual void OnChildRequestedSizeChanged() {}
  /* This method will be called by a child when it changes its visibility. */
  virtual void OnChildVisibilityChanged() {}

private:
  Color clear_color = Color(0,0,0,0);
  Color overlay_color = Color(0,0,0,0);
  bool needs_redrawing = true;
  Size2D minimal_size = Size2D(0,0);
  Size2D custom_size = Size2D(0,0);

  // This flag is used to track incorrect usage of SetMinimalSize()
  bool in_custom_resize = false;

  bool visible = true;

  std::shared_ptr<SDLTexture> cache_texture;
  void RedrawToCache(Size2D size);
};


} // namespace AlgAudio
#endif //UIWIDGET_HPP
