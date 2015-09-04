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

#include "BuiltinModules.hpp"
#include "UI/UIButton.hpp"
#include "UI/UIBox.hpp"
#include "ModuleUI/ModuleGUI.hpp"
#include "CanvasView.hpp"
#include "CanvasXML.hpp"

namespace AlgAudio{
namespace Builtin{

void Subpatch::on_init(){
  if(canvas.lock() == nullptr) std::cout << "AAAAAAAA nullptr" << std::endl;
    else std::cout << "BBBBBBB not nullptr" << std::endl;
  LateAssign(internal_canvas, Canvas::CreateEmpty(canvas.lock()));
}

void Subpatch::on_destroy(){
  std::cout << "Subpatch destroy" << std::endl;
}

void Subpatch::state_store_xml(rapidxml::xml_node<char>* node) const {
  auto canvasxml = CanvasXML::CreateFromCanvas(internal_canvas);
  auto filesave_node = node->document()->allocate_node(rapidxml::node_type::node_element,"algaudio");
  node->append_node(filesave_node);
  canvasxml->CloneToAnotherXMLTree(filesave_node, node->document());
}
void Subpatch::state_load_xml(rapidxml::xml_node<char>* node){
  auto filesave_node = node->first_node("algaudio");
  if(!filesave_node) return; // ??? No save node? Apparently custom save data has no subpatch information, so ignore it.
  
  auto canvasxml = CanvasXML::CreateFromNode(filesave_node);
  // TODO: This may be possibly dangerous. If the canvas is not yet assigned before view is switched,
  // the CV may end up trying to display a nullptr canvas.
  LateAssign(internal_canvas, canvasxml->CreateNewCanvas(canvas.lock()));
}

void Subpatch::on_gui_build(std::shared_ptr<ModuleGUI> gui){
  auto paramsbox = std::dynamic_pointer_cast<UIVBox>( gui->FindChild(UIWidget::ID("paramsbox")) );
  auto button = UIButton::Create(gui->GetWindow(), "Edit...");
  paramsbox->Insert(button, UIBox::PackMode::TIGHT);
  
  subscriptions += button->on_clicked.Subscribe([this](){
    std::cout << "Edit clicked." << std::endl;
    auto canvasview = std::dynamic_pointer_cast<CanvasView>(modulegui->parent.lock());
    if(!canvasview){
      std::cout << "Whoops, failed to get a reference to a canvasview, unable to switch displayed canvas." << std::endl;
      return;
    }
    canvasview->EnterCanvas(internal_canvas, "Subpatch");
  });
}

}} // namespace AlgAudio:::Builtin
