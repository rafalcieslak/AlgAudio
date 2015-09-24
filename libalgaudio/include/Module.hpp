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
#include <unordered_set>
#include "DynamicallyLoadableClass.hpp"
#include "Signal.hpp"
#include "Utilities.hpp"
#include "LateReturn.hpp"
#include "Timer.hpp"

// Forward declaration to strip rapidxml header dependecncy
namespace rapidxml{
  template <class Ch>
  class xml_node;
}

namespace AlgAudio{

namespace Exceptions{
/** Third pardy modules may throw this from on_init if conditions for creating
 *  and instance of this module are not met, for example, if multiple instances
 *  of SubpatchEntrance would exist on the same canvas. */
struct ModuleDoesNotWantToBeCreated : public Exception{
  ModuleDoesNotWantToBeCreated(std::string t) : Exception(t){};
};
} // namespace Exceptions

class ModuleTemplate;
class ParamTemplate;
class Canvas;
class ModuleGUI;
class Window;
class Module;
class ParamController;
class SendReplyController;

/** This is a wrapper class for SC buses */
class Bus{
public:
  int GetID() const {return id;}
  /**  Asks SC for a new bus id, and returns a new Bus instance wrapping that bus. */
  static LateReturn<std::shared_ptr<Bus>> CreateNew();
  /** Creates a fake Bus instance, which does not wrap anything. Useful only for
   *  testing module instances without OSC connection. */
  static std::shared_ptr<Bus> CreateFake();
  ~Bus();
private:
  Bus(int id);
  int id;
};

/** This is a wrapper class for SC groups */
class Group{
public:
  int GetID() const {return id;}
  /** Asks SC for a new group id, and returns a new Group instance wrapping that group. */
  static LateReturn<std::shared_ptr<Group>> CreateNew(std::shared_ptr<Group> parent);
  /** Creates a group that has no corresponding SC instance. */
  static LateReturn<std::shared_ptr<Group>> CreateFake(std::shared_ptr<Group> parent);
  ~Group();
private:
  Group(int id);
  int id;
};

// =================

/** This is the module, the main class that represents any module instance.
 *  It is intended to:
 *  1. serve as a client-side representation of a SC Synth instance
 *  2. provide an interface to be overriden by third-party module extensions.
 */
class Module : public DynamicallyLoadableClass, public virtual SubscriptionsManager, public std::enable_shared_from_this<Module>, public virtual TimerHandleManager{
public:
  ///@{
  /** You don't usually this constructor. Create a module instance
   *  using ModuleFactory instead, this will ensure proper initialisation. */
  Module() {};
  Module(void (*deleter)(void*)) : DynamicallyLoadableClass(deleter) {};
  Module(std::shared_ptr<ModuleTemplate> t) : templ(t) {};
  ///@}
  /** No copy constructing. */
  Module(const Module& other) = delete;
  virtual ~Module();

  /** Custom module implementations will prefer to override on_init and
   *  on_destroy, instead of creating a custom constructor/destructor. on_init
   *  is guaranteed to run when the module is ready to work - the corresponding
   *  SC instance was created etc. Similarly, on_destroy is called before the SC
   *  instance is removed, while the buses still exist etc. */
  virtual void on_init() {};
  virtual LateReturn<> on_init_latereturn() {
    Relay<> r;
    on_init();
    return r.Return();
  };
  virtual void on_destroy() {};
  /** This method will be called when the module GUI is ready to be displayed.
   * If you wish to modify the GUI but do not want to override BuildGUI with
   * custom builder, you override on_gui_build and modify the UI from there. */
  virtual void on_gui_build(std::shared_ptr<ModuleGUI>) {};
  /** This method is used for params defined with action="custom" attribute. */
  virtual void on_param_set(std::string, float) {}

  /** These methods are used for storing and loading internal module state.
   *  They are mostly used when a canvas is saved, or when a module is copied
   *  to clipboard. State loading will never happen before on_init. */
  virtual std::string state_store_string() const {return "";}
  virtual void state_load_string(std::string) {}
  virtual void state_store_xml(rapidxml::xml_node<char>*) const {}
  virtual void state_load_xml(rapidxml::xml_node<char>*) {}

