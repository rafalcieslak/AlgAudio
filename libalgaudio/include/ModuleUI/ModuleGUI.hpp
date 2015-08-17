#ifndef MODULEGUI_HPP
#define MODULEGUI_HPP
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
#include "UI/UIWidget.hpp"
#include "Module.hpp"
#include "Canvas.hpp"

namespace AlgAudio{

struct GUIBuildException : public Exception{
  GUIBuildException(std::string t) : Exception(t) {}
};

// Merely an interface implemented by standard box types. Module creators
// may wish to implement a custom version of a module GUI, by inheriting
// from this class and writing a custom Module::BuildGUI.
class ModuleGUI : public UIWidget{
public:
  // The position of this module on the parent CanvasView.
  Point2D position;
  // When set to true, the module shall draw itself in it's "highlighted"
  // variant.
  virtual void SetHighlight(bool) = 0;

  // These methods are used by the CanvasView to query where it should draw
  // connection wire endings.
  // Arguments: WidgetIDs
  // virtual Point2D WhereIsInlet(UIWidget::ID inlet) = 0;
  // virtual Point2D WhereIsOutlet(UIWidget::ID outlet) = 0;
  // Arguments: iolet IDs
  virtual Point2D WhereIsInlet(std::string inlet) = 0;
  virtual Point2D WhereIsOutlet(std::string outlet) = 0;
  virtual Point2D WhereIsParamInlet(std::string inlet) = 0;
  virtual Point2D WhereIsParamOutlet(std::string inlet) = 0;

  // This method shall translate an inlet/outlet widget id to the corresponding
  // param id.
  virtual std::string GetIoletParamID(UIWidget::ID) const = 0;
  // Sets the link to module instance
  std::shared_ptr<Module> GetModule(){ return module.lock(); }

  // CanvasView uses this function to notify the ModuleGUI that a slider is
  // being dragged. This way the drag can continue outside the ModuleGUI.
  // Argument: The slider widget id (as returned by WhatIsHere)
  virtual void SliderDragStart(UIWidget::ID){}
  // This method is called by CanvasView for each step of a slider drag.
  // Note that offset values may be outside moduleGUI, or even negative.
  virtual void SliderDragStep(UIWidget::ID, Point2D_<float>){}
  virtual void SliderDragEnd(UIWidget::ID){}

  enum class WhatIsHereType{
    Nothing,
    Inlet,
    Outlet,
    SliderInput,
    SliderOutput,
    SliderBody,
  };
  struct WhatIsHere{
    WhatIsHereType type;
    // The id of the widget
    UIWidget::ID widget_id;
    // The id of the param/inlet/outlet corresponding to this widget
    std::string param_id;
  };
  // This function is used by the CanvasView to ask the module GUI what kind of
  // element is located at a given point. This way the CanvasView can handle
  // connections etc. and ModuleGUI does not have to bother about them.
  virtual WhatIsHere GetWhatIsHere(Point2D) const = 0;

protected:
  ModuleGUI(std::shared_ptr<Window> w, std::shared_ptr<Module> mod) : UIWidget(w), module(mod){}
  // A link to the module instance this GUI represents.
  std::weak_ptr<Module> module;
};

} // namespace AlgAudio

#endif //MODULEGUI_HPP
