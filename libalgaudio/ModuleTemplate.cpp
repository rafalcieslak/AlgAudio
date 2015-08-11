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
#include "ModuleTemplate.hpp"
#include <sstream>
#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_print.hpp"
using namespace rapidxml;
#include "Module.hpp"
#include "ModuleCollection.hpp"

namespace AlgAudio{

ModuleTemplate::ModuleTemplate(ModuleCollection& c) : collection(c){

}

ModuleTemplate::ModuleTemplate(ModuleCollection& c, xml_node<>* node) : collection(c){
  xml_attribute<>* id_attr = node->first_attribute("id");
  if(!id_attr) throw ModuleParseException("", "Missing module id");
  id = id_attr->value();

  xml_attribute<>* name_attr = node->first_attribute("name");
  if(!name_attr) throw ModuleParseException(id, "Missing module name");
  name = name_attr->value();
  if(name == "") throw ModuleParseException(id, "Module name is empty");

  xml_node<>* sc_node = node->first_node("sc");
  if(sc_node){
    has_sc_code = true;
    sc_code = sc_node->value();
  }

  xml_node<>* class_node = node->first_node("class");

  if(class_node){
    has_class = true;
    xml_attribute<>* class_name_attr = class_node->first_attribute("name");
    if(!class_name_attr) throw ModuleParseException(id, "Module has class node, but the class name is missing");
    class_name = class_name_attr->value();
  }
  xml_node<>* parrams_node = node->first_node("parrams");
  if(parrams_node){
    for(xml_node<>* inlet_node = parrams_node->first_node("inlet"); inlet_node; inlet_node = inlet_node->next_sibling("inlet")){
      xml_attribute<>* inlet_id = inlet_node->first_attribute("id");
      if(!inlet_id) throw ModuleParseException(id, "An inlet is missing its id");
      inlets.push_back(inlet_id->value());
    }
    for(xml_node<>* outlet_node = parrams_node->first_node("outlet"); outlet_node; outlet_node = outlet_node->next_sibling("outlet")){
      xml_attribute<>* outlet_id = outlet_node->first_attribute("id");
      if(!outlet_id) throw ModuleParseException(id, "An outlet is missing its id");
      outlets.push_back(outlet_id->value());
    }
    for(xml_node<>* parram_node = parrams_node->first_node("parram"); parram_node; parram_node = parram_node->next_sibling("parram")){
      auto p = std::make_shared<ParramTemplate>();
      xml_attribute<>* parram_id = parram_node->first_attribute("id");
      if(!parram_id) throw ModuleParseException(id, "An parram is missing its id");
      p->id = parram_id->value();
      xml_attribute<>* parram_name = parram_node->first_attribute("name");
      if(!parram_name) p->name = "";
      else p->name = parram_name->value();
      xml_attribute<>* parram_defaultmax = parram_node->first_attribute("defaultmax");
      if(!parram_defaultmax) p->default_max = 1.0;
      else p->default_max = std::stof(parram_defaultmax->value());
      xml_attribute<>* parram_defaultmin = parram_node->first_attribute("defaultmin");
      if(!parram_defaultmin) p->default_min = 0.0;
      else p->default_min = std::stof(parram_defaultmin->value());
      xml_attribute<>* parram_defaultval = parram_node->first_attribute("defaultval");
      if(!parram_defaultval) p->default_val = 1.0;
      else p->default_val = std::stof(parram_defaultval->value());

      xml_attribute<>* parram_action = parram_node->first_attribute("action");
      if(!parram_action) p->parram_mode = ParramTemplate::ParramMode::SC;
      else{
        std::string val(parram_action->value());
        if(val == "sc")     p->parram_mode = ParramTemplate::ParramMode::SC;
        else if(val == "custom") p->parram_mode = ParramTemplate::ParramMode::Custom;
        else if(val == "none") p->parram_mode = ParramTemplate::ParramMode::None;
        else throw ModuleParseException(id, "Action attribute has an invalid value: " + val);
      }


      parrams.push_back(p);
    }
  }

  xml_node<>* desc_node = node->first_node("description");
  if(desc_node) description = Utilities::TrimAllLines(desc_node->value());

  xml_node<>* gui_node = node->first_node("gui");
  if(gui_node){
    std::stringstream ss;
    ss << *gui_node;
    guitree = std::string(ss.str());
    xml_attribute<>* gui_type_attr = gui_node->first_attribute("type");
    if(gui_type_attr) guitype = gui_type_attr->value();
  }

  if(!has_class && !has_sc_code) throw ModuleParseException(id, "Module must have either SC code, class name, or both.");

}

std::string ModuleTemplate::GetFullID() const{
  return collection.id + "/" + id;
}

} // namespace AlgAudio
