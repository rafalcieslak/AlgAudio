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
#include "ModuleUI/ModuleGUI.hpp"
#include "Canvas.hpp"
#include "Alertable.hpp"

namespace AlgAudio{

/* This class is a graphical representation of a Canvas.
 * A CanvasView is always linked with a single Canvas instance.
 * It will draw the ModuleGUIs and connections between them. It will also
 * process mouse events, clicks and drags. By reacting on user input it asks
 * the underlying Canvas instance to connect/disconnect modules, etc.
 */
class CanvasView : public UIWidget{
public:
  // Creates a new empty instance of this widget.
  static std::shared_ptr<CanvasView> CreateEmpty(std::shared_ptr<Window> parent);
  virtual void CustomDraw(DrawContext& c);
  virtual bool CustomMousePress(bool,MouseButton,Point2D);
  virtual void CustomMouseEnter(Point2D);
  virtual void CustomMouseLeave(Point2D);
  virtual void CustomMouseMotion(Point2D,Point2D);
  virtual void OnKeyboard(KeyData);

  // This method (usually called by the parent) creates a new Module instance
  // on the underlying canvas according to the template with the given ID,
  // prepares its ModuleGUI and places it on the given position.
  LateReturn<> AddModule(std::string id, Point2D position);

  // This method removes the currencly selected module from both the CanvasView
  // and the underlying Canvas.
  void RemoveSelected();
  
  // Resets a view position to the one at the center of the bounding box that
  // has al module guis inside.
  void CenterView();

  // Switches the canvas this CV displays. If the second argument is true,
  // all ModuleGUIs will be build - so when switching the view context you
  // can ignore that argument, but when loading a file you should set it
  // to true.
  void SwitchCanvas(std::shared_ptr<Canvas> canvas, bool build_guis = false);
  std::shared_ptr<Canvas> GetCanvas() { return canvas; }

private:
  CanvasView(std::shared_ptr<Window> parent);
  // The link to the canvas containing Modules.
  std::shared_ptr<Canvas> canvas;
  // The collection of all maintained ModuleGUIs.
  std::vector<std::shared_ptr<ModuleGUI>> module_guis;
  // Used to determine which module was clicked knowing the click position.
  int InWhich(Point2D relative_pos);
  int CurveStrengthFuncA(Point2D a, Point2D b);
  int CurveStrengthFuncB(Point2D a, Point2D b);
  // These flags are set according to the mouse buttons state.
  bool lmb_down = false;
  bool mmb_down = false;
  // What happened the last time the mouse button was pressed down?
  // This information is important to determine what to do when a drag starts.
  enum MouseDownMode{
    ModeNone, // Nothing, mouse was outside any module
    ModeModuleBody, // Mouse was inside a module, but not on any widget.
    ModeCaptured, // Mouse was inside a module, but the ModuleGUI captured the click event.
    ModeInlet, // Mouse was over an inlet.
    ModeOutlet, // Mouse was over an outlet.
    ModeSlider, // Mouse was over a slider.
    ModeSliderInlet, // Mouse was over a slider.
    ModeSliderRelativeOutlet, // Mouse was over a slider's relative outlet.
    ModeSliderAbsoluteOutlet, // Mouse was over a slider's relative outlet.
  };
  MouseDownMode mouse_down_mode;
  // The Widget id of the element corresponding to current mouse_down_mode.
  UIWidget::ID mouse_down_elem_widgetid;
  // The iolet/param id of the element coresponding to the current mouse_down_mode;
  std::string mouse_down_elem_paramid;
  // The position where mouse was last pressed down.
  Point2D mouse_down_position, drag_position;
  // The number of the ModuleGUI mouse press happened over. Note that this
  // value only represents the position in ModuleGUIs vector.
  int mouse_down_id = -1;

