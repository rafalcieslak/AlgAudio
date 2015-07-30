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
}

int CanvasView::InWhich(Point2D p){
  for(int i = module_guis.size() - 1; i >= 0; i--){
    if(p.IsInside(module_guis[i]->position, module_guis[i]->GetRequestedSize()) )
      return i;
  }
  return -1;
}

void CanvasView::CustomMousePress(bool down,short b,Point2D pos){
  if(down == true && b == SDL_BUTTON_LEFT){
    pressed = true;
    press_position = pos;
    press_id = InWhich(pos);
    if(press_id >= 0){
      press_offset = pos - module_guis[press_id]->position;
    }
    Select(press_id);
  }else if(down == false && b == SDL_BUTTON_LEFT && pressed == true){
    pressed = false;
    if(drag_in_progress){
      drag_in_progress = false;
    }
  }
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

void CanvasView::CustomMouseEnter(Point2D){
}
void CanvasView::CustomMouseLeave(Point2D){
  if(pressed) pressed = false;
  if(drag_in_progress) drag_in_progress = false;
}
void CanvasView::CustomMouseMotion(Point2D,Point2D to){
  if(drag_in_progress){
    module_guis[dragged_id]->position = to - drag_offset;
    SetNeedsRedrawing();
  }else{
    if(pressed && press_id >=0 && Point2D::Distance(press_position, to) > 5){
      drag_offset = press_offset;
      drag_in_progress = true;
      dragged_id = press_id;
    }
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
