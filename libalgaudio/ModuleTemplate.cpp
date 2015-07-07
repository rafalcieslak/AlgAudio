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
}

} // namespace AlgAudio
