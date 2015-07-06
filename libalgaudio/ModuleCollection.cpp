#include <vector>
#include <iterator>
#include <iostream>

#include "rapidxml/rapidxml.hpp"

#include "ModuleCollection.hpp"

using namespace rapidxml;

namespace AlgAudio{

ModuleCollection::ModuleCollection(std::ifstream& file){
  xml_document<> document;
  xml_node<>* root = nullptr;
  std::vector<char> file_buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  file_buffer.push_back('\0');
  if(file_buffer.size() < 10) throw CollectionParseException("The collection file is apparently too short");
  try{
    document.parse<0>(&(file_buffer[0]));
    root = document.first_node("collection");
    if(!root) throw CollectionParseException("Missing `collection` node");
    xml_attribute<>* version_attr = root->first_attribute("version");
    if(!version_attr) throw CollectionParseException("Missing version information");
    std::string version(version_attr->value());

    // Version check!
    if(version != "1") throw CollectionParseException("Invalid version");

    // Assuming version 1
    xml_attribute<>* id_attr = root->first_attribute("id");
    if(!id_attr) throw CollectionParseException("Missing collection id");
    id = id_attr->value();
    if(id == "") throw CollectionParseException("Collection id is empty");
    // TODO: Testing for collection id uniqueness

    xml_node<>* name_node = root->first_node("name");
    if(!name_node) throw CollectionParseException("Mising collection name");
    name = name_node->value();
    if(name == "") throw CollectionParseException("Collection name is empty");

    xml_node<>* defaultlib_node = root->first_node("defaultlib");
    if(!defaultlib_node){
      has_defaultlib = false;
      defaultlib = "";
    }else{
      has_defaultlib = true;
      defaultlib = defaultlib_node->value();
    }

    for(xml_node<>* module_node = root->first_node("module"); module_node; module_node = module_node->next_sibling("module")){
      try{
        templates.push_back(std::make_shared<ModuleTemplate>(*this,module_node));
      }catch(ModuleParseException ex){
        std::cerr << "An invalid module in collection, ignoring." << std::endl;
      }
    }

  }catch(rapidxml::parse_error ex){
    throw CollectionParseException(std::string("XML parse error: ") + ex.what());
  }
}



}
