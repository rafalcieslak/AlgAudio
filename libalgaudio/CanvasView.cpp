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
  canvas = Canvas::CreateEmpty();
}

std::shared_ptr<CanvasView> CanvasView::CreateEmpty(std::shared_ptr<Window> parent){
  return std::shared_ptr<CanvasView>( new CanvasView(parent) );
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
      c.DrawLine(from_pos, to_pos, true);
    }
  }
  // Then draw the currently dragged line...
  if(drag_in_progress && drag_mode == DragModeConnectFromOutlet){
    Point2D p = module_guis[mouse_down_id]->position + module_guis[mouse_down_id]->WhereIsOutlet(mouse_down_outletid);
    c.DrawLine(p, drag_position, true);
  }else  if(drag_in_progress && drag_mode == DragModeConnectFromInlet){
    Point2D p = module_guis[mouse_down_id]->position + module_guis[mouse_down_id]->WhereIsInlet(mouse_down_inletid);
    c.DrawLine(p, drag_position, true);
  }
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
      mouse_down_inletid = mouse_down_outletid = "";
      // This subscription will be deleted just when this scope ends.
      Subscription temp_sub1 = module_guis[id]-> on_inlet_pressed.Subscribe([this](std::string i, bool ){  mouse_down_inletid = i; });
      Subscription temp_sub2 = module_guis[id]->on_outlet_pressed.Subscribe([this](std::string o, bool ){ mouse_down_outletid = o; });
      bool captured = module_guis[id]->OnMousePress(true, SDL_BUTTON_LEFT, offset);
      if(!captured) mouse_down_mode = ModeModuleBody;
      else if( mouse_down_inletid != "") mouse_down_mode =  ModeInlet;
      else if(mouse_down_outletid != "") mouse_down_mode = ModeOutlet;
      else mouse_down_mode = ModeCaptured;

      if(mouse_down_mode == ModeModuleBody) Select(id);
    }
  }
  if(down == false && b == SDL_BUTTON_LEFT){
    // Mouse button down
    mouse_down = false;

    if(id >=0 ){
      // Mouse button released on some module

      // This subscription will be deleted just when this scope ends.
      std::string outlet, inlet;
      Subscription temp_sub1 = module_guis[id]-> on_inlet_pressed.Subscribe( [&inlet](std::string i, bool ){  inlet = i; });
      Subscription temp_sub2 = module_guis[id]->on_outlet_pressed.Subscribe([&outlet](std::string o, bool ){ outlet = o; });
      bool captured = module_guis[id]->OnMousePress(false, SDL_BUTTON_LEFT, offset);

      if(drag_in_progress){
        drag_in_progress = false;
        if(drag_mode == DragModeConnectFromInlet && captured && outlet != ""){
            FinalizeConnectingDrag(mouse_down_id, mouse_down_inletid, id, outlet);
        }else if(drag_mode == DragModeConnectFromOutlet && captured && inlet != ""){
            FinalizeConnectingDrag(id, inlet, mouse_down_id, mouse_down_outletid);
        }
      }

    }else{
      // Mouse button released on an empty space
      if(drag_in_progress){
        drag_in_progress = false;
        // Redrawing to clear the drag-wire.
        SetNeedsRedrawing();
      }
    }
  }
  return true;
}

void CanvasView::FinalizeConnectingDrag(int inlet_module_id, std::string inlet_id, int outlet_module_id, std::string outlet_id){
  std::cout << "Finalizing drag from " << inlet_module_id << "/" << inlet_id << " to " << outlet_module_id << "/" << outlet_id << std::endl;
  std::shared_ptr<Module> from = module_guis[outlet_module_id]->module.lock();
  std::shared_ptr<Module> to   =  module_guis[inlet_module_id]->module.lock();
  if(!from || !to){
    window.lock()->ShowErrorAlert("Failed to create connection, one of the corresponding modules does not exist." , "Cancel");
    return;
  }
  canvas->Connect(Canvas::IOID{from,outlet_id},Canvas::IOID{to,inlet_id});
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
  if(drag_in_progress) drag_in_progress = false;
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
    SetNeedsRedrawing();
  }else if(drag_in_progress && drag_mode == DragModeConnectFromOutlet){
    drag_position = to;
    SetNeedsRedrawing();
  }else if(mouse_down && mouse_down_id >=0 && Point2D::Distance(mouse_down_position, to) > 5){
    drag_in_progress = true;
    drag_offset = mouse_down_offset;
    dragged_id = mouse_down_id;
    drag_position = to;
    if(mouse_down_mode == ModeModuleBody){
      drag_mode = DragModeMove;
    }else if(mouse_down_mode == ModeInlet){
      drag_mode = DragModeConnectFromInlet;
      drag_connection_io_start = mouse_down_inletid;
      std::cout << "Starting connect-drag from inlet " << drag_connection_io_start << std::endl;
    }else if(mouse_down_mode == ModeOutlet){
      drag_mode = DragModeConnectFromOutlet;
      drag_connection_io_start = mouse_down_outletid;
      std::cout << "Starting connect-drag from outlet " << drag_connection_io_start << std::endl;
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

void CanvasView::RemoveSelected(){
  if(selected_id < 0) return;
  auto m = module_guis[selected_id]->module.lock();
  canvas->RemoveModule(m);
  module_guis.erase(module_guis.begin() + selected_id);
  selected_id = -1; // Warning: Do not use Select() here, because the selected module just stopped existing!
  drag_in_progress = false;
  SetNeedsRedrawing();
}

} // namespace AlgAudio
