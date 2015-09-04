#ifndef BUILTINMODULES_HPP
#define BUILTINMODULES_HPP
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
#include "Module.hpp"
#include <array>

namespace AlgAudio{
namespace Builtin{

class SubpatchEntrance;

class Subpatch : public Module{
public:
  void on_init() override;
  void on_destroy() override;
  void on_gui_build(std::shared_ptr<ModuleGUI> gui) override;
  void state_store_xml(rapidxml::xml_node<char>*) const override;
  void state_load_xml(rapidxml::xml_node<char>*) override;
  
  // Marks the given entrance as an entrance to this subpatch. If buses
  // are ready, it applies their ids to the entrance. If buses are not
  // ready, the ids will be applied as soon as the buses are ready.
  void LinkToEntrance(std::shared_ptr<SubpatchEntrance>);
  bool HasEntrance() const {return entrance != nullptr;};
  
  // Used for ordering calculation. TODO: Move to an interface imlpemented by both subpatch and poly.
  int GetGroupID() const;
private:
  std::shared_ptr<Canvas> internal_canvas;
  std::shared_ptr<SubpatchEntrance> entrance;
};
class SubpatchEntrance : public Module{
public:
  void on_init() override;
  void LinkOutput(int output_no, int busid);
private:
};
  
std::shared_ptr<Module> CreateInstance(std::string id);
  
}} // namespace AlgAudio::Builtin

#endif // BUILTINMODULES_HPP
