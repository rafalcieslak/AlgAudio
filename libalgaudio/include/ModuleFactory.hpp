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

struct ModuleInstanceCreationFailedException : public Exception{
  ModuleInstanceCreationFailedException(std::string t, std::string id_) : Exception(t), id(id_) {};
  virtual std::string what() override {return "Failed to create module instance '" + id + "': " + text;}
  std::string id;
};

class Canvas;

class ModuleFactory{
private:
  ModuleFactory() = delete; // static class
  static std::set<std::shared_ptr<Module>> instances;
public:
  // These methods latereturn two values - a pointer to new module, which is nullptr on failure, and an error message string, which is empty on success.
  static LateReturn<std::shared_ptr<Module>, std::string> CreateNewInstance(std::shared_ptr<ModuleTemplate> templ, std::shared_ptr<Canvas> parent);
  static LateReturn<std::shared_ptr<Module>, std::string> CreateNewInstance(std::string id, std::shared_ptr<Canvas> parent);
  static LateReturn<> DestroyInstance(std::shared_ptr<Module>);
  static std::shared_ptr<ModuleTemplate> GetTemplateByID(std::string);
};

} // namnespace AlgAudio
#endif // MODULEFACTORY_HPP
