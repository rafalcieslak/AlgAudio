#ifndef CANVAS_HPP
#define CANVAS_HPP
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
#include "Module.hpp"
#include "Utilities.hpp"

namespace AlgAudio{

class Canvas : public std::enable_shared_from_this<Canvas>{
public:
  virtual ~Canvas() {};
  void InsertModule(std::shared_ptr<Module>);
  static std::shared_ptr<Canvas> CreateEmpty();
private:
  Canvas();
  std::vector<std::shared_ptr<Module>> modules;
};

} // namespace AlgAudio

#endif //CANVAS_HPP
