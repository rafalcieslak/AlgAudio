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
#include "UI/StandardModuleGUI.hpp"
#include "ModuleTemplate.hpp"
#include <SDL2/SDL.h>
#include "Theme.hpp"
#include "TextRenderer.hpp"

namespace AlgAudio{

const int StandardModuleGUI::IOConn::width = 16;
const int StandardModuleGUI::IOConn::height = 12;

std::shared_ptr<StandardModuleGUI> StandardModuleGUI::CreateFromXML(std::shared_ptr<Window> w, std::string xml_data, std::shared_ptr<Module> mod){
  auto ptr = std::shared_ptr<StandardModuleGUI>( new StandardModuleGUI(w, mod) );
  ptr->LoadFromXML(xml_data, mod->templ);
  return ptr;
}
std::shared_ptr<StandardModuleGUI> StandardModuleGUI::CreateFromTemplate(std::shared_ptr<Window> w, std::shared_ptr<Module> mod){
  auto ptr = std::shared_ptr<StandardModuleGUI>( new StandardModuleGUI(w, mod) );
  ptr->LoadFromTemplate(mod->templ);
  return ptr;
}

void StandardModuleGUI::CommonInit(){
  main_margin = UIMarginBox::Create(window,0,3,0,3);
  main_box = UIVBox::Create(window);
  inlets_box = UIHBox::Create(window);
  outlets_box = UIHBox::Create(window);
  parrams_box = UIVBox::Create(window);
  parrams_box->SetPadding(1);
  caption = UILabel::Create(window, "", 16);
  caption->SetTextColor("standardbox-caption");

  main_box->Insert(inlets_box, UIBox::PackMode::TIGHT);
  main_box->Insert(caption, UIBox::PackMode::TIGHT);
  main_box->Insert(parrams_box, UIBox::PackMode::TIGHT);
  main_box->Insert(outlets_box, UIBox::PackMode::TIGHT);
  main_margin->Insert(main_box);
  main_margin->parent = shared_from_this();

   inlets_box->SetCustomSize(Size2D(0,4));
  outlets_box->SetCustomSize(Size2D(0,4));

  SetBackColor(Theme::Get("standardbox-bg"));
}

void StandardModuleGUI::LoadFromXML(std::string xml_data, std::shared_ptr<ModuleTemplate> templ){
  CommonInit();
  std::cout << "Building GUI from XML " << std::endl;
  caption->SetText(templ->name);
  UpdateMinimalSize();
}
void StandardModuleGUI::LoadFromTemplate(std::shared_ptr<ModuleTemplate> templ){
  CommonInit();
  std::cout << "Building GUI from template" << std::endl;
  caption->SetText(templ->name);

  for(auto& i : templ->inlets){
    auto inlet = IOConn::Create(window, i, VertAlignment_TOP, Theme::Get("standardbox-inlet"));
    inlets_box->Insert(inlet,UIBox::PackMode::WIDE);
    inlet->on_press.SubscribeForever([this, i](bool b){
      on_inlet_pressed.Happen(i, b);
    });
    inlets[i] = inlet;
  }
  for(auto& o : templ->outlets){
    auto outlet = IOConn::Create(window, o, VertAlignment_BOTTOM, Theme::Get("standardbox-outlet"));
    outlets_box->Insert(outlet,UIBox::PackMode::WIDE);
    outlet->on_press.SubscribeForever([this, o](bool b){
      on_outlet_pressed.Happen(o, b);
    });
    outlets[o] = outlet;
  }
  // Parram sliders.
  for(std::shared_ptr<ParramController>& p : GetModule()->parram_controllers){
    auto slider = UISlider::Create(window, p);
    parrams_box->Insert(slider, UIBox::PackMode::TIGHT);
  }
  UpdateMinimalSize();
}

void StandardModuleGUI::OnChildRequestedSizeChanged(){
  UpdateMinimalSize();
}
void StandardModuleGUI::OnChildVisibilityChanged(){
  UpdateMinimalSize();
}

void StandardModuleGUI::UpdateMinimalSize(){
  SetMinimalSize(main_margin->GetRequestedSize());
}

void StandardModuleGUI::CustomDraw(DrawContext& c){
  // TODO: Store the color instead of getting it every time
  Color border_color = Theme::Get("standardbox-border");

  if(highlight) c.SetColor(border_color.Lighter(0.2));
  else c.SetColor(border_color);
  int w = c.Size().width;
  int h = c.Size().height;
  c.DrawLine(0,0,w-1,0);
  c.DrawLine(0,0,0,h-1);
  c.DrawLine(w-1,0,w-1,h-1);
  c.DrawLine(0,h-1,w-1,h-1);
  c.DrawLine(1,1,w-2,1);
  c.DrawLine(1,1,1,h-2);
  c.DrawLine(w-2,1,w-2,h-2);
  c.DrawLine(1,h-2,w-2,h-2);

  c.Push(Point2D(0,0),c.Size());
  main_margin->Draw(c);
  c.Pop();

  auto m = module.lock();

  if(m){
    std::string idn = std::to_string(m->sc_id);
    //std::cout << "Drawin' " << idn << std::endl;
    if(!id_texture) id_texture = TextRenderer::Render(window, FontParrams("Dosis-Bold",8), idn.c_str());
    c.DrawText(id_texture, Color(0,0,0), 3,2);
  }
}

void StandardModuleGUI::CustomResize(Size2D s){
  main_margin->Resize(s);
  UpdateWhatIsHereCache();
}

void StandardModuleGUI::SetHighlight(bool h){
  highlight = h;

  if(highlight) SetBackColor(Theme::Get("standardbox-bg").Lighter(0.03));
  else  SetBackColor(Theme::Get("standardbox-bg"));

  Color border_color = Theme::Get("standardbox-border");
  if(highlight) border_color = border_color.Lighter(0.2);
  for(auto& i :  inlets) i.second->SetBorderColor(border_color);
  for(auto& o : outlets) o.second->SetBorderColor(border_color);

  SetNeedsRedrawing();
}

std::shared_ptr<StandardModuleGUI::IOConn> StandardModuleGUI::IOConn::Create(std::weak_ptr<Window> w, std::string id, VertAlignment align, Color c){
  return std::shared_ptr<IOConn>( new IOConn(w, id, align, c) );
}

StandardModuleGUI::IOConn::IOConn(std::weak_ptr<Window> w, std::string id_, VertAlignment align_, Color c)
  : UIWidget(w), id(id_), align(align_), main_color(c), border_color(c){
  SetMinimalSize(GetRectSize() + Size2D(2,0));
  on_pointed.SubscribeForever([this](bool){
    SetNeedsRedrawing();
  });
  on_motion.SubscribeForever([this](Point2D pos){
    bool new_inside = pos.IsInside(GetRectPos(),GetRectSize());
    if(inside != new_inside){
      inside = new_inside;
      SetNeedsRedrawing();
    }
  });
};

Point2D StandardModuleGUI::IOConn::GetRectPos() const{
  int x = current_size.width/2 - width/2;
  int y = 0;
  if(align == VertAlignment_TOP) y = 0;
  if(align == VertAlignment_CENTERED) y = current_size.height/2 - height/2;
  if(align == VertAlignment_BOTTOM) y = current_size.height - height;
  return Point2D(x,y);
}

bool StandardModuleGUI::IOConn::CustomMousePress(bool down, short b,Point2D pos){
  if(b == SDL_BUTTON_LEFT && pos.IsInside(GetRectPos(),GetRectSize())){
    on_press.Happen(down);
    return true;
  }
  return false;
}

void StandardModuleGUI::IOConn::CustomDraw(DrawContext& c){
  Point2D p = GetRectPos();
  const int x = p.x, y = p.y;
  if(pointed && inside) c.SetColor(main_color.Lighter(0.2));
  else c.SetColor(main_color);
  c.DrawRect(x,y,width,height);
  c.SetColor(border_color);
  if(align != VertAlignment_TOP) c.DrawLine(x,y,x+width,y);
  c.DrawLine(x,y,x,y+height);
  c.DrawLine(x+width,y,x+width,y+height);
  if(align != VertAlignment_BOTTOM) c.DrawLine(x,y+height-1,x+width,y+height-1);
}
void StandardModuleGUI::IOConn::SetBorderColor(Color c){
  border_color = c;
  SetNeedsRedrawing();
}
Point2D StandardModuleGUI::IOConn::GetCenterPos() const{
  return GetRectPos() + GetRectSize()/2;
}

Point2D StandardModuleGUI::WhereIsInlet(std::string id){
  auto it = inlets.find(id);
  if(it == inlets.end()){
    std::cout << "WARNING: Queried position of an unexisting inlet gui" << std::endl;
    return Point2D(0,0);
  }
  return main_margin->GetChildPos()
       + main_box->GetChildPos(inlets_box)
       + inlets_box->GetChildPos(it->second)
       + it->second->GetCenterPos();
}
Point2D StandardModuleGUI::WhereIsOutlet(std::string id){
  auto it = outlets.find(id);
  if(it == outlets.end()){
    std::cout << "WARNING: Queried position of an unexisting outlet gui" << std::endl;
    return Point2D(0,0);
  }
  return main_margin->GetChildPos()
       + main_box->GetChildPos(outlets_box)
       + outlets_box->GetChildPos(it->second)
       + it->second->GetCenterPos();
}


std::pair<ModuleGUI::WhatIsHereType, std::string> StandardModuleGUI::WhatIsHere(Point2D p) const{
  for(const auto &it : rect_cache)
    if(p.IsInside(it.first))
      return it.second;
  // Rect not found.
  return std::make_pair(WhatIsHereType::Nothing,"");
}

void StandardModuleGUI::UpdateWhatIsHereCache(){
  std::cout << "Updating rect cache!" << std::endl;
  rect_cache.clear();
  for(const auto &it : inlets){
    Point2D pos = it.second->GetPosInParent(main_margin);
    pos = pos + it.second->GetRectPos();
    Rect r(pos, it.second->GetRectSize());
    rect_cache.push_back({r,{WhatIsHereType::Inlet, it.second->id}});
  }
  for(const auto &it : outlets){
    Point2D pos = it.second->GetPosInParent(main_margin);
    pos = pos + it.second->GetRectPos();
    Rect r(pos, it.second->GetRectSize());
    rect_cache.push_back({r,{WhatIsHereType::Outlet, it.second->id}});
  }
}

} // namespace AlgAudio
