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
  int GetID() const {return id;}
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
    std::string id;
    Module& mod;
    // The outlet is not the owner of a bus.
    std::weak_ptr<Bus> bus;
    static std::shared_ptr<Outlet> Create(std::string id, Module& mod);
  private:
    Outlet(std::string i, Module& m) : id(i), mod(m) {
      mod.SetParram(id, 999999999);
    }
  };
  class Inlet{
  public:
    ~Inlet(){
      //(*(int*)0x00000) = 1;
      std::cout << "Inlet destroyed" << std::endl;
    }
    static LateReply<std::shared_ptr<Inlet>> Create(std::string id, Module& mod);
    std::string id;
    Module& mod;
    // The inlet is the owner of a bus.
    std::shared_ptr<Bus> bus;
  private:
    Inlet(std::string i, Module& m, std::shared_ptr<Bus> b) : id(i), mod(m), bus(b) {}
  };
  void SetParram(std::string name, int value);

  LateReply<> CreateIOFromTemplate();
  std::vector<std::shared_ptr<Inlet>> inlets;
  std::vector<std::shared_ptr<Outlet>> outlets;

  static void Connect(std::shared_ptr<Module> a, std::shared_ptr<Module> b);
};

} // namespace AlgAudio

#endif //MODULE_HPP
