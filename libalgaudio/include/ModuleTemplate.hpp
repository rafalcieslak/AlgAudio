#ifndef MODULE_TEMPLATE
#define MODULE_TEMPLATE
/*
This file is part of AlgAudio.

AlgAudio, Copyright (C) 2015 CeTA - Audiovisual Technology Center

AlgAudio is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

AlgAudio is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with AlgAudio.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <string>
#include <memory>
#include <list>

#include "Utilities.hpp"

// Forward declaration, to aviod rapidxml becoming a dependency for
// external modules
namespace rapidxml{
  template <class T> class xml_node;
}

namespace AlgAudio{

class ModuleCollection;
class Module;

struct ModuleParseException : public Exception{
  ModuleParseException(std::string i, std::string t) : Exception(t), id(i) {};
  ModuleParseException(std::string t) : Exception(t) {};
  virtual std::string what() override {
    if(id == "") return "While loading an unknown module: " + text;
    else return "While loading module '" + id + "': " + text;
  };
  std::string id = "";
};

/* A template for creating params. Contains all data about a param that is
 * loaded from module description file. All ParamControlers have a link to their
 * corresponding template.
 */
class ParamTemplate{
public:
  std::string id;
  std::string name;
  enum class ParamAction{
    SC,
    Custom,
    None,
  };
  ParamAction action;
  enum class ParamMode{
    Input,
    Output,
    None,
  };
  ParamMode mode = ParamMode::Input;
  enum class ParamScale{
    Linear,
    Logarithmic
  };
  ParamScale scale;
  float default_min, default_max;
  float default_val;
};

class IOLetTemplate{
public:
  std::string id;
  std::string name;
};

/* All modules are build according to a template. If multiple instances
 * of the same module type are present, they will share a single instance
 * of ModuleTemplate.
 * ModuleTemplate contains all data about a module that are loaded from the
 * module description file. The fields are self-explainatory.
 */
class ModuleTemplate{
public:
  ModuleTemplate(ModuleCollection& collection);
  // Create a module template by parsing data from an XML module node.
  ModuleTemplate(ModuleCollection& collection, rapidxml::xml_node<char>* node);
  std::string GetFullID() const;
  std::string id;
  std::string name;
  std::string description = "";
  std::string guitype = "";
  std::string guitree = "";
  bool has_sc_code = false;
  std::string sc_code;
  bool has_class = false;
  std::string class_name;
  ModuleCollection& collection;
  std::list<IOLetTemplate> inlets;
  std::list<IOLetTemplate> outlets;
  std::list<std::shared_ptr<ParamTemplate>> params;
  std::list<std::pair<std::string, std::string>> replies;
};

} // namespace AlgAudio
#endif //MODULE_TEMPLATE
