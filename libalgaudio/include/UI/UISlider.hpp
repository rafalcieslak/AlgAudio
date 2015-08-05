#ifndef UISLIDER_HPP
#define UISLIDER_HPP
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
#include "UI/UIWidget.hpp"

namespace AlgAudio{

struct ParramController;

class UISlider : public UIWidget{
public:
  static std::shared_ptr<UISlider> Create(std::weak_ptr<Window> parent_window, std::shared_ptr<ParramController> controller);
  void CustomDraw(DrawContext& c) override;
protected:
  UISlider(std::weak_ptr<Window> parent_window, std::shared_ptr<ParramController> controller);
  std::weak_ptr<ParramController> controller;
};


} // namespace AlgAudio

#endif // UISLIDER_HPP