  // The list of currently selected module guis. Each is stored with a corresponding
  // distance from drag start - necessary for calculating positions when moving
  // multiple modules.
  std::list<std::pair<std::shared_ptr<ModuleGUI>, Point2D>> selection;
  void ClearSelection();
  void SelectSingle(std::shared_ptr<ModuleGUI>);
  void AddToSelection(std::shared_ptr<ModuleGUI>);

  // The positions on the canvas are represented in two forms. The absolute
  // coordinates express the position in screenwise position, so that the top
  // left corner of the canvasview is always at 0,0 abs. Mouse events etc. will
  // receive absolute position information. In order to take view position into
  // account, the coordinates need to be transformed to relative format. All
  // module GUIs are placed at some fixed place in relative coordinates, and that
  // place does not change with zoom or view paning. Thus, when reacting on a
  // mouse event it is desirable to transform coordinates to relative format
  // before checking what lies on the clicked spot.
  inline Point2D PositionRelToAbs(Point2D rel){ return rel + current_size/2 - view_position; }
  inline Point2D PositionAbsToRel(Point2D abs){ return abs - current_size/2 + view_position; }

  // Is there any dragging action in progress?
  bool drag_in_progress = false;
  bool view_move_in_progress = false;
  // This enum determines what is the currrent drag action.
  enum DragMode{
    DragModeMove, // Changing a ModuleGUI position
    DragModeConnectAudioFromInlet, // Creating an audio connection from an inlet to an outlet
    DragModeConnectAudioFromOutlet, // Creating an audio connection from an outlet to an inlet
    DragModeConnectDataFromInlet, // Creating a data connection from an inlet to an outlet
    DragModeConnectDataFromRelativeOutlet, // Creating a data connection from an outlet to an inlet
    DragModeConnectDataFromAbsoluteOutlet, // Creating a data connection from an outlet to an inlet
    DragModeSlider, // Dragging a slider value
    DragModeBBSelect, // Bounding-box selection
  };
  DragMode drag_mode;
  // Helper function which is called right after a connection drag is completed.
  // It checks the connection for validity and asks the Canvas to make it.
  void FinalizeAudioConnectingDrag(int inlet_module_id, UIWidget::ID inlet_id, int outlet_module_id, UIWidget::ID outlet_id);
  void FinalizeDataConnectingDrag(int inlet_module_id, UIWidget::ID inlet_slider_id, int outlet_module_id, UIWidget::ID outlet_slider_id, Canvas::DataConnectionMode);

  // The position where mmb was pressed.
  Point2D mmb_down_pos_abs;
  // This variable stores what was the view_position value when the view drag
  // started.
  Point2D view_move_start_view_position;

  // Sets drag_in_progress to false, but also does extra cleanup.
  void StopDrag();

  // This is the green or red wire that is drawn when a new connection is almost
  // created or removed.
  enum class PotentialWireMode{
    None,
    New,
    Remove,
  };
  enum class PotentialWireType{
    Audio,
    DataRelative,
    DataAbsolute,
  };
  // A potential wire connection is the name for a graphical wire that is alraedy
  // drawn on the canvas (bright green), but was not yet created.
  PotentialWireMode potential_wire = PotentialWireMode::None;
  PotentialWireType potential_wire_type;
  std::pair<std::pair<int,std::string>, std::pair<int,std::string>> potential_wire_connection;
  // Animated fadeout of potential wire.
  Subscription fadeout_anim;
  PotentialWireMode fadeout_wire = PotentialWireMode::None;
  PotentialWireType fadeout_wire_type;
  std::pair<std::pair<std::weak_ptr<ModuleGUI>,std::string>, std::pair<std::weak_ptr<ModuleGUI>,std::string>> fadeout_wire_connection;
  void FadeoutWireStart(PotentialWireMode m);
  void FadeoutWireStep(float delta);
  float fadeout_phase;

  // Flags remebering held keys.
  bool shift_held, ctrl_held, alt_held;
  
  // A global offset for drawing all contents.
  Point2D view_position = Point2D(0,0);
  float view_scale = 0.5;
};

} // namespace AlgAudio

#endif //CANVASVIEW_HPP
