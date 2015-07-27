#ifndef CANVASVIEW_HPP
#define CANVASVIEW_HPP
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
#include "UI/ModuleGUI.hpp"
#include "Canvas.hpp"

namespace AlgAudio{

class CanvasView : public UIWidget{
public:
  static std::shared_ptr<CanvasView> CreateEmpty(std::shared_ptr<Window> parent);
  virtual void CustomDraw(DrawContext& c);
  LateReturn<> AddModule(std::string id, Point2D position);
private:
  CanvasView(std::shared_ptr<Window> parent);
  std::shared_ptr<Canvas> canvas;
  std::list<std::shared_ptr<ModuleGUI>> module_guis;
};

} // namespace AlgAudio

#endif //CANVASVIEW_HPP
