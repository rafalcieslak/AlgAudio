#ifndef MODULE_CONNECTION
#define MODULE_CONNECTION
#include <fstream>
#include <list>
#include <memory>

#include "ModuleTemplate.hpp"

namespace AlgAudio{

struct CollectionParseException{
  CollectionParseException(std::string t) : text(t) {};
  std::string text;
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

}
#endif //MODULE_CONNECTION
