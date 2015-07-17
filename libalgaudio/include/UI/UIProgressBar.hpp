#ifndef UIPROGRESSBAR_HPP
#define UIPROGRESSBAR_HPP
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
#include "UIWidget.hpp"

namespace AlgAudio{

class UIProgressBar : public UIWidget{
public:
  static std::shared_ptr<UIProgressBar> Create(std::weak_ptr<Window> parent_window);
  void SetAmount(double);
  virtual void CustomDraw(DrawContext& c) override;
private:
  UIProgressBar(std::weak_ptr<Window> parent_window);
  double amount;
};

} // namespace AlgAudio

#endif // UIPROGRESSBAR_HPP
