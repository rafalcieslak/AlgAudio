#include "ModuleFactory.hpp"
#include "ModuleCollection.hpp"
#include "LibLoader.hpp"

namespace AlgAudio{

std::set<std::shared_ptr<Module>> ModuleFactory::instances;

std::shared_ptr<Module> ModuleFactory::CreateNewInstance(std::string id){
  return CreateNewInstance( GetTemplateByID(id) );
}

std::shared_ptr<Module> ModuleFactory::CreateNewInstance(std::shared_ptr<ModuleTemplate> templ){
  std::shared_ptr<Module> res;
  if(!templ->has_class){
    res = std::make_shared<Module>(templ);
  }else{
    // Ask the corresponding libloader to create a class.
    // TODO: non-default libs
    Module* module = templ->collection.defaultlib->AskForInstance(templ->class_name);
    module->templ = templ;
    /*
    Wrapping the returned instance pointer in a shared_ptr is a bit tricky.
     When wrapped, we claim the ownership of the pointer, and we will be
     responsible of deleting it when all references are released. However,
     the pointer was originally created and allocated within the dynamically
     loaded library. It also has to be deleted there, deleting a pointer in
     a different module then it was created is undefined behavior. Therefore,
     a custom deleter is used. It calls the selfdestruct method, which refers
     to the internally remembered, library-specific deleter function, which
     deallocates the pointed instance from within the module.
    */
    res = std::shared_ptr<Module>(module, [](Module* mod){
      mod->SelfDestruct();
    });
  }
  res->on_init();
  return res;
}

std::shared_ptr<ModuleTemplate> ModuleFactory::GetTemplateByID(std::string id){
  std::vector<std::string> v = Utilities::SplitString(id, "/");
  if(v.size() < 2){
    std::cout << "Invalid template ID" << std::endl;
    return nullptr;
  }
  auto coll = ModuleCollectionBase::GetCollectionByID(v[0]);
  if(!coll) return nullptr;
  return coll->GetTemplateByID(v[1]);
}

}
