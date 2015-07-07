#ifndef MODULE_TEMPLATE
#define MODULE_TEMPLATE
#include <string>

#include "Utilities.hpp"

// Forward declaration, to aviod rapidxml becoming a dependency for
// external modules
namespace rapidxml{
  template <class T> class xml_node;
}

namespace AlgAudio{

class ModuleCollection;

struct ModuleParseException : public Exception{
  ModuleParseException(std::string i, std::string t) : Exception(t), id(i) {};
  ModuleParseException(std::string t) : Exception(t) {};
  virtual std::string what() override {
    if(id == "") return "While loading an unknown module: " + text;
    else return "While loading module '" + id + "': " + text;
  };
  std::string id = "";
};

class ModuleTemplate{
public:
  ModuleTemplate(ModuleCollection& collection);
  ModuleTemplate(ModuleCollection& collection, rapidxml::xml_node<char>* node);
  std::string id;
  std::string name;
  bool has_sc_code = false;
  std::string sc_code;
  bool has_class = false;
  std::string class_name;
  ModuleCollection& collection;

};

} // namespace AlgAudio
#endif //MODULE_TEMPLATE
