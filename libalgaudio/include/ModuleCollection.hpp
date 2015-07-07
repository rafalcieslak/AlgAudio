#ifndef MODULE_CONNECTION
#define MODULE_CONNECTION
#include <fstream>
#include <list>
#include <memory>

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

class ModuleCollection{
public:
  ModuleCollection(std::ifstream& file);
  std::list<std::shared_ptr<ModuleTemplate>> templates;
  std::string id;
  std::string name;
  bool has_defaultlib;
  std::string defaultlib;
};

} // namespace AlgAudio
#endif //MODULE_CONNECTION
