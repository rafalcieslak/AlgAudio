#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP
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
#include "ModuleSelector.hpp"
#include "UI/UIBox.hpp"
#include "UI/UIButton.hpp"
#include "UI/UILayered.hpp"
#include "UI/UILabel.hpp"
#include "UI/UISeparator.hpp"
#include "UI/UIAlert.hpp"
#include "UI/UISeparator.hpp"
#include "CanvasView.hpp"

namespace AlgAudio{

// TODO: Singleton?
class MainWindow : public Window{
public:
  static std::shared_ptr<MainWindow> Create();
private:
  MainWindow();
  void init();

  std::shared_ptr<UIVBox> mainvbox;
  std::shared_ptr<UIButton> addbutton;
  std::shared_ptr<UIButton> quitbutton;
  std::shared_ptr<UIHBox> toolbarbox;
  std::shared_ptr<UISeparator> toolbar_separator;
  std::shared_ptr<ModuleSelector> selector;
  std::shared_ptr<UILayered> layered;
  std::shared_ptr<UILayered> layered_alert;
  std::shared_ptr<UIMarginBox> marginbox_alert;
  std::shared_ptr<UIAlert> alert;
  std::shared_ptr<CanvasView> canvasview;
};

} //namespace AlgAudio
#endif // MAINWINDOW_HPP
