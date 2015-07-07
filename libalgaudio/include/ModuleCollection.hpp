#ifndef MODULE_CONNECTION
#define MODULE_CONNECTION
#include <fstream>
#include <memory>
#include <map>

#include "ModuleTemplate.hpp"
#include "Utilities.hpp"

namespace AlgAudio{

class LibLoader;

struct CollectionParseException : public Exception{
  CollectionParseException(std::string t) : Exception(t) {};
  CollectionParseException(std::string i, std::string t) : Exception(t), id(i) {};
  virtual std::string what() override {
    if(id == "") return "While loading an unknown collection: " + text;
    else return "While loading collection '" + id + "': " + text;
  };
  std::string id = "";
};
struct CollectionLoadingException : public Exception{
  CollectionLoadingException(std::string p, std::string t) : Exception(t), path(p) {};
  virtual std::string what() override {
    return "While loading collection from '" + path + "': " + text;
  };
  std::string path;
};

class ModuleCollection{
public:
  ModuleCollection(std::ifstream& file, std::string basedir);
  std::shared_ptr<ModuleTemplate> GetTemplateByID(std::string id);
  std::map<std::string, std::shared_ptr<ModuleTemplate>> templates_by_id;
  std::string id;
  std::string name;
  std::string basedir;
  bool has_defaultlib;
  std::string defaultlib_path;
  std::shared_ptr<LibLoader> defaultlib;
};

class ModuleCollectionBase{
private:
  ModuleCollectionBase() = delete; // static class

  static std::map<std::string, std::shared_ptr<ModuleCollection>> collections_by_id;
public:
  static std::shared_ptr<ModuleCollection> GetCollectionByID(std::string id);
  static std::shared_ptr<ModuleCollection> InstallFile(std::string filepath);
  static void InstallDir(std::string dirpath);
  static std::string ListInstalledTemplates();
};

} // namespace AlgAudio
#endif //MODULE_CONNECTION
