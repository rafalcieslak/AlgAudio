#include <vector>
#include <iterator>
#include <iostream>

#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"

#include "ModuleCollection.hpp"

using namespace rapidxml;

namespace AlgAudio{

std::map<std::string, std::shared_ptr<ModuleCollection>>
  ModuleCollectionBase::collections_by_id;

ModuleCollection::ModuleCollection(std::ifstream& file){
  xml_document<> document;
  xml_node<>* root = nullptr;
  try{
    rapidxml::file<> file_buffer(file);
    if(file_buffer.size() < 10) throw CollectionParseException("The collection file is apparently too short");
    document.parse<0>(file_buffer.data());
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
    if(!name_node) throw CollectionParseException(id, "Mising collection name");
    name = name_node->value();
    if(name == "") throw CollectionParseException(id, "Collection name is empty");

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
        std::cerr << "Exception: " + ex.what();
        std::cerr << "An invalid module in collection " + id + ", ignoring." << std::endl;
      }
    }

  }catch(rapidxml::parse_error ex){
    throw CollectionParseException(std::string("XML parse error: ") + ex.what());
  }catch(std::runtime_error ex){
    throw CollectionParseException(std::string("XML file error: ") + ex.what());
  }
}

std::shared_ptr<ModuleCollection> ModuleCollectionBase::GetByID(std::string id){
  auto it = collections_by_id.find(id);
  if(it == collections_by_id.end()) return nullptr;
  else return it->second;
}

std::shared_ptr<ModuleCollection> ModuleCollectionBase::InstallFile(std::string filepath){
  std::ifstream file(filepath);
  if(!file)
    throw CollectionLoadingException(filepath,"File does not exist or is not readable");
  try{
    auto collection = std::make_shared<ModuleCollection>(file);
    if(collections_by_id.find(collection->id) != collections_by_id.end())
      throw CollectionLoadingException(filepath, "The collection has a duplicate id");
    collections_by_id[collection->id] = collection;
    return collection;
  }catch(CollectionParseException ex){
    throw CollectionLoadingException(filepath, "Collection file parsing failed: " + ex.what());
  }
}

} // namespace AlgAudio
