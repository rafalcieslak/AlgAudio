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

namespace AlgAudio{

struct GUIBuildException : public Exception{
  GUIBuildException(std::string t) : Exception(t) {}
};

// Merely an interface implemented by standard box types. Module creators
// may wish to implement a custom version of a module GUI, by inheriting
// from this class and writing a custom Module::BuildGUI.
class ModuleGUI : public UIWidget{
public:
  // The moduleGUI shall emit this signals when the user presses mouse button
  // over one of the inlets/outlets. Arguments: inlet/outlet ID, press state
  // (true = mouse button down, false = mouse button up).
  Signal<std::string, bool> on_inlet_pressed;
  Signal<std::string, bool> on_outlet_pressed;
  // The position of this module on the parent CanvasView.
  Point2D position;
  // When set to true, the module shall draw itself in it's "highlighted"
  // variant.
  virtual void SetHighlight(bool) = 0;
  // These methods are used by the CanvasView to query where it should draw
  // connection wire endings.
  virtual Point2D WhereIsInlet(std::string inlet) = 0;
  virtual Point2D WhereIsOutlet(std::string outlet) = 0;
  // Sets the link to module instance
  std::shared_ptr<Module> GetModule(){ return module.lock(); }

  // CanvasView uses this function to notify the ModuleGUI that a slider is
  // being dragged. This way the drag can continue outside the ModuleGUI.
  // Arguments: The slider id (as returned by WhatIsHere), starting position
  // (relative to moduleGUI position).
  virtual void SliderDragStart(std::string, Point2D){}
  // This method is called by CanvasView for each step of a slider drag.
  // Note that current_pos values may be outside moduleGUI, or even negative.
  virtual void SliderDragStep(std::string, Point2D){}
  virtual void SliderDragEnd(std::string, Point2D){}

  enum class WhatIsHereType{
    Nothing,
    Inlet,
    Outlet,
    SliderInput,
    SliderOutput,
    SliderBody,
  };
  // This function is used by the CanvasView to ask the module GUI what kind of
  // element is located at a given point. This function shall return a enum
  // value of WhatIsHereType and a string id (if applicable), for example for
  // inlets/outlets. This way the CanvasView can handle connections etc. and
  // ModuleGUI does not have to bother about them.
  virtual std::pair<WhatIsHereType, std::string> WhatIsHere(Point2D) const = 0;
protected:
  ModuleGUI(std::shared_ptr<Window> w, std::shared_ptr<Module> mod) : UIWidget(w), module(mod){}
  // A link to the module instance this GUI represents.
  std::weak_ptr<Module> module;
};

} // namespace AlgAudio

#endif //MODULEGUI_HPP
