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
#include <SDL2/SDL.h>
#include "Window.hpp"

namespace AlgAudio{

CanvasView::CanvasView(std::shared_ptr<Window> parent) : UIWidget(parent){
}

std::shared_ptr<CanvasView> CanvasView::CreateEmpty(std::shared_ptr<Window> parent){
  auto ptr = std::shared_ptr<CanvasView>( new CanvasView(parent) );
  ptr->canvas = Canvas::CreateEmpty();
  return ptr;
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
      int id = module_guis.size()-1;
      Select(id); // Select the just-added module
      dragged_id = id;
      drag_in_progress = true;
      drag_mode = DragModeMove;
      drag_offset = (guisize/2).ToPoint();
      SetNeedsRedrawing();
      r.Return();
    }catch(GUIBuildException ex){
      // TODO: Module removing
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
  // Then draw all the connections...
  // TODO: Connection ending offsets should be cached. Asking each module gui
  // about the io position every time when redrawing is not going to be
  // efficient when there are 100+ modules present.
  c.SetColor(Theme::Get("canvas-connection"));
  for(auto it : canvas->connections){
    Canvas::IOID from = it.first;
    Point2D from_pos = from.module->GetGUI()->position + from.module->GetGUI()->WhereIsOutlet(from.iolet);
    // For each target of this outlet
    std::list<Canvas::IOID> to_list = it.second;
    for(auto to : to_list){
      Point2D to_pos = to.module->GetGUI()->position + to.module->GetGUI()->WhereIsInlet(to.iolet);
      int strength = CurveStrengthFunc(from_pos, to_pos);
      c.DrawCubicBezier(from_pos, from_pos + Point2D(0,strength), to_pos + Point2D(0, -strength), to_pos);
    }
  }
  // Then draw the potential new wire.
  if(drag_in_progress && potential_wire != PotentialWireMode::None){
    int from_id = potential_wire_connection.first.first, to_id = potential_wire_connection.second.first;
    std::string from_outletid = potential_wire_connection.first.second, to_inletid = potential_wire_connection.second.second;

    if(canvas->GetDirectConnectionExists(Canvas::IOID{module_guis[from_id]->GetModule(), from_outletid},
                                         Canvas::IOID{module_guis[  to_id]->GetModule(),    to_inletid}  ))
      c.SetColor(Theme::Get("canvas-connection-remove"));
    else
      c.SetColor(Theme::Get("canvas-connection-new"));

    Point2D p1 = module_guis[from_id]->position + module_guis[from_id]->WhereIsOutlet(from_outletid);
    Point2D p2 = module_guis[  to_id]->position + module_guis[  to_id]->WhereIsInlet (   to_inletid);
    int strength = CurveStrengthFunc(p1, p2);
    c.DrawCubicBezier(p1, p1 + Point2D(0, strength), p2 + Point2D(0, -strength), p2);
  }else if(drag_in_progress){
    // This is a normal drag in progress.
    // Simply draw the currently dragged line...
    if(drag_mode == DragModeConnectFromOutlet){
      Point2D p = module_guis[mouse_down_id]->position + module_guis[mouse_down_id]->WhereIsOutlet(mouse_down_outletid);
      int strength = CurveStrengthFunc(p, drag_position);
      c.DrawCubicBezier(p, p + Point2D(0,strength), drag_position + Point2D(0, -strength/2), drag_position);
    }else if(drag_mode == DragModeConnectFromInlet){
      Point2D p = module_guis[mouse_down_id]->position + module_guis[mouse_down_id]->WhereIsInlet(mouse_down_inletid);
      int strength = CurveStrengthFunc(p, drag_position);
      c.DrawCubicBezier(p, p + Point2D(0,-strength), drag_position + Point2D(0, strength/2), drag_position);
    }
  }
}

int CanvasView::CurveStrengthFunc(Point2D a, Point2D b){
  return std::min(std::abs(a.x - b.x) + std::abs(a.y - b.y)/3, 90);
}

int CanvasView::InWhich(Point2D p){
  for(int i = module_guis.size() - 1; i >= 0; i--){
    if(p.IsInside(module_guis[i]->position, module_guis[i]->GetRequestedSize()) )
      return i;
  }
  return -1;
}

bool CanvasView::CustomMousePress(bool down,short b,Point2D pos){
  int id = InWhich(pos);
  Point2D offset;

  if(id >=0 ){
    offset = pos - module_guis[id]->position;
  }
  if(down == true && b == SDL_BUTTON_LEFT){

    // When a module was just added, it is dragged with LMB up. The drag is
    // stopped by clicking. We want to ignore that click, so generally let's skip
    // all mouse down events that happen during a drag.
    if(drag_in_progress) return true;

    // Mouse button down
    mouse_down = true;
    mouse_down_position = pos;
    mouse_down_id = id;
    mouse_down_mode = ModeNone;
    if(id < 0){
      // Mouse buton down on an empty space
      if(down == true && b == SDL_BUTTON_LEFT){
        Select(-1);
      }
    }else{
      // Mouse button down on some module

      mouse_down_offset = offset;

      auto whatishere = module_guis[id]->WhatIsHere(offset);
      if(whatishere.first == ModuleGUI::WhatIsHereType::Inlet){
        std::cout << "Mouse down on inlet" << std::endl;
        mouse_down_mode =  ModeInlet;
        mouse_down_inletid = whatishere.second;
      }else if(whatishere.first == ModuleGUI::WhatIsHereType::Outlet){
        std::cout << "Mouse down on outlet" << std::endl;
        mouse_down_mode =  ModeOutlet;
        mouse_down_outletid = whatishere.second;
      }else if(whatishere.first == ModuleGUI::WhatIsHereType::SliderBody){
          module_guis[id]->OnMousePress(true, SDL_BUTTON_LEFT, offset);
          //if(captured) mouse_down_mode = ModeCaptured;
          //else{
            mouse_down_mode = ModeSlider; // The slider is not a part of the main module body.
            mouse_down_sliderid = whatishere.second;
          //}

      }else if(whatishere.first == ModuleGUI::WhatIsHereType::Nothing){
        bool captured = module_guis[id]->OnMousePress(true, SDL_BUTTON_LEFT, offset);
        if(!captured) mouse_down_mode = ModeModuleBody;
        else mouse_down_mode = ModeCaptured;
      }else{
        mouse_down_mode = ModeNone;
      }

      if(mouse_down_mode == ModeModuleBody) Select(id);
    }
  }
  if(down == false && b == SDL_BUTTON_LEFT){
    // Mouse button up
    mouse_down = false;

    if(id >=0 ){
      // Mouse button released on some module
      auto whatishere = module_guis[id]->WhatIsHere(offset);
      if(drag_in_progress){
        if(drag_mode == DragModeConnectFromInlet && whatishere.first == ModuleGUI::WhatIsHereType::Outlet){
          FinalizeConnectingDrag(mouse_down_id, mouse_down_inletid, id, whatishere.second);
        }else if(drag_mode == DragModeConnectFromOutlet && whatishere.first == ModuleGUI::WhatIsHereType::Inlet){
          FinalizeConnectingDrag(id, whatishere.second, mouse_down_id, mouse_down_outletid);
        }else{
          // Drag ended on module body.
        }
        StopDrag();
        SetNeedsRedrawing(); // Redraw to remove the dragged wire.
      }else{
        // No drag in progress.
         module_guis[id]->OnMousePress(false, SDL_BUTTON_LEFT, offset);
      }
    }else{
      // Mouse button released on an empty space
      if(drag_in_progress){
        StopDrag();
        // Redrawing to clear the drag-wire.
        SetNeedsRedrawing();
      }
    }
  }
  return true;
}

void CanvasView::FinalizeConnectingDrag(int inlet_module_id, std::string inlet_id, int outlet_module_id, std::string outlet_id){
  std::cout << "Finalizing drag from " << inlet_module_id << "/" << inlet_id << " to " << outlet_module_id << "/" << outlet_id << std::endl;
  std::shared_ptr<Module> from_module = module_guis[outlet_module_id]->GetModule();
  std::shared_ptr<Module> to_module   =  module_guis[inlet_module_id]->GetModule();
  if(!from_module || !to_module){
    window.lock()->ShowErrorAlert("Failed to create connection, one of the corresponding modules does not exist." , "Cancel");
    return;
  }
  Canvas::IOID from = {from_module,outlet_id};
  Canvas::IOID   to = {  to_module,inlet_id };
  if(!canvas->GetDirectConnectionExists(from, to)){
    // There is no such connection, connect!
    try{
      canvas->Connect(from,to);
    }catch(MultipleConnectionsException){
      window.lock()->ShowErrorAlert("Multiple connections from a single outlet are not yet implemented.", "Cancel connection");
    }catch(ConnectionLoopException){
      window.lock()->ShowErrorAlert("Cannot add the selected connection, it would create a loop.", "Cancel connection");
    }catch(DoubleConnectionException){
      window.lock()->ShowErrorAlert("The selected connection already exists!", "Cancel connection");
    }
  }else{
    // This connection already exists, remove it.
    canvas->Disconnect(from,to);
  }
  SetNeedsRedrawing();
}

void CanvasView::Select(int id){
  if(selected_id != -1){
    module_guis[selected_id]->SetHighlight(false);
  }
  selected_id = id;
  if(selected_id != -1){
    module_guis[selected_id]->SetHighlight(true);
  }
}

void CanvasView::CustomMouseEnter(Point2D pos){
  int id = InWhich(pos);
  if(id != -1){
    module_guis[id]->OnMouseEnter(pos);
  }
}
void CanvasView::CustomMouseLeave(Point2D pos){
  if(mouse_down) mouse_down = false;
  if(drag_in_progress) StopDrag();
  int id = InWhich(pos);
  if(id != -1){
    module_guis[id]->OnMouseLeave(pos);
  }
}
void CanvasView::CustomMouseMotion(Point2D from,Point2D to){
  if(drag_in_progress && drag_mode == DragModeMove){
    module_guis[dragged_id]->position = to - drag_offset;
    SetNeedsRedrawing();
  }else if(drag_in_progress && drag_mode == DragModeConnectFromInlet){
    drag_position = to;
    int id = InWhich(to);
    if(id >= 0){
      auto whatishere = module_guis[id]->WhatIsHere(to - module_guis[id]->position);
      if(whatishere.first == ModuleGUI::WhatIsHereType::Outlet){
        potential_wire = PotentialWireMode::New;
        potential_wire_connection = {{id, whatishere.second}, {mouse_down_id, mouse_down_inletid}};
      }else{
        potential_wire = PotentialWireMode::None;
      }
    }else{
      potential_wire = PotentialWireMode::None;
    }
    SetNeedsRedrawing();
  }else if(drag_in_progress && drag_mode == DragModeConnectFromOutlet){
    drag_position = to;
    int id = InWhich(to);
    if(id >= 0){
      auto whatishere = module_guis[id]->WhatIsHere(to - module_guis[id]->position);
      if(whatishere.first == ModuleGUI::WhatIsHereType::Inlet){
        potential_wire = PotentialWireMode::New;
        potential_wire_connection = {{mouse_down_id, mouse_down_outletid}, {id, whatishere.second}};
      }else{
        potential_wire = PotentialWireMode::None;
      }
    }else{
      potential_wire = PotentialWireMode::None;
    }
    SetNeedsRedrawing();
  }else if(mouse_down && mouse_down_id >=0 && Point2D::Distance(mouse_down_position, to) > 5 &&
    ( mouse_down_mode == ModeModuleBody ||
      mouse_down_mode == ModeInlet      ||
      mouse_down_mode == ModeOutlet       ) ) {
    //std::cout << "DRAG start" << std::endl;
    drag_in_progress = true;
    drag_offset = mouse_down_offset;
    dragged_id = mouse_down_id;
    drag_position = to;
    if(mouse_down_mode == ModeModuleBody){
      drag_mode = DragModeMove;
    }else if(mouse_down_mode == ModeInlet){
      drag_mode = DragModeConnectFromInlet;
      drag_connection_io_start = mouse_down_inletid;
    }else if(mouse_down_mode == ModeOutlet){
      drag_mode = DragModeConnectFromOutlet;
      drag_connection_io_start = mouse_down_outletid;
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

void CanvasView::StopDrag(){
  //std::cout << "Stopping drag" << std::endl;
  drag_in_progress = false;
  dragged_id = -1;
  potential_wire = PotentialWireMode::None;
}

void CanvasView::RemoveSelected(){
  if(selected_id < 0) return;
  auto m = module_guis[selected_id]->GetModule();
  if(m) canvas->RemoveModule(m);
  module_guis.erase(module_guis.begin() + selected_id);
  selected_id = -1; // Warning: Do not use Select() here, because the selected module just stopped existing!
  StopDrag();
  SetNeedsRedrawing();
}

} // namespace AlgAudio
