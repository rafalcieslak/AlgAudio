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
#include "LateReply.hpp"

namespace AlgAudio{

class ModuleTemplate;
class Canvas;

// A wrapper for SC buses.
class Bus{
public:
  int GetID() {return id;}
  static LateReply<std::shared_ptr<Bus>> CreateNew();
  ~Bus();
private:
  Bus(int id);
  int id;
};

class Module : public DynamicallyLoadableClass, public SubscriptionsManager{
public:
  Module(){};
  Module(void (*deleter)(void*)) : DynamicallyLoadableClass(deleter) {};
  Module(std::shared_ptr<ModuleTemplate> t) : templ(t) {};
  virtual ~Module() {};
  virtual void on_init() {};
  std::shared_ptr<ModuleTemplate> templ;
  int sc_id = -1;
  std::weak_ptr<Canvas> canvas;

  // TODO: Common base class
  class Outlet{
  public:
    Outlet(std::string i, Module& m) : id(i), mod(m) {
      mod.SetParram(id, 999999999);
    }
    std::string id;
    Module& mod;
    // The outlet is not the owner of a bus.
    std::weak_ptr<Bus> bus;
  };
  class Inlet{
  public:
    Inlet(std::string i, Module& m) : id(i), mod(m) {
      Bus::CreateNew().Then([=](std::shared_ptr<Bus> b){
        std::cout << b.get() << " " << this << std::endl;
        bus = b;
        mod.SetParram(id, b->GetID());
      });
    }
    std::string id;
    Module& mod;
    // The inlet is the owner of a bus.
    std::shared_ptr<Bus> bus;
  };
  void SetParram(std::string name, int value);

  void AddInlet(std::string s);
  void AddOutlet(std::string s);
  void CreateIOFromTemplate();
  std::vector<Inlet> inlets;
  std::vector<Outlet> outlets;

  static void Connect(std::shared_ptr<Module> a, std::shared_ptr<Module> b);
};

} // namespace AlgAudio

#endif //MODULE_HPP
