#ifndef MODULEFACTORY_HPP
#define MODULEFACTORY_HPP
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

#include <memory>
#include <set>
#include "Module.hpp"
#include "ModuleTemplate.hpp"
#include "LateReturn.hpp"

namespace AlgAudio{

namespace Exceptions{
struct ModuleInstanceCreationFailed : public Exception{
  ModuleInstanceCreationFailed(std::string t, std::string id_) : Exception(t), id(id_) {};
  std::string id;
};
} // namespace Exceptions

class Canvas;

/** This class contains procedures for correct creation and destruction of
 *  Module instances. */
class ModuleFactory{
private:
  ModuleFactory() = delete; // static class
  static std::set<std::shared_ptr<Module>> instances;
public:
  /** Creates, initializes and installs a new module instance. This is the
   *  correct way to create new module instances. In case of problems, this
   *  method may latethrow Exceptions::ModuleInstanceCreationFailed. The
   *  returned pointer is never null, and always points to a valid module.
   *  \param templ The module template to use when creating a new instance.
   *  \param parent The parent canvas where this new instance shall be installed. */
  static LateReturn<std::shared_ptr<Module>> CreateNewInstance(std::shared_ptr<ModuleTemplate> templ, std::shared_ptr<Canvas> parent);
    /** Creates, initializes and installs a new module instance. This is the
     *  correct way to create new module instances. In case of problems, this
     *  method may latethrow Exceptions::ModuleInstanceCreationFailed. The
     *  returned pointer is never null, and always points to a valid module.
     *  \param id The module template id to use when creating a new instance. 
     *  \param parent The parent canvas where this new instance shall be installed. */
  static LateReturn<std::shared_ptr<Module>> CreateNewInstance(std::string id, std::shared_ptr<Canvas> parent);
  static LateReturn<> DestroyInstance(std::shared_ptr<Module>);
  static std::shared_ptr<ModuleTemplate> GetTemplateByID(std::string);
};

} // namnespace AlgAudio
#endif // MODULEFACTORY_HPP
