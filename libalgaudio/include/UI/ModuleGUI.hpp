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

// Merely an interface implemented by standard box types. Module creators
// may wish to implement a custom version of a module GUI, by inheriting
// from this class and writing a custom Module::BuildGUI.
class ModuleGUI : public UIWidget{
public:
  std::weak_ptr<Module> module;
  Point2D position;
protected:
  ModuleGUI(std::shared_ptr<Window> w) : UIWidget(w){}
};

} // namespace AlgAudio

#endif //MODULEGUI_HPP
