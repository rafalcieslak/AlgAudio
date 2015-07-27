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
  drawerlvl1 = UIAnimDrawer::Create(window, Direction_LEFT);
  drawerlvl2 = UIAnimDrawer::Create(window, Direction_LEFT);
  Insert(drawerlvl1, PackMode::TIGHT);
  Insert(drawerlvl2, PackMode::TIGHT);
  listlvl1 = UIList::Create(window);
  listlvl2 = UIList::Create(window);
  drawerlvl1->Insert(listlvl1);
  drawerlvl2->Insert(listlvl2);
  description_label = UILabel::Create(window,"Eventually, this label will contain useful text\nabout pointed module, including the description.\n\nThis is currently just a placeholder.");
  description_label->SetVisible(false);
  description_label->SetBackColor(Color(0,0,0,150));
  description_label->SetAlignment(HorizAlignment_LEFT, VertAlignment_TOP);
  Insert(description_label, PackMode::WIDE);
  listlvl1->SetBackColor(Color(0,0,0,150));
  listlvl2->SetBackColor(Color(0,0,0,150));

  subscriptions += listlvl1->on_clicked.Subscribe([=](std::string id){
    if(lvl1_selection != id){
      lvl1_selection = id;
      lvl2_anim_end_wait = drawerlvl2->on_hide_complete.Subscribe([=](){
        PopulateLvl2();
        drawerlvl2->StartShow(0.15);
      });
      drawerlvl2->StartHide(0.15);
    }else{
      // Same id. Hide the lvl2 drawer
      lvl2_anim_end_wait->Release();
      drawerlvl2->StartHide(0.15);
      lvl1_selection = "";
    }
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
  lvl1_selection = "";
  SetVisible(true);
  lvl1_anim_end_wait = drawerlvl1->on_show_complete.Subscribe([=](){
  });
    description_label->SetVisible(true);
  drawerlvl1->StartShow(0.15);
}
void ModuleSelector::Hide(){
  description_label->SetVisible(false);
  lvl2_anim_end_wait = drawerlvl2->on_hide_complete.Subscribe([=](){
    lvl1_anim_end_wait = drawerlvl1->on_hide_complete.Subscribe([=](){
      SetVisible(false);
    });
    drawerlvl1->StartHide(0.08);
  });
  drawerlvl2->StartHide(0.08);
}


} // namespace AlgAudio
