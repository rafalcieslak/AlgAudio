#ifndef MODULE_CONNECTION
#define MODULE_CONNECTION
#include <fstream>
#include <memory>
#include <map>

#include "ModuleTemplate.hpp"
#include "Utilities.hpp"

namespace AlgAudio{

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
  ModuleCollection(std::ifstream& file);
  std::map<std::string, std::shared_ptr<ModuleTemplate>> templates_by_id;
  std::string id;
  std::string name;
  bool has_defaultlib;
  std::string defaultlib;
};

class ModuleCollectionBase{
private:
  ModuleCollectionBase() = delete; // static class

  static std::map<std::string, std::shared_ptr<ModuleCollection>> collections_by_id;
public:
  static std::shared_ptr<ModuleCollection> GetByID(std::string id);
  static std::shared_ptr<ModuleCollection> InstallFile(std::string filepath);
  static void InstallDir(std::string dirpath);
  static std::string ListInstalledTemplates();
};

} // namespace AlgAudio
#endif //MODULE_CONNECTION
