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
#include "Config.hpp"

namespace AlgAudio{
namespace Builtin{

LateReturn<> Subpatch::on_init_latereturn(){
  Relay<> r;
  Sync s(5);
  auto parent = canvas.lock();
  
  inlets.resize(4);
  
  bool fake = Config::Global().do_not_use_sc;
  Module::Inlet::Create("in1","Inlet 1",shared_from_this(),fake).Then([this,s](auto inlet){
    inlets[0] = inlet;
    if(entrance) entrance->LinkOutput(0, inlet->bus->GetID());
    s.Trigger();
  });
  Module::Inlet::Create("in2","Inlet 2",shared_from_this(),fake).Then([this,s](auto inlet){
    inlets[1] = inlet;
    if(entrance) entrance->LinkOutput(1, inlet->bus->GetID());
    s.Trigger();
  });
  Module::Inlet::Create("in3","Inlet 3",shared_from_this(),fake).Then([this,s](auto inlet){
    inlets[2] = inlet;
    if(entrance) entrance->LinkOutput(2, inlet->bus->GetID());
    s.Trigger();
  });
  Module::Inlet::Create("in4","Inlet 4",shared_from_this(),fake).Then([this,s](auto inlet){
    inlets[3] = inlet;
    if(entrance) entrance->LinkOutput(3, inlet->bus->GetID());
    s.Trigger();
  });
  s.WhenAll([r, this](){
    if(modulegui) modulegui->OnInletsChanged();
    r.Return();
  });
  
  Canvas::CreateEmpty(parent).Then([this,s](auto c){
    c->owner_hint = this->shared_from_this();
    internal_canvas = c;
    Sync s2(2);
    c->CreateModule("builtin/subentry").Then([this,s2](auto module){
      module->position_in_canvas = Point2D(0,-300);
      s2.Trigger();
    });
    c->CreateModule("builtin/subexit").Then([this,s2](auto module){
      module->position_in_canvas = Point2D(0,300);
      s2.Trigger();
    });
    s2.WhenAll([s](){s.Trigger();});
  });
  
  return r;
}

void Subpatch::on_destroy(){
  
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
  
  auto parent = canvas.lock();
  Canvas::CreateEmpty(parent).Then([this,canvasxml,parent](auto c){
    c->owner_hint = this->shared_from_this();
    internal_canvas = c;
    canvasxml->ApplyToCanvas(c);
    // After the canvas was substituted, force recalculate order to use the new group id.
    parent->RecalculateOrder();
  });
}

void Subpatch::on_gui_build(std::shared_ptr<ModuleGUI> gui){
  auto paramsbox = std::dynamic_pointer_cast<UIVBox>( gui->FindChild(UIWidget::ID("paramsbox")) );
  auto button = UIButton::Create(gui->GetWindow(), "Edit...");
  paramsbox->Insert(button, UIBox::PackMode::TIGHT);
  
  subscriptions += button->on_clicked.Subscribe([this](){
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

void Subpatch::LinkToExit(std::shared_ptr<SubpatchExit> e){  
  exit = e;
}

 int Subpatch::GetGroupID() const {
   return internal_canvas->GetGroup()->GetID();
 }
 
 void Subpatch::LinkOutput(int output_no, int busid){
   SCLang::SendOSC("/algaudioSC/setparam", "isi", sc_id, ("subout" + std::to_string(output_no + 1)).c_str(), busid);
 }

// ============= SubpatchEntrance ===========

void SubpatchEntrance::on_init(){
  // Check if parent canvas is managed by a subpatch.
  std::shared_ptr<Module> owner = canvas.lock()->owner_hint;
  auto owner_subpatch = std::dynamic_pointer_cast<Subpatch>(owner);
  // If not, throw a DoNotWantToBeCreated exception.
  if(!owner_subpatch) throw Exceptions::ModuleDoesNotWantToBeCreated("This module can only be created inside a subpatch.");
  if(owner_subpatch->HasEntrance()) throw Exceptions::ModuleDoesNotWantToBeCreated("Currently it is not possible to create multiple entrances inside the same subpatch.");
  // Otherwise link to that entrance.
  owner_subpatch->LinkToEntrance(std::static_pointer_cast<SubpatchEntrance>( shared_from_this() ));
}

void SubpatchEntrance::LinkOutput(int output_no, int busid){
  SCLang::SendOSC("/algaudioSC/setparam", "isi", sc_id, ("subin" + std::to_string(output_no + 1)).c_str(), busid);
}

// ============= SubpatchExit ===========

LateReturn<> SubpatchExit::on_init_latereturn(){
  
  Relay<> r;
  Sync s(4);
  
  // Check if parent canvas is managed by a subpatch.
  std::shared_ptr<Module> owner = canvas.lock()->owner_hint;
  auto owner_subpatch = std::dynamic_pointer_cast<Subpatch>(owner);
  // If not, throw a DoNotWantToBeCreated exception.
  if(!owner_subpatch) {r.LateThrow<Exceptions::ModuleDoesNotWantToBeCreated>("This module can only be created inside a subpatch."); return r;}
  if(owner_subpatch->HasExit()) {r.LateThrow<Exceptions::ModuleDoesNotWantToBeCreated>("Currently it is not possible to create multiple exits inside the same subpatch."); return r;}
  // Otherwise link to that entrance.
  owner_subpatch->LinkToExit(std::static_pointer_cast<SubpatchExit>( shared_from_this() ));
  subpatch = owner_subpatch;
  
  // Create inlets.
  inlets.resize(4);
  
  bool fake = Config::Global().do_not_use_sc;
  
  Module::Inlet::Create("in1","Inlet 1",shared_from_this(),fake).Then([this,s](auto inlet){
    inlets[0] = inlet;
    subpatch->LinkOutput(0, inlet->bus->GetID());
    s.Trigger();
  });
  Module::Inlet::Create("in2","Inlet 2",shared_from_this(),fake).Then([this,s](auto inlet){
    inlets[1] = inlet;
    subpatch->LinkOutput(1, inlet->bus->GetID());
    s.Trigger();
  });
  Module::Inlet::Create("in3","Inlet 3",shared_from_this(),fake).Then([this,s](auto inlet){
    inlets[2] = inlet;
    subpatch->LinkOutput(2, inlet->bus->GetID());
    s.Trigger();
  });
  Module::Inlet::Create("in4","Inlet 4",shared_from_this(),fake).Then([this,s](auto inlet){
    inlets[3] = inlet;
    subpatch->LinkOutput(3, inlet->bus->GetID());
    s.Trigger();
  });
  s.WhenAll([r, this](){  
    if(modulegui) modulegui->OnInletsChanged();
    r.Return();
  });
  
  return r;
}


}} // namespace AlgAudio:::Builtin
