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
#include "ModuleFactory.hpp"

namespace AlgAudio{

CanvasView::CanvasView(std::shared_ptr<Window> parent) : UIWidget(parent){
  canvas = Canvas::CreateEmpty();
}

std::shared_ptr<CanvasView> CanvasView::CreateEmpty(std::shared_ptr<Window> parent){
  return std::shared_ptr<CanvasView>( new CanvasView(parent) );
}

LateReturn<> CanvasView::AddModule(std::string id, Point2D pos){
  auto r = Relay<>::Create();
  ModuleFactory::CreateNewInstance(id).Then([&](std::shared_ptr<Module> m){
    canvas->InsertModule(m);
    auto modulegui = m->BuildGUI(window.lock(), m->templ->guitype);
    if(!modulegui){
      std::cout << "Failed to build gui" << std::endl;
      r.Return();
      return;
    }
    modulegui->position = pos;
    module_guis.push_back(modulegui);
    SetNeedsRedrawing();
    r.Return();
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

} // namespace AlgAudio
