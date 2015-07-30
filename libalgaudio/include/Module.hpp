#ifndef MODULE_HPP
#define MODULE_HPP
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
#include <vector>
#include "DynamicallyLoadableClass.hpp"
#include "Signal.hpp"
#include "LateReturn.hpp"

namespace AlgAudio{

class ModuleTemplate;
class Canvas;
class ModuleGUI;
class Window;

// A wrapper for SC buses.
class Bus{
public:
  int GetID() const {return id;}
  static LateReturn<std::shared_ptr<Bus>> CreateNew();
  ~Bus();
private:
  Bus(int id);
  int id;
};

class Module : public DynamicallyLoadableClass, public SubscriptionsManager, public std::enable_shared_from_this<Module>{
public:
  Module() {};
  Module(void (*deleter)(void*)) : DynamicallyLoadableClass(deleter) {};
  Module(std::shared_ptr<ModuleTemplate> t) : templ(t) {};
  Module(const Module& other) = delete;
  virtual ~Module();

  // Custom module implementations will prefer to override on_init and
  // on_destroy, instead of creating a custom constructor/destructor. on_init
  // is guaranteed to run when the module is ready to work - the corresponding
  // SC instance was created etc. Similarly, on_destroy is called before the SC
  // instance is removed, while the buses still exist etc.
  virtual void on_init() {};
  virtual void on_destroy() {};

  // The template this module instance is based on.
  std::shared_ptr<ModuleTemplate> templ;

  // The id of the supercollider synth instance this Module represents and
  // manages.
  int sc_id = -1;


  // This flag marks whether this module was initialized by ModuleFactory.
  // When ModuleFactory destoys a module, this flag is set back to false.
  // This way it's easy to detect whether the module was correctly created,
  // and when it's destructed witout havning been destroyed by the factory.
  bool enabled_by_factory = false;

  // TODO: Common base class
  class Outlet{
  public:
    std::string id;
    Module& mod;
    // The outlet is not the owner of a bus.
    std::weak_ptr<Bus> bus;
    static std::shared_ptr<Outlet> Create(std::string id, std::shared_ptr<Module> mod);
    ~Outlet(){
      std::cout << "Outlet freed" << std::endl;
    }
  private:
    Outlet(std::string i, std::shared_ptr<Module> m) : id(i), mod(*m.get()) {
      mod.SetParram(id, 999999999);
    }
  };
  class Inlet{
  public:
    static LateReturn<std::shared_ptr<Inlet>> Create(std::string id, std::shared_ptr<Module> mod);
    std::string id;
    Module& mod;
    // The inlet is the owner of a bus.
    std::shared_ptr<Bus> bus;
  private:
    Inlet(std::string i, std::shared_ptr<Module> m, std::shared_ptr<Bus> b) : id(i), mod(*m.get()), bus(b) {}
  };
  void SetParram(std::string name, int value);
  void SetParram(std::string name, double value);

  // TODO: Move these functions to Canvas
  static void Connect(std::shared_ptr<Outlet> o, std::shared_ptr<Inlet> i);
  static void Connect(std::shared_ptr<Module> a, std::shared_ptr<Module> b);

  // Returns a reference to the ModuleGUI that represents this particular module
  // instance. If the GUI was not yet build, or was already deleted, this
  // function will return nullptr.
  std::shared_ptr<ModuleGUI> GetGUI();

  // This function is used to create a new ModuleGUI that will represent this
  // module instance in a CanvasView. Custom modules can override this function
  // with anything they desire, as long as it returns any ModuleGUI. It can be
  // a custom class that derives from ModuleGUI. However, there is no
  // necessity to override this method - the default implementation will create
  // the GUI hierarchy according to the description from collection XML. The
  // only argument to this function is the value of "type" attribute of xml's
  // "gui" node.
  // Whoever calls this function claims the ownership of the returned pointer,
  // it will not be owned by the module instance.
  virtual std::shared_ptr<ModuleGUI> BuildGUI(std::shared_ptr<Window> parent_window);

  // Creates and allocates Inlets and Outlets for this instance, according to
  // the data in template info.
  LateReturn<> CreateIOFromTemplate();
  std::vector<std::shared_ptr<Inlet>> inlets;
  std::vector<std::shared_ptr<Outlet>> outlets;

  std::weak_ptr<Canvas> canvas;
private:
  std::weak_ptr<ModuleGUI> modulegui;
};

} // namespace AlgAudio

#endif //MODULE_HPP
