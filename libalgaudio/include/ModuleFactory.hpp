#ifndef MODULEFACTORY_HPP
#define MODULEFACTORY_HPP

#include <memory>
#include <set>
#include "Module.hpp"
#include "ModuleTemplate.hpp"

namespace AlgAudio{

class ModuleFactory{
private:
  ModuleFactory() = delete; // static class
  static std::set<std::shared_ptr<Module>> instances;
public:
  static std::shared_ptr<Module> CreateNewInstance(std::shared_ptr<ModuleTemplate> templ);
  static std::shared_ptr<Module> CreateNewInstance(std::string id);
  static std::shared_ptr<ModuleTemplate> GetTemplateByID(std::string);
};

} // namnespace AlgAudio
#endif // MODULEFACTORY_HPP
