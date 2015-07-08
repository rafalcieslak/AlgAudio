#include <vector>
#include <iterator>
#include <iostream>
#ifdef __unix__
  #include <glob.h>
#else
  #include <windows.h>
#endif

#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"

#include "ModuleCollection.hpp"
#include "LibLoader.hpp"

using namespace rapidxml;

namespace AlgAudio{

std::map<std::string, std::shared_ptr<ModuleCollection>>
  ModuleCollectionBase::collections_by_id;

ModuleCollection::ModuleCollection(std::ifstream& file, std::string b) :
  basedir(b)
{
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
      defaultlib_path = "";
    }else{
      has_defaultlib = true;
      xml_attribute<>* libfile_node = defaultlib_node->first_attribute("file");
      if(!libfile_node) throw CollectionParseException(id, "Missing file attribute in defaultlib node");
      defaultlib_path = libfile_node->value();
    }

    for(xml_node<>* module_node = root->first_node("module"); module_node; module_node = module_node->next_sibling("module")){
      try{
        auto templ = std::make_shared<ModuleTemplate>(*this,module_node);
        if(templates_by_id.find(templ->id) != templates_by_id.end())
          throw CollectionParseException(id, "Collection has duplicate module ids: '" + templ->id + "'");
        templates_by_id[templ->id] = templ;
      }catch(ModuleParseException ex){
        std::cerr << "Exception: " + ex.what() << std::endl;
        std::cerr << "An invalid module in collection " + id + ", ignoring." << std::endl;
      }
    }

  }catch(rapidxml::parse_error ex){
    throw CollectionParseException(std::string("XML parse error: ") + ex.what());
  }catch(std::runtime_error ex){
    throw CollectionParseException(std::string("XML file error: ") + ex.what());
  }

  if(has_defaultlib){
    defaultlib = LibLoader::GetByPath(basedir + Utilities::OSDirSeparator + defaultlib_path + Utilities::OSLibSuffix);
  }
}

std::shared_ptr<ModuleTemplate> ModuleCollection::GetTemplateByID(std::string id){
  auto it = templates_by_id.find(id);
  if(it == templates_by_id.end()) return nullptr;
  else return it->second;
}

std::shared_ptr<ModuleCollection> ModuleCollectionBase::GetCollectionByID(std::string id){
  auto it = collections_by_id.find(id);
  if(it == collections_by_id.end()) return nullptr;
  else return it->second;
}

std::shared_ptr<ModuleCollection> ModuleCollectionBase::InstallFile(std::string filepath){
  std::cout << "Loading collection from file '" << filepath << "'..." << std::endl;
  std::ifstream file(filepath);
  if(!file)
    throw CollectionLoadingException(filepath,"File does not exist or is not readable");
  try{
    std::string directory = Utilities::ConvertUnipathToOSPath(filepath);
    directory = Utilities::GetDir(directory);
    auto collection = std::make_shared<ModuleCollection>(file, directory);
    if(collections_by_id.find(collection->id) != collections_by_id.end())
      throw CollectionLoadingException(filepath, "The collection has a duplicate id");
    collections_by_id[collection->id] = collection;
    return collection;
  }catch(CollectionParseException ex){
    throw CollectionLoadingException(filepath, "Collection file parsing failed: " + ex.what());
  }
}

void ModuleCollectionBase::InstallDir(std::string dirpath){
  std::cout << "Loading all collections from '" << dirpath << "' directory..." << std::endl;
#ifdef __unix__
 glob_t glob_result;
 std::string glob_pattern = dirpath + "/*.xml";
 glob(glob_pattern.c_str(),0 , NULL, &glob_result);
 for(unsigned int i=0; i<glob_result.gl_pathc;i++){
   InstallFile(glob_result.gl_pathv[i]);
 }
#else
  WIN32_FIND_DATA fileData;
  std::string glob = dirpath + "/*.xml";
  HANDLE hFind = FindFirstFile(glob.c_str(), &fileData);
  do{
    InstallFile( dirpath + "/" + fileData.cFileName);
  }while(FindNextFile(hFind, &fileData) != 0);
#endif
}

std::string ModuleCollectionBase::ListInstalledTemplates(){
  std::string result = "Installed module templates list:\n";
  for(const auto& it1 : collections_by_id){
    result += it1.first + " \t :" + it1.second->name + "\n";
    if(it1.second->templates_by_id.empty()) result += "  (collection is empty)\n";
    for(const auto& it2 : it1.second->templates_by_id){
      result += "  " + it2.first + " \t |" + it2.second->name + "\n";
    }
  }
  return result;
}

} // namespace AlgAudio
