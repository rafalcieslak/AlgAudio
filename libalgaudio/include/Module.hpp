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
#include "DynamicallyLoadableClass.hpp"
#include "Signal.hpp"

namespace AlgAudio{

class ModuleTemplate;

class Module : public DynamicallyLoadableClass, public SubscriptionsManager{
public:
  Module(){};
  Module(void (*deleter)(void*)) : DynamicallyLoadableClass(deleter) {};
  Module(std::shared_ptr<ModuleTemplate> t) : templ(t) {};
  virtual ~Module() {};
  virtual void on_init() {};
  std::shared_ptr<ModuleTemplate> templ;
};

} // namespace AlgAudio

#endif //MODULE_HPP
