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
#include "SCLang.hpp"

namespace AlgAudio{
namespace Builtin{

LateReturn<> Subpatch::on_init_latereturn(){
  Relay<> r;
  Sync s(5);
  Canvas::CreateEmpty(canvas.lock()).Then([this,s](auto canvas){
    canvas->owner_hint = shared_from_this();
    internal_canvas = canvas;
    s.Trigger();
  });
  std::cout << "Subpatch initting!" << std::endl;
  inlets.resize(4);
  // TODO : USe global config to run w/o SC
  bool fake = !SCLang::ready;
  Module::Inlet::Create("in1","Inlet 1",shared_from_this(),fake).Then([this,s](auto inlet){
    inlets[0] = inlet;
    if(entrance) entrance->LinkOutput(0, inlet->bus->GetID());
    if(modulegui) modulegui->OnInletsChanged();
    s.Trigger();
  });
  Module::Inlet::Create("in2","Inlet 2",shared_from_this(),fake).Then([this,s](auto inlet){
    inlets[1] = inlet;
    if(entrance) entrance->LinkOutput(1, inlet->bus->GetID());
    if(modulegui) modulegui->OnInletsChanged();
    s.Trigger();
  });
  Module::Inlet::Create("in3","Inlet 3",shared_from_this(),fake).Then([this,s](auto inlet){
    inlets[2] = inlet;
    if(entrance) entrance->LinkOutput(2, inlet->bus->GetID());
    if(modulegui) modulegui->OnInletsChanged();
    s.Trigger();
  });
  Module::Inlet::Create("in4","Inlet 4",shared_from_this(),fake).Then([this,s](auto inlet){
    inlets[3] = inlet;
    if(entrance) entrance->LinkOutput(3, inlet->bus->GetID());
    if(modulegui) modulegui->OnInletsChanged();
    s.Trigger();
  });
  s.WhenAll([r](){r.Return();});
  return r;
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
  std::cout << "Subpatch load XML" << std::endl;
  auto filesave_node = node->first_node("algaudio");
  if(!filesave_node) return; // ??? No save node? Apparently custom save data has no subpatch information, so ignore it.
  
  auto canvasxml = CanvasXML::CreateFromNode(filesave_node);
  
  Canvas::CreateEmpty(canvas.lock()).Then([this,canvasxml](auto c){
    c->owner_hint = shared_from_this();
    internal_canvas = c;
    canvasxml->ApplyToCanvas(c);
  });
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

void Subpatch::LinkToEntrance(std::shared_ptr<SubpatchEntrance> e){  
  entrance = e;
  if(inlets[0] != nullptr) e->LinkOutput(0, inlets[0]->bus->GetID());
  if(inlets[1] != nullptr) e->LinkOutput(1, inlets[1]->bus->GetID());
  if(inlets[2] != nullptr) e->LinkOutput(2, inlets[2]->bus->GetID());
  if(inlets[3] != nullptr) e->LinkOutput(3, inlets[3]->bus->GetID());
}

 int Subpatch::GetGroupID() const {
   return internal_canvas->GetGroup()->GetID();
 }

// ============= SubpatchEntrnace ===========

void SubpatchEntrance::on_init(){
  std::cout << "Subpatch entrance initting!" << std::endl;
  // Check if parent canvas is managed by a subpatch.
  std::shared_ptr<Module> owner = canvas.lock()->owner_hint;
  auto owner_subpatch = std::dynamic_pointer_cast<Subpatch>(owner);
  // If not, throw a DoNotWantToBeCreated exception.
  if(!owner_subpatch) throw ModuleDoesNotWantToBeCreatedException("This module can only be created inside a subpatch.");
  if(owner_subpatch->HasEntrance()) throw ModuleDoesNotWantToBeCreatedException("Currently it is not possible to create multiple entrances inside the same subpatch.");
  // Otherwise link to that entrance.
  owner_subpatch->LinkToEntrance(std::static_pointer_cast<SubpatchEntrance>( shared_from_this() ));
}

void SubpatchEntrance::LinkOutput(int output_no, int busid){
  SCLang::SendOSC("/algaudioSC/setparam", "isi", sc_id, ("subin" + std::to_string(output_no + 1)).c_str(), busid);
}

}} // namespace AlgAudio:::Builtin
