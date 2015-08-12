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
#include "ModuleUI/StandardModuleGUI.hpp"
#include "ModuleTemplate.hpp"
#include <SDL2/SDL.h>
#include "Theme.hpp"
#include "TextRenderer.hpp"
#include "rapidxml/rapidxml.hpp"

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
  params_box = UIVBox::Create(window);
  params_box->SetPadding(1);
  caption = UILabel::Create(window, "", 16);
  caption->SetTextColor("standardbox-caption");

  main_box->Insert(inlets_box, UIBox::PackMode::TIGHT);
  main_box->Insert(caption, UIBox::PackMode::TIGHT);
  main_box->Insert(params_box, UIBox::PackMode::TIGHT);
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

  unsigned int length = xml_data.length();

  char* buffer = new char[length+2];
  strcpy(buffer, xml_data.c_str());
  rapidxml::xml_document<> doc;
  doc.parse<0>(buffer);
  rapidxml::xml_node<>* gui_node = doc.first_node("gui");
  rapidxml::xml_node<>* node = gui_node->first_node();
  for( ; node; node = node->next_sibling()){
    std::string name = node->name();
    if(name == "inlet"){
      std::string id, inlet_id;
      rapidxml::xml_attribute<>* attr_id = node->first_attribute("id");
      if(attr_id) id = attr_id->value();
      else throw GUIBuildException("An inlet is missing its corresponding param id");

      rapidxml::xml_attribute<>* attr_inlet = node->first_attribute("inlet");
      if(attr_inlet) inlet_id = attr_inlet->value();
      else inlet_id = id;

      auto inlet = IOConn::Create(window, inlet_id, VertAlignment_TOP, Theme::Get("standardbox-inlet"));
      inlet->widget_id = UIWidget::ID(id);
      inlets_box->Insert(inlet,UIBox::PackMode::WIDE);
      inlets[inlet->widget_id] = inlet;
    }else if(name == "outlet"){
      std::string id, outlet_id;
      rapidxml::xml_attribute<>* attr_id = node->first_attribute("id");
      if(attr_id) id = attr_id->value();
      else throw GUIBuildException("An outlet is missing its corresponding param id");

      rapidxml::xml_attribute<>* attr_outlet = node->first_attribute("outlet");
      if(attr_outlet) outlet_id = attr_outlet->value();
      else outlet_id = id;

      auto outlet = IOConn::Create(window, id, VertAlignment_BOTTOM, Theme::Get("standardbox-outlet"));
      outlet->widget_id = UIWidget::ID(id);
      outlets_box->Insert(outlet,UIBox::PackMode::WIDE);
      outlets[outlet->widget_id] = outlet;
    }else if(name == "slider" || name == "display"){
      std::string id, param;
      rapidxml::xml_attribute<>* attr_id = node->first_attribute("id");
      if(attr_id) id = attr_id->value();
      else throw GUIBuildException("A slider is missing its id");
      rapidxml::xml_attribute<>* attr_param = node->first_attribute("param");
      if(attr_param) param = attr_param->value();
      else throw GUIBuildException("A slider is missing its corresponding param id");

      auto p = GetModule()->GetParamControllerByID(param);
      if(!p) throw GUIBuildException("A slider has an unexisting param id " + param);
      auto slider = UISlider::Create(window, p);
      slider->widget_id = UIWidget::ID(id);
      slider->param_id = param;
      if(name == "display") slider->SetMode(UISlider::Mode::Display);
      params_box->Insert(slider, UIBox::PackMode::TIGHT);
      param_sliders[slider->widget_id] = slider;

      rapidxml::xml_attribute<>* attr_name = node->first_attribute("name");
      if(attr_name) slider->SetName(attr_name->value());
      slider->SetRangeMin(p->GetRangeMin());
      slider->SetRangeMax(p->GetRangeMax());

    }else if(name == "display"){

    }else {
      throw GUIBuildException("Unrecognized gui element: " + name);
    }
  }

  delete[] buffer;
  UpdateMinimalSize();
}
void StandardModuleGUI::LoadFromTemplate(std::shared_ptr<ModuleTemplate> templ){
  CommonInit();
  std::cout << "Building GUI from template" << std::endl;
  caption->SetText(templ->name);

  for(auto& i : templ->inlets){
    auto inlet = IOConn::Create(window, i, VertAlignment_TOP, Theme::Get("standardbox-inlet"));
    inlet->widget_id = UIWidget::ID("autoinlet_" + i);
    inlets_box->Insert(inlet,UIBox::PackMode::WIDE);
    inlets[inlet->widget_id] = inlet;
  }
  for(auto& o : templ->outlets){
    auto outlet = IOConn::Create(window, o, VertAlignment_BOTTOM, Theme::Get("standardbox-outlet"));
    outlet->widget_id = UIWidget::ID("autooutlet_" + o);
    outlets_box->Insert(outlet,UIBox::PackMode::WIDE);
    outlets[outlet->widget_id] = outlet;
  }
  // Param sliders.
  for(std::shared_ptr<ParamController>& p : GetModule()->param_controllers){
    if(p->templ->mode == ParamTemplate::ParamMode::None) continue;
    auto slider = UISlider::Create(window, p);
    slider->widget_id = UIWidget::ID("autoslider_" + p->id);
    slider->param_id = p->id;
    if(p->templ->mode == ParamTemplate::ParamMode::Output) slider->SetMode(UISlider::Mode::Display);
    params_box->Insert(slider, UIBox::PackMode::TIGHT);
    param_sliders[slider->widget_id] = slider;
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
    if(!id_texture) id_texture = TextRenderer::Render(window, FontParams("Dosis-Bold",8), idn.c_str());
    c.DrawText(id_texture, Theme::Get("standardbox-border"), 3,1);
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
  : UIWidget(w), iolet_id(id_), align(align_), main_color(c), border_color(c){
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
  for(auto &it: inlets){
    if(it.second->iolet_id == id)
      return it.second->GetPosInParent(main_margin) + it.second->GetCenterPos();
  }
  std::cout << "WARNING: Queried position of an unexisting inlet" << std::endl;
  return Point2D(0,0);
}
Point2D StandardModuleGUI::WhereIsOutlet(std::string id){
  for(auto &it: outlets){
    if(it.second->iolet_id == id)
      return it.second->GetPosInParent(main_margin) + it.second->GetCenterPos();
  }
  std::cout << "WARNING: Queried position of an unexisting outlet" << std::endl;
  return Point2D(0,0);
}
Point2D StandardModuleGUI::WhereIsParamInlet(std::string id){
  for(auto &it: param_sliders){
    if(it.second->param_id == id)
      return it.second->GetPosInParent(main_margin) + it.second->GetInputRect().Center();
  }
  std::cout << "WARNING: Queried position of an unexisting param inlet" << std::endl;
  return Point2D(0,0);
}
Point2D StandardModuleGUI::WhereIsParamOutlet(std::string id){
  for(auto &it: param_sliders){
    if(it.second->param_id == id)
      return it.second->GetPosInParent(main_margin) + it.second->GetOutputRect().Center();
  }
  std::cout << "WARNING: Queried position of an unexisting param outlet" << std::endl;
  return Point2D(0,0);
}

std::string StandardModuleGUI::GetIoletParamID(UIWidget::ID id) const{
  auto it = outlets.find(id);
  if(it == outlets.end()){
    it = inlets.find(id);
    if(it == inlets.end()){
      auto it2 = param_sliders.find(id);
      if(it2 == param_sliders.end()){
        std::cout << "WARNING: Queried position of an unexisting outlet gui" << std::endl;
        return "";
      }
      return it2->second->param_id;
    }
    return it->second->iolet_id;
  }
  return it->second->iolet_id;
}


auto StandardModuleGUI::GetWhatIsHere(Point2D p) const -> WhatIsHere{
  for(const auto &it : rect_cache)
    if(p.IsInside(it.first))
      return it.second;
  // Rect not found.
  return WhatIsHere{WhatIsHereType::Nothing,UIWidget::ID(),""};
}

void StandardModuleGUI::UpdateWhatIsHereCache(){
  std::cout << "Updating rect cache!" << std::endl;
  rect_cache.clear();
  for(const auto &it : inlets){
    Point2D pos = it.second->GetPosInParent(main_margin);
    pos = pos + it.second->GetRectPos();
    Rect r(pos, it.second->GetRectSize());
    rect_cache.push_back({r,WhatIsHere{WhatIsHereType::Inlet, it.second->widget_id, it.second->iolet_id}});
  }
  for(const auto &it : outlets){
    Point2D pos = it.second->GetPosInParent(main_margin);
    pos = pos + it.second->GetRectPos();
    Rect r(pos, it.second->GetRectSize());
    rect_cache.push_back({r,WhatIsHere{WhatIsHereType::Outlet, it.second->widget_id, it.second->iolet_id}});
  }
  for(const auto &it : param_sliders){
    Point2D pos = it.second->GetPosInParent(main_margin);
    Rect r;
    r = it.second->GetInputRect().MoveOffset(pos);
    rect_cache.push_back({r,WhatIsHere{WhatIsHereType::SliderInput, it.second->widget_id, it.second->param_id}});
    r = it.second->GetOutputRect().MoveOffset(pos);
    rect_cache.push_back({r,WhatIsHere{WhatIsHereType::SliderOutput, it.second->widget_id, it.second->param_id}});
    r = it.second->GetBodyRect().MoveOffset(pos);
    rect_cache.push_back({r,WhatIsHere{WhatIsHereType::SliderBody, it.second->widget_id, it.second->param_id}});
  }
}


void StandardModuleGUI::SliderDragStart(UIWidget::ID id, Point2D start_pos){
  param_sliders[id]->DragStart(start_pos);
}
void StandardModuleGUI::SliderDragStep(UIWidget::ID id, Point2D current_pos){
  param_sliders[id]->DragStep(current_pos);
}
void StandardModuleGUI::SliderDragEnd(UIWidget::ID id, Point2D final_pos){
  param_sliders[id]->DragEnd(final_pos);
}

} // namespace AlgAudio
