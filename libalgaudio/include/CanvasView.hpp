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
#include "Alertable.hpp"

namespace AlgAudio{

class CanvasView : public UIWidget{
public:
  static std::shared_ptr<CanvasView> CreateEmpty(std::shared_ptr<Window> parent);
  virtual void CustomDraw(DrawContext& c);
  LateReturn<> AddModule(std::string id, Point2D position);
  virtual bool CustomMousePress(bool,short,Point2D);
  virtual void CustomMouseEnter(Point2D);
  virtual void CustomMouseLeave(Point2D);
  virtual void CustomMouseMotion(Point2D,Point2D);
  void RemoveSelected();
private:
  CanvasView(std::shared_ptr<Window> parent);
  std::shared_ptr<Canvas> canvas;
  std::vector<std::shared_ptr<ModuleGUI>> module_guis;
  int InWhich(Point2D);
  int CurveStrengthFunc(Point2D a, Point2D b);
  bool mouse_down = false;
  enum MouseDownMode{
    ModeNone,
    ModeModuleBody,
    ModeCaptured,
    ModeInlet,
    ModeOutlet,
    ModeSlider,
  };
  MouseDownMode mouse_down_mode;
  std::string mouse_down_inletid, mouse_down_outletid, mouse_down_sliderid;
  Point2D mouse_down_position, drag_position;
  int mouse_down_id = -1;
  Point2D mouse_down_offset, drag_offset;
  int selected_id = -1;
  bool drag_in_progress = false;
  enum DragMode{
    DragModeMove,
    DragModeConnectFromInlet,
    DragModeConnectFromOutlet,
  };
  DragMode drag_mode;
  int dragged_id = -1;
  std::string drag_connection_io_start;
  void FinalizeConnectingDrag(int inlet_module_id, std::string inlet_id, int outlet_module_id, std::string outlet_id);
  // Use -1 to unselect
  void Select(int id);

  // Sets drag_in_progress to false, but also does extra cleanup.
  void StopDrag();

  // This is the green or red wire that is drawn when a new connection is almost
  // created or removed.
  enum class PotentialWireMode{
    None,
    New,
    Remove,
  };
  PotentialWireMode potential_wire = PotentialWireMode::None;
  std::pair<std::pair<int,std::string>, std::pair<int,std::string>> potential_wire_connection;

};

} // namespace AlgAudio

#endif //CANVASVIEW_HPP
