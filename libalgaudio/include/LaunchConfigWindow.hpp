#ifndef LAUNCHCONFIGWINDOW_HPP
#define LAUNCHCONFIGWINDOW_HPP
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
#include "UI/UIButton.hpp"
#include "UI/UIMarginBox.hpp"
#include "UI/UITextArea.hpp"
#include "UI/UIBox.hpp"
#include "UI/UILabel.hpp"
#include "UI/UICheckbox.hpp"
#include "UI/UIProgressBar.hpp"

namespace AlgAudio{

/* This class is an implementation of a Window which displays the welcome
 * dialog and AlgAudio launcher config.
 */
class LaunchConfigWindow : public Window{
public:
  static std::shared_ptr<LaunchConfigWindow> Create();
  // This signal happens when the configuration and server starting has
  // been completed. It's time then to open the main window, probably.
  Signal<> on_complete;
private:
  LaunchConfigWindow();
  void init();

  std::shared_ptr<UIMarginBox> marginbox;
  std::shared_ptr<UIButton> startbutton;
  std::shared_ptr<UIButton> testbutton;
  std::shared_ptr<UIButton> quitbutton;
  std::shared_ptr<UILabel> titlelabel;
  std::shared_ptr<UILabel> configlabel;
  std::shared_ptr<UICheckbox> chkbox;
  std::shared_ptr<UICheckbox> supernovachkbox;
  std::shared_ptr<UIVBox> mainvbox;
  std::shared_ptr<UIHBox> buttonhbox;
  std::shared_ptr<UIProgressBar> progressbar;
  std::shared_ptr<UILabel> statustext;
};

} //namespace AlgAudio
#endif // LAUNCHCONFIGWINDOW_HPP
