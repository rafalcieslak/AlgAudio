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
#include "CanvasView.hpp"
#include <algorithm>
#include "rapidxml/rapidxml.hpp"
#include "Window.hpp"
#include "SDLMain.hpp"

namespace AlgAudio{

CanvasView::CanvasView(std::shared_ptr<Window> parent) : UIWidget(parent){
}

std::shared_ptr<CanvasView> CanvasView::CreateEmpty(std::shared_ptr<Window> parent){
  auto ptr = std::shared_ptr<CanvasView>( new CanvasView(parent) );
  ptr->canvas = Canvas::CreateEmpty();
  return ptr;
}

void CanvasView::SwitchCanvas(std::shared_ptr<Canvas> c, bool build_guis){
  canvas = c;
  drag_in_progress = false;
  module_guis.clear();
  mouse_down_mode = ModeNone;
  potential_wire = PotentialWireMode::None;
  fadeout_wire = PotentialWireMode::None;
  fadeout_anim.Release();
  ClearSelection();

  if(build_guis){
    for(auto& m : c->modules){
      try{
        auto modulegui = m->BuildGUI(window.lock());
        Size2D guisize = modulegui->GetRequestedSize();
        modulegui->Resize(guisize);
        // Load stored data (e.g. loaded from save file or clipboard)
        if(m->guidata != ""){
          try{
            char buffer[10000];
            strncpy(buffer,m->guidata.c_str(),10000);
            rapidxml::xml_document<> doc;
            doc.parse<0>(buffer);
            rapidxml::xml_node<>* root = doc.first_node("gui");
            if(root){
              rapidxml::xml_attribute<>* x_attr = root->first_attribute("x");
              rapidxml::xml_attribute<>* y_attr = root->first_attribute("y");
              if(x_attr && y_attr){
                // Set modulegui position in canvas
                modulegui->position = Point2D( std::stoi(x_attr->value()), std::stoi(y_attr->value()));
              }
            }
          }catch(rapidxml::parse_error){
            /*ignore*/
          }catch(std::runtime_error){
            /*ignore*/
          }
        }
        modulegui->parent = shared_from_this();
        module_guis.push_back(modulegui);
      }catch(GUIBuildException ex){
        canvas->RemoveModule(m);
        window.lock()->ShowErrorAlert("Failed to create module GUI.\n\n" + ex.what(),"Dismiss");
      }
    }
  }

  SetNeedsRedrawing();
}

LateReturn<> CanvasView::AddModule(std::string id, Point2D pos){
  auto r = Relay<>::Create();
  canvas->CreateModule(id).Then([this,r,pos](std::shared_ptr<Module> m){
    try{
      auto modulegui = m->BuildGUI(window.lock());
      Size2D guisize = modulegui->GetRequestedSize();
      modulegui->position = pos - guisize/2;
      modulegui->parent = shared_from_this();
      modulegui->Resize(guisize);
      module_guis.push_back(modulegui);
      ClearSelection();
      selection.push_back({modulegui, (guisize/2).ToPoint()});
      modulegui->SetHighlight(true);
      drag_in_progress = true;
      drag_mode = DragModeMove;
      SetNeedsRedrawing();
      r.Return();
    }catch(GUIBuildException ex){
      canvas->RemoveModule(m);
      window.lock()->ShowErrorAlert("Failed to create module GUI.\n\n" + ex.what(),"Dismiss");
      r.Return();
      return;
    }
  });
  return r;
}
void CanvasView::CustomDraw(DrawContext& c){
  // For each modulegui, draw the modulegui.
  for(auto& modulegui : module_guis){
    c.Push(modulegui->position, modulegui->GetRequestedSize());
    modulegui->Draw(c);
    c.Pop();
  }
  // Then draw all the audio connections...
  // TODO: Connection ending offsets should be cached. Asking each module gui
  // about the io position every time when redrawing is not going to be
  // efficient when there are 100+ modules present.
  c.SetColor(Theme::Get("canvas-connection-audio"));
  for(auto it : canvas->audio_connections){
    Canvas::IOID from = it.first;
    Point2D from_pos = from.module->GetGUI()->position + from.module->GetGUI()->WhereIsOutlet(from.iolet);
    // For each target of this outlet
    std::list<Canvas::IOID> to_list = it.second;
    for(auto to : to_list){
      Point2D to_pos = to.module->GetGUI()->position + to.module->GetGUI()->WhereIsInlet(to.iolet);
      int strength = CurveStrengthFuncA(from_pos, to_pos);
      c.DrawCubicBezier(from_pos, from_pos + Point2D(0,strength), to_pos + Point2D(0, -strength), to_pos);
    }
  }
  // Next, data connections.
  for(auto it : canvas->data_connections){
    Canvas::IOID from = it.first;
    Point2D from_pos_relative = from.module->GetGUI()->position + from.module->GetGUI()->WhereIsParamRelativeOutlet(from.iolet);
    Point2D from_pos_absolute = from.module->GetGUI()->position + from.module->GetGUI()->WhereIsParamAbsoluteOutlet(from.iolet);
    // For each target of this data outlet:
    std::list<Canvas::IOIDWithMode> to_list = it.second;
    for(auto to : to_list){
      c.SetColor(Theme::Get( (to.mode == Canvas::DataConnectionMode::Relative) ? "canvas-connection-data-relative" : "canvas-connection-data-absolute"));
      Point2D to_pos = to.ioid.module->GetGUI()->position + to.ioid.module->GetGUI()->WhereIsParamInlet(to.ioid.iolet);
      Point2D from_pos = (to.mode == Canvas::DataConnectionMode::Relative) ? from_pos_relative : from_pos_absolute;
      int strength = CurveStrengthFuncB(from_pos, to_pos);
      c.DrawCubicBezier(from_pos, from_pos + Point2D(strength,0), to_pos + Point2D(-strength, 0), to_pos, 15, 1.0f);
    }
  }

  // Then draw the potential new wire.
  if(drag_in_progress && potential_wire != PotentialWireMode::None){

    std::shared_ptr<ModuleGUI> from_mgui = module_guis[potential_wire_connection.first .first],
                                 to_mgui = module_guis[potential_wire_connection.second.first];
    std::string from_outlet_paramid = potential_wire_connection.first .second,
                   to_inlet_paramid = potential_wire_connection.second.second;

    if(potential_wire == PotentialWireMode::Remove) c.SetColor(Theme::Get("canvas-connection-remove"));
    else                                                 c.SetColor(Theme::Get("canvas-connection-new"));

    if(potential_wire_type == PotentialWireType::Audio){
      // Potential audio wire
      Point2D p1 = from_mgui->position + from_mgui->WhereIsOutlet(from_outlet_paramid);
      Point2D p2 =   to_mgui->position +   to_mgui->WhereIsInlet (   to_inlet_paramid);
      int strength = CurveStrengthFuncA(p1, p2);
      c.DrawCubicBezier(p1, p1 + Point2D(0, strength), p2 + Point2D(0, -strength), p2);
    }else{
      // Potential data wire
      Point2D outlet_pos = (potential_wire_type == PotentialWireType::DataRelative) ?
                            from_mgui->WhereIsParamRelativeOutlet(from_outlet_paramid) :
                            from_mgui->WhereIsParamAbsoluteOutlet(from_outlet_paramid);
      Point2D p1 = from_mgui->position + outlet_pos;
      Point2D p2 =   to_mgui->position +   to_mgui->WhereIsParamInlet (   to_inlet_paramid);
      int strength = CurveStrengthFuncB(p1, p2);
      c.DrawCubicBezier(p1, p1 + Point2D(strength,0), p2 + Point2D(-strength, 0), p2, 15, 1.0f);
    }


  }else if(drag_in_progress){
    // This is a normal drag in progress.
    // Simply draw the currently dragged line...
    if(drag_mode == DragModeConnectAudioFromOutlet){
      c.SetColor(Theme::Get("canvas-connection-audio"));
      Point2D p = module_guis[mouse_down_id]->position + module_guis[mouse_down_id]->WhereIsOutlet(mouse_down_elem_paramid);
      int strength = CurveStrengthFuncA(p, drag_position);
      c.DrawCubicBezier(p, p + Point2D(0,strength), drag_position + Point2D(0, -strength/2), drag_position);
    }else if(drag_mode == DragModeConnectAudioFromInlet){
      c.SetColor(Theme::Get("canvas-connection-audio"));
      Point2D p = module_guis[mouse_down_id]->position + module_guis[mouse_down_id]->WhereIsInlet(mouse_down_elem_paramid);
      int strength = CurveStrengthFuncA(p, drag_position);
      c.DrawCubicBezier(p, p + Point2D(0,-strength), drag_position + Point2D(0, strength/2), drag_position);
    }else if(drag_mode == DragModeConnectDataFromRelativeOutlet){
      c.SetColor(Theme::Get("canvas-connection-data-relative"));
      Point2D p = module_guis[mouse_down_id]->position + module_guis[mouse_down_id]->WhereIsParamRelativeOutlet(mouse_down_elem_paramid);
      int strength = CurveStrengthFuncB(p, drag_position);
      c.DrawCubicBezier(p, p + Point2D(strength,0), drag_position + Point2D(-strength/2, 0), drag_position, 15, 1.0f);
    }else if(drag_mode == DragModeConnectDataFromAbsoluteOutlet){
      c.SetColor(Theme::Get("canvas-connection-data-absolute"));
      Point2D p = module_guis[mouse_down_id]->position + module_guis[mouse_down_id]->WhereIsParamAbsoluteOutlet(mouse_down_elem_paramid);
      int strength = CurveStrengthFuncB(p, drag_position);
      c.DrawCubicBezier(p, p + Point2D(strength,0), drag_position + Point2D(-strength/2, 0), drag_position, 15, 1.0f);
    }else if(drag_mode == DragModeConnectDataFromInlet){
      c.SetColor(Theme::Get("canvas-connection-data-relative"));
      Point2D p = module_guis[mouse_down_id]->position + module_guis[mouse_down_id]->WhereIsParamInlet(mouse_down_elem_paramid);
      int strength = CurveStrengthFuncB(p, drag_position);
      c.DrawCubicBezier(p, p + Point2D(-strength,0), drag_position + Point2D(strength/2, 0), drag_position, 15, 1.0f);
    }else if(drag_mode == DragModeBBSelect){
      c.SetColor(Theme::Get("canvas-bb-body"));
      Rect r(drag_position, mouse_down_position);
      c.DrawRect(r);
      c.SetColor(Theme::Get("canvas-bb-border"));
      c.DrawRectBorder(r);
    }
  }

  if(fadeout_wire != PotentialWireMode::None){
    // Draw the fadeout wire
    std::shared_ptr<ModuleGUI> from_mgui = fadeout_wire_connection.first .first.lock(),
                                 to_mgui = fadeout_wire_connection.second.first.lock();
    std::string from_outlet_paramid = fadeout_wire_connection.first .second,
                   to_inlet_paramid = fadeout_wire_connection.second.second;

    if(from_mgui && to_mgui){
      if(fadeout_wire == PotentialWireMode::Remove) c.SetColor(Theme::Get("canvas-connection-remove").SetAlpha(fadeout_phase));
      else                                          c.SetColor(Theme::Get("canvas-connection-new").SetAlpha(fadeout_phase));

      if(fadeout_wire_type == PotentialWireType::Audio){
        Point2D p1 = from_mgui->position + from_mgui->WhereIsOutlet(from_outlet_paramid);
        Point2D p2 =   to_mgui->position +   to_mgui->WhereIsInlet (   to_inlet_paramid);
        int strength = CurveStrengthFuncA(p1, p2);
        c.DrawCubicBezier(p1, p1 + Point2D(0, strength), p2 + Point2D(0, -strength), p2);
      }else{

        Point2D outlet_pos = (fadeout_wire_type == PotentialWireType::DataRelative) ?
                              from_mgui->WhereIsParamRelativeOutlet(from_outlet_paramid) :
                              from_mgui->WhereIsParamAbsoluteOutlet(from_outlet_paramid);
        Point2D p1 = from_mgui->position + outlet_pos;
        Point2D p2 =   to_mgui->position +   to_mgui->WhereIsParamInlet (   to_inlet_paramid);
        int strength = CurveStrengthFuncB(p1, p2);
        c.DrawCubicBezier(p1, p1 + Point2D(strength,0), p2 + Point2D(-strength, 0), p2, 15, 1.0f);
      }
    }
  } // if fadeout wire is not none

}

int CanvasView::CurveStrengthFuncA(Point2D a, Point2D b){
  return std::min(std::abs(a.x - b.x) + std::abs(a.y - b.y)/3, 90);
}
int CanvasView::CurveStrengthFuncB(Point2D a, Point2D b){
  return std::min(std::abs(a.x - b.x)/3 + std::abs(a.y - b.y)/4, 90);
}

int CanvasView::InWhich(Point2D p){
  for(int i = module_guis.size() - 1; i >= 0; i--){
    if(p.IsInside(module_guis[i]->position, module_guis[i]->GetRequestedSize()) )
      return i;
  }
  return -1;
}

bool CanvasView::CustomMousePress(bool down,MouseButton b,Point2D pos){
  int id = InWhich(pos);
  Point2D offset;

  if(id >=0 ){
    offset = pos - module_guis[id]->position;
  }
  if(down == true && b == MouseButton::Left){

    // When a module was just added, it is dragged with LMB up. The drag is
    // stopped by clicking. We want to ignore that click, so generally let's skip
    // all mouse down events that happen during a drag.
    if(drag_in_progress) return true;

    // Mouse button down
    lmb_down = true;
    mouse_down_position = pos;
    mouse_down_id = id;
    mouse_down_mode = ModeNone;
    if(id < 0){
      // Mouse buton down on an empty space
      if(down == true && b == MouseButton::Left)
        ClearSelection();
    }else{
      // Mouse button down on some module

      auto whatishere = module_guis[id]->GetWhatIsHere(offset);
      if(whatishere.type == ModuleGUI::WhatIsHereType::Inlet){
        //std::cout << "Mouse down on inlet" << std::endl;
        mouse_down_mode =  ModeInlet;
        mouse_down_elem_widgetid = whatishere.widget_id;
        mouse_down_elem_paramid = whatishere.param_id;
      }else if(whatishere.type == ModuleGUI::WhatIsHereType::Outlet){
        //std::cout << "Mouse down on outlet" << std::endl;
        mouse_down_mode =  ModeOutlet;
        mouse_down_elem_widgetid = whatishere.widget_id;
        mouse_down_elem_paramid = whatishere.param_id;
      }else if(whatishere.type == ModuleGUI::WhatIsHereType::SliderBody){
        if(!shift_held){
          // Holding shift starts relative slider drag.
          // So when the slider is clicked with shift held down, we don't want
          // it to jump to the pointed position.
          module_guis[id]->OnMousePress(true, MouseButton::Left, offset);
        }
        mouse_down_mode = ModeSlider; // The slider is not a part of the main module body.
        mouse_down_elem_widgetid = whatishere.widget_id;
        mouse_down_elem_paramid = whatishere.param_id;
      }else if(whatishere.type == ModuleGUI::WhatIsHereType::SliderInput){
        mouse_down_mode = ModeSliderInlet;
        mouse_down_elem_widgetid = whatishere.widget_id;
        mouse_down_elem_paramid = whatishere.param_id;
      }else if(whatishere.type == ModuleGUI::WhatIsHereType::SliderOutputRelative){
        mouse_down_mode = ModeSliderRelativeOutlet;
        mouse_down_elem_widgetid = whatishere.widget_id;
        mouse_down_elem_paramid = whatishere.param_id;
      }else if(whatishere.type == ModuleGUI::WhatIsHereType::SliderOutputAbsolute){
        mouse_down_mode = ModeSliderAbsoluteOutlet;
        mouse_down_elem_widgetid = whatishere.widget_id;
        mouse_down_elem_paramid = whatishere.param_id;
      }else if(whatishere.type == ModuleGUI::WhatIsHereType::Nothing){
        bool captured = module_guis[id]->OnMousePress(true, MouseButton::Left, offset);
        if(!captured) mouse_down_mode = ModeModuleBody;
        else mouse_down_mode = ModeCaptured;
      }else{
        mouse_down_mode = ModeNone;
      }

      if(mouse_down_mode == ModeModuleBody){
        auto mgui = module_guis[id];
        bool already_selected = false;
        for(auto p : selection)
          if(p.first == mgui)
            already_selected = true;

        if(already_selected){
          // This module is already selected!
          // TODO: Unselect it if shift is pressed
        }else{
          // This module is not selected.
          if(shift_held) AddToSelection(mgui);
          else SelectSingle(mgui);
        }
      }
    }
  }
  if(down == false && b == MouseButton::Left){
    // Mouse button up
    lmb_down = false;

    if(drag_in_progress){
      if(drag_mode == DragModeSlider){
        // Slider drag end
        module_guis[mouse_down_id]->SliderDragEnd(mouse_down_elem_widgetid);
      }else if(drag_mode == DragModeConnectAudioFromInlet || drag_mode == DragModeConnectAudioFromOutlet){
        // Connection drag end
        if(id >= 0){
          auto whatishere = module_guis[id]->GetWhatIsHere(offset);
          if(drag_mode == DragModeConnectAudioFromInlet && whatishere.type == ModuleGUI::WhatIsHereType::Outlet)
            FinalizeAudioConnectingDrag(mouse_down_id, mouse_down_elem_widgetid, id, whatishere.widget_id);
          else if(drag_mode == DragModeConnectAudioFromOutlet && whatishere.type == ModuleGUI::WhatIsHereType::Inlet)
            FinalizeAudioConnectingDrag(id, whatishere.widget_id, mouse_down_id, mouse_down_elem_widgetid);
          else{
            // Connecting drag ended on module, but not on an inlet/outlet.
          }
        }else{
          // Connecting drag ended on empty space.
        }
      }else if(drag_mode == DragModeConnectDataFromInlet || drag_mode == DragModeConnectDataFromRelativeOutlet || drag_mode == DragModeConnectDataFromAbsoluteOutlet){
        // Data connection drag end
        if(id >= 0){
          auto whatishere = module_guis[id]->GetWhatIsHere(offset);
          if(drag_mode == DragModeConnectDataFromInlet && whatishere.type == ModuleGUI::WhatIsHereType::SliderOutputRelative)
            FinalizeDataConnectingDrag(mouse_down_id, mouse_down_elem_widgetid, id, whatishere.widget_id, Canvas::DataConnectionMode::Relative);
          else if(drag_mode == DragModeConnectDataFromInlet && whatishere.type == ModuleGUI::WhatIsHereType::SliderOutputAbsolute)
            FinalizeDataConnectingDrag(mouse_down_id, mouse_down_elem_widgetid, id, whatishere.widget_id, Canvas::DataConnectionMode::Absolute);
          else if(drag_mode == DragModeConnectDataFromRelativeOutlet && whatishere.type == ModuleGUI::WhatIsHereType::SliderInput)
            FinalizeDataConnectingDrag(id, whatishere.widget_id, mouse_down_id, mouse_down_elem_widgetid, Canvas::DataConnectionMode::Relative);
          else if(drag_mode == DragModeConnectDataFromAbsoluteOutlet && whatishere.type == ModuleGUI::WhatIsHereType::SliderInput)
            FinalizeDataConnectingDrag(id, whatishere.widget_id, mouse_down_id, mouse_down_elem_widgetid, Canvas::DataConnectionMode::Absolute);
          else{
            // Data onnecting drag ended on module, but not on a slider inlet/outlet.
          }
        }else{
          // Data connecting drag ended on empty space.
        }
      }else if(drag_mode == DragModeBBSelect){
        // BB select drag end
        ClearSelection();
        Rect bb = {drag_position, mouse_down_position};
        for(auto mgui : module_guis){
          Rect r = {mgui->position,  mgui->GetCurrentSize()};
          if(r.IsFullyInside(bb))
            AddToSelection(mgui);
        }
      }
      StopDrag();
      // Redrawing to clear the drag-wire.
      SetNeedsRedrawing();
    }else{
      // No drag in progress.
      if(id >= 0) module_guis[id]->OnMousePress(false, MouseButton::Left, offset);
    }
  }
  return true;
}

void CanvasView::FinalizeAudioConnectingDrag(int inlet_module_id, UIWidget::ID inlet_widget_id, int outlet_module_id, UIWidget::ID outlet_widget_id){
  std::cout << "Finalizing drag from " << inlet_module_id << "/" << inlet_widget_id.ToString() << " to " << outlet_module_id << "/" << outlet_widget_id.ToString() << std::endl;
  std::shared_ptr<Module> from_module = module_guis[outlet_module_id]->GetModule();
  std::shared_ptr<Module> to_module   =  module_guis[inlet_module_id]->GetModule();
  if(!from_module || !to_module){
    window.lock()->ShowErrorAlert("Failed to create connection, one of the corresponding modules does not exist." , "Cancel");
    return;
  }
  std::string outlet_id = module_guis[outlet_module_id]->GetIoletParamID(outlet_widget_id);
  std::string inlet_id = module_guis[inlet_module_id]->GetIoletParamID(inlet_widget_id);
  std::cout << "Which corresponds to " << inlet_module_id << "/" << inlet_id << " to " << outlet_module_id << "/" << outlet_id << std::endl;
  Canvas::IOID from = {from_module,outlet_id};
  Canvas::IOID   to = {  to_module,inlet_id };
  if(!canvas->GetDirectAudioConnectionExists(from, to)){
    // There is no such connection, connect!
    try{
      canvas->Connect(from,to);
      FadeoutWireStart(PotentialWireMode::New);
    }catch(MultipleConnectionsException ex){
      window.lock()->ShowErrorAlert(ex.what(), "Cancel connection");
    }catch(ConnectionLoopException ex){
      window.lock()->ShowErrorAlert(ex.what(), "Cancel connection");
    }catch(DoubleConnectionException ex){
      window.lock()->ShowErrorAlert(ex.what(), "Cancel connection");
    }
  }else{
    // This connection already exists, remove it.
    canvas->Disconnect(from,to);
    FadeoutWireStart(PotentialWireMode::Remove);
  }
  SetNeedsRedrawing();
}


void CanvasView::FinalizeDataConnectingDrag(int inlet_module_id, UIWidget::ID inlet_slider_id, int outlet_module_id, UIWidget::ID outlet_slider_id, Canvas::DataConnectionMode mode){
  std::cout << "Finalizing data drag from " << inlet_module_id << "/" << inlet_slider_id.ToString() << " to " << outlet_module_id << "/" << outlet_slider_id.ToString() << std::endl;
  std::shared_ptr<Module> from_module = module_guis[outlet_module_id]->GetModule();
  std::shared_ptr<Module> to_module   =  module_guis[inlet_module_id]->GetModule();
  if(!from_module || !to_module){
    window.lock()->ShowErrorAlert("Failed to create connection, one of the corresponding modules does not exist." , "Cancel");
    return;
  }
  std::string param1_id = module_guis[outlet_module_id]->GetIoletParamID(outlet_slider_id);
  std::string param2_id = module_guis[inlet_module_id]->GetIoletParamID(inlet_slider_id);
  std::cout << "Which corresponds to " << inlet_module_id << "/" << param1_id << " to " << outlet_module_id << "/" << param2_id << std::endl;

  Canvas::IOID from = {from_module,param1_id};
  Canvas::IOID   to = {  to_module,param2_id };
  if(!canvas->GetDirectDataConnectionExists(from, to).first){
    try{
      canvas->ConnectData(from,to,mode);
      FadeoutWireStart(PotentialWireMode::New);
    }catch(MultipleConnectionsException ex){
      window.lock()->ShowErrorAlert(ex.what(), "Cancel connection");
    }catch(ConnectionLoopException ex){
      window.lock()->ShowErrorAlert(ex.what(), "Cancel connection");
    }catch(DoubleConnectionException ex){
      window.lock()->ShowErrorAlert(ex.what(), "Cancel connection");
    }
  }else{
    // This connection already exists, remove it.
    canvas->DisconnectData(from,to);
    FadeoutWireStart(PotentialWireMode::Remove);
  }
}

void CanvasView::SelectSingle(std::shared_ptr<ModuleGUI> mgui){
  ClearSelection();
  AddToSelection(mgui);
}

void CanvasView::AddToSelection(std::shared_ptr<ModuleGUI> mgui){
  selection.push_back({mgui, Point2D(0,0)});
  mgui->SetHighlight(true);
}

void CanvasView::ClearSelection(){
  for(auto p : selection){
    std::shared_ptr<ModuleGUI> mgui = p.first;
    mgui->SetHighlight(false);
  }
  selection.clear();
}

void CanvasView::CustomMouseEnter(Point2D pos){
  int id = InWhich(pos);
  if(id != -1){
    module_guis[id]->OnMouseEnter(pos);
  }
}
void CanvasView::CustomMouseLeave(Point2D pos){
  if(lmb_down) lmb_down = false;
  if(drag_in_progress) StopDrag();
  int id = InWhich(pos);
  if(id != -1){
    module_guis[id]->OnMouseLeave(pos);
  }
}
void CanvasView::CustomMouseMotion(Point2D from,Point2D to){
  if(drag_in_progress){
    // A drag is already in progress.
    drag_position = to;

    // Gather drag position data
    int id = InWhich(drag_position);
    ModuleGUI::WhatIsHere whatishere;
    if(id >= 0) whatishere = module_guis[id]->GetWhatIsHere(drag_position - module_guis[id]->position);

    // By default, there is not potential wire. This will be overriden to a wire
    // type soon below.
    potential_wire = PotentialWireMode::None;

    if(drag_mode == DragModeMove){
      // Update selected widgets positions.
      for(auto& p : selection)
        p.first->position = drag_position - p.second;

    }else if(id >= 0 && // If the drag is over some widget
            ( (whatishere.type == ModuleGUI::WhatIsHereType::Outlet && drag_mode == DragModeConnectAudioFromInlet ) || // and it's a audio connecting drag over outlet
              (whatishere.type == ModuleGUI::WhatIsHereType::Inlet  && drag_mode == DragModeConnectAudioFromOutlet) )  //   or over inlet
            ){
      int from_id, to_id; std::string from_outlet_paramid, to_inlet_paramid;
      if(drag_mode == DragModeConnectAudioFromInlet){
        from_id = id;
        to_id = mouse_down_id;
        from_outlet_paramid = whatishere.param_id;
        to_inlet_paramid = mouse_down_elem_paramid;
      }else{
        from_id = mouse_down_id;
        to_id = id;
        from_outlet_paramid = mouse_down_elem_paramid;
        to_inlet_paramid =  whatishere.param_id;
      }
      if(canvas->GetDirectAudioConnectionExists( // If there is already an audio connection between these two
        {module_guis[from_id]->GetModule(), from_outlet_paramid},
        {module_guis[to_id]->GetModule(),    to_inlet_paramid})
      )
        potential_wire = PotentialWireMode::Remove;
      else
        potential_wire = PotentialWireMode::New;

      // Store porential connection data
      potential_wire_type = PotentialWireType::Audio;
      potential_wire_connection = {{from_id, from_outlet_paramid}, {to_id, to_inlet_paramid}};

    }else if( id >= 0 && // If the drag is over some widget
              drag_mode == DragModeConnectDataFromInlet && // and it's data connecting drag from inlet
              (whatishere.type == ModuleGUI::WhatIsHereType::SliderOutputRelative || whatishere.type == ModuleGUI::WhatIsHereType::SliderOutputAbsolute) // and the mouse is now over an output connector
            ){
      auto res = canvas-> GetDirectDataConnectionExists( // Test if there is already some connection between these params
        { module_guis[id]->GetModule(), whatishere.param_id},
        { module_guis[mouse_down_id]->GetModule(), mouse_down_elem_paramid});
      if(res.first){
        // Connection exists
        potential_wire = PotentialWireMode::Remove;
        potential_wire_type = (res.second == Canvas::DataConnectionMode::Relative) ? PotentialWireType::DataRelative : PotentialWireType::DataAbsolute;
      }else{
        // No connection
        potential_wire = PotentialWireMode::New;
        potential_wire_type = (whatishere.type == ModuleGUI::WhatIsHereType::SliderOutputRelative) ? PotentialWireType::DataRelative : PotentialWireType::DataAbsolute;
      }
      // Store porential connection data
      potential_wire_connection = {{id, whatishere.param_id}, {mouse_down_id, mouse_down_elem_paramid}};

    }else if( id >= 0 && // If the drag is over some widget
             (drag_mode == DragModeConnectDataFromRelativeOutlet || drag_mode == DragModeConnectDataFromAbsoluteOutlet) && // and it's data connecting drag from outlet
             whatishere.type == ModuleGUI::WhatIsHereType::SliderInput // and the mouse is now over an inlet
           ){
      auto res = canvas-> GetDirectDataConnectionExists( // Test if there is already some connection between these params
        { module_guis[mouse_down_id]->GetModule(), mouse_down_elem_paramid},
        { module_guis[id]->GetModule(), whatishere.param_id});
      if(res.first){
        // Connection exists
        potential_wire = PotentialWireMode::Remove;
        potential_wire_type = (res.second == Canvas::DataConnectionMode::Relative) ? PotentialWireType::DataRelative : PotentialWireType::DataAbsolute;
      }else{
        // No connection
        potential_wire = PotentialWireMode::New;
        potential_wire_type = (drag_mode == DragModeConnectDataFromRelativeOutlet) ? PotentialWireType::DataRelative : PotentialWireType::DataAbsolute;
      }
      // Store porential connection data
      potential_wire_connection = {{mouse_down_id, mouse_down_elem_paramid}, {id, whatishere.param_id}};

    }else if(drag_mode == DragModeSlider){
      Point2D_<float> offset = drag_position - mouse_down_position;
      if(shift_held) offset = offset/16.0;
      module_guis[mouse_down_id]->SliderDragStep(mouse_down_elem_widgetid, offset);
    }else if(drag_mode == DragModeBBSelect){
      //SetNeedsRedrawing(); // Will be set later on anyway.
    }

    // Any drag in progress requires constant redrawing.
    SetNeedsRedrawing();
  }else{
    // No drag in progress.
    if(lmb_down && Point2D::Distance(mouse_down_position, to) > 5 &&
      ( mouse_down_mode == ModeModuleBody ||
        mouse_down_mode == ModeInlet      ||
        mouse_down_mode == ModeOutlet     ||
        mouse_down_mode == ModeSliderInlet ||
        mouse_down_mode == ModeSliderRelativeOutlet ||
        mouse_down_mode == ModeSliderAbsoluteOutlet ||
        mouse_down_mode == ModeNone ) ) {
      //std::cout << "DRAG start" << std::endl;
      drag_in_progress = true;
      if(mouse_down_mode == ModeModuleBody){
        drag_mode = DragModeMove;
        // Store move drag offsets.
        for(auto &p : selection)
          p.second = mouse_down_position - p.first->position;

      }else if(mouse_down_mode == ModeInlet){
        drag_mode = DragModeConnectAudioFromInlet;
      }else if(mouse_down_mode == ModeOutlet){
        drag_mode = DragModeConnectAudioFromOutlet;
      }else if(mouse_down_mode == ModeSliderInlet){
        drag_mode = DragModeConnectDataFromInlet;
      }else if(mouse_down_mode == ModeSliderRelativeOutlet){
        drag_mode = DragModeConnectDataFromRelativeOutlet;
      }else if(mouse_down_mode == ModeSliderAbsoluteOutlet){
        drag_mode = DragModeConnectDataFromAbsoluteOutlet;
      }else if(mouse_down_mode == ModeNone){
        drag_mode = DragModeBBSelect;
      }
      // Slider dragging does not require such a huge distance to start.
    }else if(lmb_down && mouse_down_id >=0 &&
      ( mouse_down_mode == ModeSlider ) ) {
      drag_in_progress = true;
      drag_mode = DragModeSlider;
      //std::cout << "Slider drag." << std::endl;
      module_guis[mouse_down_id]->SliderDragStart(mouse_down_elem_widgetid);
    }
  }

  // standard motion?
  int id1 = InWhich(from), id2 = InWhich(to);
  Point2D offset1, offset2;
  if(id1 != -1) offset1 = from - module_guis[id1]->position;
  if(id2 != -1) offset2 = to   - module_guis[id2]->position;
  if(id1 != id2){
    if(id1 != -1) module_guis[id1]->OnMouseLeave(offset1);
    if(id2 != -1) module_guis[id2]->OnMouseEnter(offset2);
  }else if(id1 == id2 && id1 != -1){
    module_guis[id1]->OnMouseMotion(offset1,offset2);
  }
}

void CanvasView::OnKeyboard(KeyData k){
  if(k.type == KeyData::KeyType::Delete && k.pressed){
    RemoveSelected();
  }
  if(k.type == KeyData::KeyType::Shift) shift_held = k.pressed;
  if(k.type == KeyData::KeyType::Alt) alt_held = k.pressed;
  if(k.type == KeyData::KeyType::Ctrl) ctrl_held = k.pressed;

  // TODO: pass events to children
}

void CanvasView::StopDrag(){
  //std::cout << "Stopping drag" << std::endl;
  drag_in_progress = false;
  potential_wire = PotentialWireMode::None;
}

void CanvasView::RemoveSelected(){
  for(auto p : selection){
    std::shared_ptr<ModuleGUI> mgui = p.first;
    std::shared_ptr<Module> module = mgui->GetModule();
    if(module) canvas->RemoveModule(module);
    module_guis.erase(std::remove(module_guis.begin(), module_guis.end(), mgui), module_guis.end());
  }
  selection.clear();
  StopDrag();
  SetNeedsRedrawing();
}

void CanvasView::FadeoutWireStart(PotentialWireMode m){
  if(potential_wire == PotentialWireMode::None || m == PotentialWireMode::None) return;
  fadeout_wire = m;
  fadeout_anim.Release();
  fadeout_phase = 0.8; // Start at 4/5 opacity
  auto pw = potential_wire_connection; // temp alias
  fadeout_wire_connection = {{module_guis[pw.first .first], pw.first .second},
                             {module_guis[pw.second.first], pw.second.second}};
  fadeout_wire_type = potential_wire_type;
  fadeout_anim = SDLMain::on_before_frame.Subscribe(this, &CanvasView::FadeoutWireStep);
}
void CanvasView::FadeoutWireStep(float delta){
  const float length = (fadeout_wire == PotentialWireMode::New)?0.12f:0.3f; // in seconds
  fadeout_phase -= delta*(1.0f/length);
  if(fadeout_phase <= 0.0){
    fadeout_wire = PotentialWireMode::None;
    fadeout_phase = 0.0;
    fadeout_anim.Release();
  }
  SetNeedsRedrawing();
}

} // namespace AlgAudio
