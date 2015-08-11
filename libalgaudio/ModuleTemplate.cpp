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
  xml_node<>* params_node = node->first_node("params");
  if(params_node){
    for(xml_node<>* inlet_node = params_node->first_node("inlet"); inlet_node; inlet_node = inlet_node->next_sibling("inlet")){
      xml_attribute<>* inlet_id = inlet_node->first_attribute("id");
      if(!inlet_id) throw ModuleParseException(id, "An inlet is missing its id");
      inlets.push_back(inlet_id->value());
    }
    for(xml_node<>* outlet_node = params_node->first_node("outlet"); outlet_node; outlet_node = outlet_node->next_sibling("outlet")){
      xml_attribute<>* outlet_id = outlet_node->first_attribute("id");
      if(!outlet_id) throw ModuleParseException(id, "An outlet is missing its id");
      outlets.push_back(outlet_id->value());
    }
    for(xml_node<>* param_node = params_node->first_node("param"); param_node; param_node = param_node->next_sibling("param")){
      auto p = std::make_shared<ParamTemplate>();
      xml_attribute<>* param_id = param_node->first_attribute("id");
      if(!param_id) throw ModuleParseException(id, "An param is missing its id");
      p->id = param_id->value();
      xml_attribute<>* param_name = param_node->first_attribute("name");
      if(!param_name) p->name = "";
      else p->name = param_name->value();

      xml_attribute<>* param_mode = param_node->first_attribute("mode");
      if(!param_mode) p->mode = ParamTemplate::ParamMode::Input;
      else{
        std::string mode = param_mode->value();
        if(mode == "input") p->mode = ParamTemplate::ParamMode::Input;
        else if(mode == "output") p->mode = ParamTemplate::ParamMode::Output;
        else if(mode == "none") p->mode = ParamTemplate::ParamMode::None;
        else throw ModuleParseException(id, "A param has invalid mode value: " + mode);
      }

      xml_attribute<>* param_defaultmax = param_node->first_attribute("defaultmax");
      if(!param_defaultmax) p->default_max = 1.0;
      else p->default_max = std::stof(param_defaultmax->value());
      xml_attribute<>* param_defaultmin = param_node->first_attribute("defaultmin");
      if(!param_defaultmin) p->default_min = 0.0;
      else p->default_min = std::stof(param_defaultmin->value());
      xml_attribute<>* param_defaultval = param_node->first_attribute("defaultval");
      if(!param_defaultval) p->default_val = 1.0;
      else p->default_val = std::stof(param_defaultval->value());

      xml_attribute<>* action = param_node->first_attribute("action");
      if(!action) p->action = ParamTemplate::ParamAction::SC;
      else{
        std::string val(action->value());
        if(val == "sc")     p->action = ParamTemplate::ParamAction::SC;
        else if(val == "custom") p->action = ParamTemplate::ParamAction::Custom;
        else if(val == "none") p->action = ParamTemplate::ParamAction::None;
        else throw ModuleParseException(id, "Action attribute has an invalid value: " + val);
      }


      params.push_back(p);
    }
    for(xml_node<>* reply_node = params_node->first_node("reply"); reply_node; reply_node = reply_node->next_sibling("reply")){
      xml_attribute<>* reply_id = reply_node->first_attribute("id");
      if(!reply_id) throw ModuleParseException(id, "A reply is missing its id attribute");
      xml_attribute<>* reply_param = reply_node->first_attribute("param");
      if(!reply_param) throw ModuleParseException(id, "A reply is missing its param attribute");
      std::string id = reply_id->value();
      std::string param = reply_param->value();

      replies.push_back({id,param});
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
