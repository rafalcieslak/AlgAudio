#ifndef MODULESELECTOR_HPP
#define MODULESELECTOR_HPP
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

#include "Window.hpp"
#include "UI/UIBox.hpp"
#include "UI/UIAnimDrawer.hpp"
#include "UI/UIList.hpp"
#include "UI/UILabel.hpp"

namespace AlgAudio{

class ModuleSelector : public UIHBox, public SubscriptionsManager{
public:
  static std::shared_ptr<ModuleSelector> Create(std::weak_ptr<Window> parent_window);
  void Populate() {PopulateLvl1();}
  void Expose();
  void Hide();
private:
  ModuleSelector(std::weak_ptr<Window> parent_window);
  void PopulateLvl1();
  void PopulateLvl2();
  void ShowLvl2();
  void init();
  std::shared_ptr<UIAnimDrawer> drawerlvl1;
  std::shared_ptr<UIAnimDrawer> drawerlvl2;
  std::shared_ptr<UIList> listlvl1;
  std::shared_ptr<UIList> listlvl2;
  std::shared_ptr<UILabel> description_label;

  std::shared_ptr<Subscription> lvl1_anim_end_wait;
  std::shared_ptr<Subscription> lvl2_anim_end_wait;

  std::string lvl1_selection = "";
};

} //namespace AlgAudio
#endif // MODULESELECTOR_HPP
