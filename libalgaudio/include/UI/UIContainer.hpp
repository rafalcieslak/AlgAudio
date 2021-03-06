#ifndef UICONTAINER_HPP
#define UICONTAINER_HPP
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
#include "UIWidget.hpp"

namespace AlgAudio{

/** This is an interface for widgets which act as a container for a single child,
 *  like UICentered or UIMarginBox. */
class UIContainerSingle : public UIWidget{
public:
  virtual void Insert(std::shared_ptr<UIWidget>) {};
  virtual Point2D GetChildPos() const = 0;
  virtual void RemoveChild() {};
  virtual std::shared_ptr<UIWidget> CustomFindChild(ID id) const override {return child?child->FindChild(id):nullptr;}
  virtual void OnChildFocusRequested(std::shared_ptr<UIWidget>) override { 
    child->OnFocusChanged(true);
    child_is_focused = true;
    UIWidget::RequestFocus();
  }
  virtual void RequestFocus() override{
    if(child) child->OnFocusChanged(false);
    child_is_focused = false;
    UIWidget::RequestFocus();
  }
  virtual bool OnChildFocusTested(std::shared_ptr<const UIWidget>) override {
    return child_is_focused;
  }
  virtual void OnKeyboard(KeyData k) override { 
    if(child && child_is_focused) child->OnKeyboard(k);
  }
  virtual void OnFocusChanged(bool has_focus) override{
    if(child) child->OnFocusChanged(has_focus && child_is_focused);
  }
protected:
  UIContainerSingle(std::weak_ptr<Window> parent_window) : UIWidget(parent_window) {}
  std::shared_ptr<UIWidget> child;
  bool child_is_focused = false;
};

/** This is an interface for widgets which act as a container for multiple
 *  children, like UIBox or UILayered. */
class UIContainerMultiple : public UIWidget{
public:
  // Not forcing on an Insert method, as some widgets may require extra arguments
  // for insert
  // virtual void Insert(std::shared_ptr<UIWidget>) = 0;
  virtual Point2D GetChildPos(std::shared_ptr<UIWidget>) const = 0;
  virtual void Clear() = 0;
  
  virtual void OnChildFocusRequested(std::shared_ptr<UIWidget> w) override {
    auto old_focused_child = focused_child;
    
    if(w != focused_child){
      focused_child = nullptr;
        UIWidget::RequestFocus();
      focused_child = w;
      if(old_focused_child && old_focused_child != w) old_focused_child->OnFocusChanged(false);
      focused_child->OnFocusChanged(true);
    }else{
      UIWidget::RequestFocus();
    }
  }
  virtual void RequestFocus() override{
    if(focused_child) focused_child->OnFocusChanged(false);
    focused_child = nullptr;
    UIWidget::RequestFocus();
  }
  virtual bool OnChildFocusTested(std::shared_ptr<const UIWidget> w) override {
    return (w == focused_child) && IsFocused();
  }
  virtual void OnKeyboard(KeyData k) override{
    if(focused_child) focused_child->OnKeyboard(k);
  }
  virtual void OnFocusChanged(bool has_focus) override{
    if(focused_child) focused_child->OnFocusChanged(has_focus);
  }
protected:
  UIContainerMultiple(std::weak_ptr<Window> parent_window) : UIWidget(parent_window) {}
  // Not forcing what the child list looks like, some widgets may wish to store
  // some extra data wich each child.
  //std::list<std::shared_ptr<UIWidget>> child;

  std::shared_ptr<UIWidget> focused_child = nullptr;
};

} // namespace AlgAudio

#endif // UICONTAINER_HPP