  /* The template this module instance is based on. */
  std::shared_ptr<ModuleTemplate> templ;

  /** The id of the supercollider synth instance this Module represents and
   *  manages. */
  int sc_id = -1;

  /** This variable stores the widget position in canvas. */
  Point2D position_in_canvas;

  /** This flag marks whether this module was initialized by ModuleFactory.
   *  When ModuleFactory destoys a module, this flag is set back to false.
   *  This way it's easy to detect whether the module was correctly created,
   *  and when it's destructed witout havning been destroyed by the factory.
   */
  bool enabled_by_factory = false;

  class Inlet; // Forward decl

  // TODO: Common base class
  class Outlet{
  public:
    std::string id;
    std::string name;
    Module& mod;
    // The outlet is not the owner of the buses.
    std::list<std::weak_ptr<Bus>> buses;
    LateReturn<> ConnectToInlet(std::shared_ptr<Inlet> i);
    LateReturn<> DetachFromInlet(std::shared_ptr<Inlet> i);
    LateReturn<> DetachFromAll();
    static std::shared_ptr<Outlet> Create(std::string id, std::string name, std::shared_ptr<Module> mod);
    ~Outlet(){
      std::cout << "Outlet freed" << std::endl;
    }
  private:
    LateReturn<> SendConnections();
    Outlet(std::string i, std::string n, std::shared_ptr<Module> m) : id(i), name(n), mod(*m.get()) {}
  };
  class Inlet{
  public:
    // If fake is set to true, this inlet will have no corresponding bus. Pointless to use, great for debugging.
    static LateReturn<std::shared_ptr<Inlet>> Create(std::string id, std::string name, std::shared_ptr<Module> mod, bool fake = false);
    std::string id;
    std::string name;
    Module& mod;
    // The inlet is the owner of a bus.
    std::shared_ptr<Bus> bus;
  private:
    Inlet(std::string i, std::string n, std::shared_ptr<Module> m, std::shared_ptr<Bus> b) : id(i), name(n), mod(*m.get()), bus(b) {}
  };

  /** Returns a reference to the ModuleGUI that represents this particular module
   *  instance. If the GUI was not yet build, or was already deleted, this
   *  function will return nullptr. */
  std::shared_ptr<ModuleGUI> GetGUI() const;

  /** This function is used to create a new ModuleGUI that will represent this
   *  module instance in a CanvasView. Custom modules can override this function
   *  with anything they desire, as long as it returns any ModuleGUI. It can be
   *  a custom class that derives from ModuleGUI. However, there is no
   *  necessity to override this method - the default implementation will create
   *  the GUI hierarchy according to the description from collection XML. The
   *  only argument to this function is the value of "type" attribute of xml's
   *  "gui" node.
   *  Whoever calls this function claims the ownership of the returned pointer,
   *  it will not be owned by the module instance.
   */
  virtual std::shared_ptr<ModuleGUI> BuildGUI(std::shared_ptr<Window> parent_window);

  void DropGUI() {modulegui.reset();}
  
  /** Creates and allocates Inlets and Outlets for this instance, according to
   *  the data in template info. */
  LateReturn<> CreateIOFromTemplate(bool fake = false);
  std::vector<std::shared_ptr<Inlet>> inlets;
  std::vector<std::shared_ptr<Outlet>> outlets;

  void PrepareParamControllers();

  /** Sets all controllers to default values */
  void ResetControllers();

  // TODO: Make this a map?
  std::vector<std::shared_ptr<ParamController>> param_controllers;

  std::vector<std::shared_ptr<SendReplyController>> reply_controllers;

  /** Returns a ParamController by given ID. */
  std::shared_ptr<ParamController> GetParamControllerByID(std::string) const;

  /** Returns inlets by their ID. */
  std::shared_ptr<Inlet >  GetInletByID(std::string id) const;
  /** Returns outlets by their ID. */
  std::shared_ptr<Outlet> GetOutletByID(std::string id) const;

  /** The canvas this module belongs to. */
  std::weak_ptr<Canvas> canvas;
  
protected:
  std::shared_ptr<ModuleGUI> modulegui;
};

} // namespace AlgAudio

#endif //MODULE_HPP
