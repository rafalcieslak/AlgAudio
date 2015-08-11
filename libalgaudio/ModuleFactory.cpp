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
#include "SCLang.hpp"
#include "ModuleFactory.hpp"
#include "ModuleCollection.hpp"
#include "LibLoader.hpp"

namespace AlgAudio{

std::set<std::shared_ptr<Module>> ModuleFactory::instances;

LateReturn<std::shared_ptr<Module>> ModuleFactory::CreateNewInstance(std::string id){
  return CreateNewInstance( GetTemplateByID(id) );
}

LateReturn<std::shared_ptr<Module>> ModuleFactory::CreateNewInstance(std::shared_ptr<ModuleTemplate> templ){
  auto r = Relay<std::shared_ptr<Module>>::Create();
  std::shared_ptr<Module> res;
  if(!templ->has_class){
    res = std::make_shared<Module>(templ);
  }else{
    // Ask the corresponding libloader to create a class.
    // TODO: non-default libs
    Module* module = templ->collection.defaultlib->AskForInstance(templ->class_name);
    if(module == nullptr){
      std::cout << "Warning: Failed to create class instance '" << templ->class_name << "' from library." << std::endl;
      return r.Return(nullptr);
    }
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
  // Create SC instance
  if(templ->has_sc_code){
    if(!SCLang::ready){
      std::cout << "WARNING: Cannot create a new instance of " << templ->GetFullID() << ", the server is not yet ready." << std::endl;
      // TODO: Do not return an unlinked module reference! This is a temporary
      // trick which allows to quickly test GUI without starting the SC server.
      // Happen a global error signal instead.
      res->CreateIOFromTemplate(true); // Create fake io
      res->PrepareParamControllers();
      res->enabled_by_factory = true;
      res->on_init(); // temporary!
      res->ResetControllers();
      r.Return(res);
    }else{
      lo::Message m;
      // Use the full ID to identify SynthDef.
      m.add_string(templ->GetFullID());
      // Prepare a list of params. Set all output buses to 999999.
      for(const std::string& o : templ->outlets){
        m.add_string(o);
        m.add_int32(999999999);
      }
      SCLang::SendOSCCustomWithReply<int>("/algaudioSC/newinstanceparams", m)
        .Then([=](int id){
          std::cout << "On id " << id << std::endl;
          res->sc_id = id;
          res->CreateIOFromTemplate().Then([=](){
            res->PrepareParamControllers();
            res->enabled_by_factory = true;
            res->on_init();
            res->ResetControllers();
            r.Return(res);
          });
          }
        );
    }
  }else{
    res->enabled_by_factory = true;
    res->on_init();
    r.Return(res);
  }
  return r;
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

LateReturn<> ModuleFactory::DestroyInstance(std::shared_ptr<Module> m){
  Relay<> r;
  m->on_destroy();
  if(m->templ->has_sc_code){
    // Remove IO
    m->inlets.clear();
    m->outlets.clear();
    try{
      SCLang::SendOSCWithEmptyReply("/algaudioSC/removeinstance", "i", m->sc_id).Then([r,m](){
        m->enabled_by_factory = false;
        r.Return();
      });
    }catch(SCLangException){
      // Failed to send osc message? If we are unable to remove the instance
      // because SC or OSC is dead, then we won't be able to clean the instance
      // anyway.
      m->enabled_by_factory = false;
      r.Return();
    }
  }else{
    m->enabled_by_factory = false;
    r.Return();
  }
  return r;
}

}
