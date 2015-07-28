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
#include "ModuleSelector.hpp"
#include "ModuleCollection.hpp"

namespace AlgAudio{

ModuleSelector::ModuleSelector(std::weak_ptr<Window> parent_window) : UIHBox(parent_window){
}
std::shared_ptr<ModuleSelector> ModuleSelector::Create(std::weak_ptr<Window> w){
  auto m = std::shared_ptr<ModuleSelector>( new ModuleSelector(w) );
  m->init();
  return m;
}
void ModuleSelector::init(){
  SetVisible(false);
  drawersbox = UIHBox::Create(window);
  drawersbox->SetPadding(2);
  drawerlvl1 = UIAnimDrawer::Create(window, Direction_LEFT);
  drawerlvl2 = UIAnimDrawer::Create(window, Direction_LEFT);
  drawersbox->Insert(drawerlvl1, PackMode::TIGHT);
  drawersbox->Insert(drawerlvl2, PackMode::TIGHT);
  Insert(drawersbox, PackMode::TIGHT);
  lvl1_box = UIVBox::Create(window);
  lvl2_box = UIVBox::Create(window);
  drawerlvl1->Insert(lvl1_box);
  drawerlvl2->Insert(lvl2_box);
  listlvl1 = UIList::Create(window);
  listlvl2 = UIList::Create(window);
  listlvl1->SetColors(Theme::Get("selector-button-normal"),Theme::Get("selector-button-highlight"));
  listlvl2->SetColors(Theme::Get("selector-button-normal"),Theme::Get("selector-button-highlight"));
  lvl1_box->Insert(listlvl1, PackMode::TIGHT);
  lvl2_box->Insert(listlvl2, PackMode::TIGHT);
  lvl1_separator = UISeparator::Create(window);
  lvl2_separator = UISeparator::Create(window);
  lvl1_box->Insert(lvl1_separator, PackMode::WIDE);
  lvl2_box->Insert(lvl2_separator, PackMode::WIDE);
  description_box = UIMarginBox::Create(window,20,0,0,20);
  description_box->SetVisible(false);
  description_label = UILabel::Create(window,"Eventually, this label will contain useful text\nabout pointed module, including the description.\n\nThis is currently just a placeholder.");
  description_label->SetAlignment(HorizAlignment_LEFT, VertAlignment_TOP);
  description_box->Insert(description_label);
  Insert(description_box, PackMode::WIDE);
  drawersbox->SetBackColor(Color(0,0,0,150));
  description_box->SetBackColor(Color(0,0,0,150));

  subscriptions += listlvl1->on_clicked.Subscribe([this](std::string id){
    if(lvl1_selection != id){
      lvl1_selection = id;
      listlvl1->SetHighlight(id);
      lvl2_anim_end_wait = drawerlvl2->on_hide_complete.Subscribe([this](){
        PopulateLvl2();
        drawerlvl2->StartShow(0.15);
      });
      drawerlvl2->StartHide(0.15);
    }else{
      // Same id. Hide the lvl2 drawer
      lvl2_anim_end_wait.Release();
      drawerlvl2->StartHide(0.15);
      listlvl1->SetHighlight("");
      lvl1_selection = "";
    }
  });

  subscriptions += listlvl2->on_pointed.Subscribe([this](std::string id){
    if(id == ""){
      description_label->SetText("Select a module to add.");
      return;
    }
    id = lvl1_selection + "/" + id;
    auto templ = ModuleCollectionBase::GetTemplateByID(id);
    if(!templ){
      std::cout << "ERROR: pointed at an unregistered module `" + id + "`" << std::endl;
      return;
    }
    if(templ->description == "") description_label->SetText("(this module has no description available)");
    else description_label->SetText( templ->description );
  });
  auto emptycomplete = [this](){ on_complete.Happen("");};
  subscriptions += description_box->on_clicked.Subscribe(emptycomplete);
  subscriptions += lvl1_separator->on_clicked.Subscribe(emptycomplete);
  subscriptions += lvl2_separator->on_clicked.Subscribe(emptycomplete);
  subscriptions += listlvl2->on_clicked.Subscribe([&](std::string id){
    on_complete.Happen(lvl1_selection + "/" + id);
  });
}

void ModuleSelector::PopulateLvl1(){
  listlvl1->Clear();
  auto collection_map = ModuleCollectionBase::GetCollections();
  for(auto& it : collection_map){
    auto c = it.second;
    listlvl1->AddItem(c->id, c->name);
  }
}
void ModuleSelector::PopulateLvl2(){
  auto collectionptr = ModuleCollectionBase::GetCollectionByID(lvl1_selection);
  if(!collectionptr) {
    std::cout << "Error, populating from an unexisting collection" << std::endl;
    return;
  }
  listlvl2->Clear();
  for(auto& it : collectionptr->templates_by_id){
    auto templ = it.second;
    listlvl2->AddItem(templ->id, templ->name);
  }
}


void ModuleSelector::Expose(){
  exposed = true;
  lvl1_selection = "";
  listlvl1->SetHighlight("");
  SetVisible(true);
  lvl1_anim_end_wait = drawerlvl1->on_show_complete.Subscribe([this](){
    description_box->SetVisible(true);
  });
  lvl2_anim_end_wait.Release();
  drawerlvl1->StartShow(0.15);
  drawerlvl2->StartHide(0.0);
}
void ModuleSelector::Hide(){
  exposed = false;
  description_box->SetVisible(false);
  lvl2_anim_end_wait = drawerlvl2->on_hide_complete.Subscribe([this](){
    this->lvl1_anim_end_wait = drawerlvl1->on_hide_complete.Subscribe([this](){
      SetVisible(false);
    });
    drawerlvl1->StartHide(0.08);
  });
  drawerlvl2->StartHide(0.08);
}


} // namespace AlgAudio
