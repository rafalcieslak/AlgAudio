#include "ModuleTemplate.hpp"
#include "rapidxml/rapidxml.hpp"
using namespace rapidxml;

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

  if(!has_class && !has_sc_code) throw ModuleParseException(id, "Module must have either SC code, class name, or both.");

}

} // namespace AlgAudio
