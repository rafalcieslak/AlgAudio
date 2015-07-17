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
#include "UI/UIProgressBar.hpp"
#include "Theme.hpp"

namespace AlgAudio{

UIProgressBar::UIProgressBar(std::weak_ptr<Window> w) : UIWidget(w){
  SetRequestedSize(Size2D(50,10));
}

std::shared_ptr<UIProgressBar> UIProgressBar::Create(std::weak_ptr<Window> w){
  std::shared_ptr<UIProgressBar> res(new UIProgressBar(w));
  return res;
}

void UIProgressBar::CustomDraw(DrawContext& c){
  c.SetColor(Theme::Get("progress-bar"));
  c.DrawRect(0,0, amount*c.width, c.height);
}

void UIProgressBar::SetAmount(double a){
  amount = a;
  SetNeedsRedrawing();
}


} // namespace AlgAudio
