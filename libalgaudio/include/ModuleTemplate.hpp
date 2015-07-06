#ifndef MODULE_TEMPLATE
#define MODULE_TEMPLATE
#include <string>

// Forward declaration, to aviod rapidxml becoming a dependency for
// external modules
namespace rapidxml{
  template <class T> class xml_node;
}


namespace AlgAudio{

class ModuleCollection;

struct ModuleParseException{
  ModuleParseException(std::string i, std::string t) : id(i), text(t) {};
  std::string id;
  std::string text;
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

}
#endif //MODULE_TEMPLATE
