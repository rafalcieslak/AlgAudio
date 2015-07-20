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

namespace AlgAudio{

class ModuleTemplate;
class Canvas;

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
  class Inlet{
  public:
    Inlet(std::string i, Module& m) : id(i), mod(m) {}
    std::string id;
    Module& mod;
    int connection = -1;
  };
  class Outlet{
  public:
    Outlet(std::string i, Module& m) : id(i), mod(m) {}
    std::string id;
    Module& mod;
    int connection = -1;
  };

  void AddInlet(std::string s);
  void AddOutlet(std::string s);
  void CreateIOFromTemplate();
  std::vector<Inlet> inlets;
  std::vector<Outlet> outlets;
};


} // namespace AlgAudio

#endif //MODULE_HPP
