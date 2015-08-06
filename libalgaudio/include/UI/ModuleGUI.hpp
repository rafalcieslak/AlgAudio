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

  // This set of helper structures and the WhatIsHere function are used
  // by the CanvasView to determine if the clicked point is an inlet, a slider,
  // module body, etc.
  enum class WhatIsHere_Types{
    Nothing,
    Inlet,
    Outlet,
  };
  struct WhatIsHere_data{
    WhatIsHere_Types type;
    std::string id;
  };
  WhatIsHere_data WhatIsHere(Point2D);
protected:
  ModuleGUI(std::shared_ptr<Window> w, std::shared_ptr<Module> mod) : UIWidget(w), module(mod){}
  // A link to the module instance this GUI represents.
  std::weak_ptr<Module> module;
};

} // namespace AlgAudio

#endif //MODULEGUI_HPP
