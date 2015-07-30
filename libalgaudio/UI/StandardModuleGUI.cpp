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
#include "Theme.hpp"

namespace AlgAudio{

std::shared_ptr<StandardModuleGUI> StandardModuleGUI::CreateEmpty(std::shared_ptr<Window> w){
  return std::shared_ptr<StandardModuleGUI>( new StandardModuleGUI(w) );
}
std::shared_ptr<StandardModuleGUI> StandardModuleGUI::CreateFromXML(std::shared_ptr<Window> w, std::string xml_data, std::shared_ptr<ModuleTemplate> templ){
  auto ptr = std::shared_ptr<StandardModuleGUI>( new StandardModuleGUI(w) );
  ptr->LoadFromXML(xml_data, templ);
  return ptr;
}
std::shared_ptr<StandardModuleGUI> StandardModuleGUI::CreateFromTemplate(std::shared_ptr<Window> w, std::shared_ptr<ModuleTemplate> templ){
  auto ptr = std::shared_ptr<StandardModuleGUI>( new StandardModuleGUI(w) );
  ptr->LoadFromTemplate( templ);
  return ptr;
}

void StandardModuleGUI::LoadFromXML(std::string xml_data, std::shared_ptr<ModuleTemplate> templ){
  std::cout << "Building GUI from XML " << std::endl;
  caption = UILabel::Create(window, templ->name, 16);
  caption->SetTextColor("standardbox-caption");
  caption->parent = shared_from_this();
  SetMinimalSize(caption->GetRequestedSize() + Size2D(16,16));
}
void StandardModuleGUI::LoadFromTemplate(std::shared_ptr<ModuleTemplate> templ){
  std::cout << "Building GUI from template" << std::endl;
  caption = UILabel::Create(window, templ->name, 16);
  caption->SetTextColor("standardbox-caption");
  caption->parent = shared_from_this();
  SetMinimalSize(caption->GetRequestedSize() + Size2D(16,16));
}

void StandardModuleGUI::CustomDraw(DrawContext& c){
  // TODO: Store the color instead of getting it every time
  Color bg_color = Theme::Get("standardbox-bg");
  Color border_color = Theme::Get("standardbox-border");

  if(highlight) c.SetColor(bg_color.Lighter(0.03));
  else c.SetColor(bg_color);
  int w = c.Size().width;
  int h = c.Size().height;
  c.DrawRect(0,0,w,h);

  if(highlight) c.SetColor(border_color.Lighter(0.2));
  else c.SetColor(border_color);
  c.DrawLine(0,0,w-1,0);
  c.DrawLine(0,0,0,h-1);
  c.DrawLine(w-1,0,w-1,h-1);
  c.DrawLine(0,h-1,w-1,h-1);
  c.DrawLine(1,1,w-2,1);
  c.DrawLine(1,1,1,h-2);
  c.DrawLine(w-2,1,w-2,h-2);
  c.DrawLine(1,h-2,w-2,h-2);

  c.Push(Point2D(8,8),Size2D(c.Size().width-16, caption->GetRequestedSize().height));
  caption->Draw(c);
  c.Pop();
}

void StandardModuleGUI::CustomResize(Size2D s){
  caption->Resize(Size2D(s.width-16, caption->GetRequestedSize().height));
}

void StandardModuleGUI::SetHighlight(bool h){
  highlight = h;
  SetNeedsRedrawing();
}

}
