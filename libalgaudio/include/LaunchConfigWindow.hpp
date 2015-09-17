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
#include "Console.hpp"
#include "UI/UIButton.hpp"
#include "UI/UIMarginBox.hpp"
#include "UI/UITextArea.hpp"
#include "UI/UIBox.hpp"
#include "UI/UILabel.hpp"
#include "UI/UICheckbox.hpp"
#include "UI/UIProgressBar.hpp"
#include "UI/UITextEntry.hpp"
#include "UI/UILayered.hpp"
#include "UI/UISeparator.hpp"
#include "UI/UISpinEntry.hpp"
#include "UI/UIPathSelector.hpp"

namespace AlgAudio{

/* This class is an implementation of a Window which displays the welcome
 * dialog and AlgAudio launcher config.
 */
class LaunchConfigWindow : public Window{
public:
  static std::shared_ptr<LaunchConfigWindow> Create();
  /** This signal happens when the configuration and server starting has
   *  been completed. It's probably the time to open the main window. */
  Signal<> on_complete;
private:
  LaunchConfigWindow();
  void init();
  /** Stores configuration from UI to the global Config */
  void ApplyToGlobalConfig();
  /** Switches back and forth between configuration display and about dialog. */
  void ToggleAbout();
  bool about_displayed = false;

std::shared_ptr<UIMarginBox> marginbox;
  std::shared_ptr<UIVBox> mainvbox;
    std::shared_ptr<UILabel> titlelabel;
    std::shared_ptr<UILabel> version_label;
    std::shared_ptr<UISeparator> config_separator;
    std::shared_ptr<UILayered> layered;
      std::shared_ptr<UIVBox> about_box;
        std::shared_ptr<UILabel> about_text;
      std::shared_ptr<UIVBox> configbox;
        std::shared_ptr<UILabel> path_label;
        std::shared_ptr<UIPathSelector> sclang_path_selector;
        std::shared_ptr<UISeparator> config_sep;
        std::shared_ptr<UIHBox> config_audio;
          std::shared_ptr<UIVBox> config_audioA;
            std::shared_ptr<UISpinEntry> config_inchannels;
            std::shared_ptr<UISpinEntry> config_samplerate;
          std::shared_ptr<UIVBox> config_audioB;
            std::shared_ptr<UISpinEntry> config_outchannels;
            std::shared_ptr<UISpinEntry> config_blocksize;
        std::shared_ptr<UISeparator> config_widesep;
        std::shared_ptr<UIVBox> config_adv;
          std::shared_ptr<UIHBox> config_adv_driver_box;
            std::shared_ptr<UILabel> config_adv_driver_label;
            std::shared_ptr<UITextEntry> config_adv_driver_entry;
          std::shared_ptr<UIHBox> config_adv_chbox;
            std::shared_ptr<UIVBox> config_advA;
              std::shared_ptr<UICheckbox> chk_supernova;
              std::shared_ptr<UICheckbox> chk_nosclang;
            std::shared_ptr<UIVBox> config_advB;
              std::shared_ptr<UICheckbox> chk_debug;
              std::shared_ptr<UICheckbox> chk_oscdebug;
        std::shared_ptr<UICheckbox> chk_advconfig;
    std::shared_ptr<UIHBox> buttonhbox;
      std::shared_ptr<UIButton> quitbutton;
      std::shared_ptr<UIButton> testbutton;
      std::shared_ptr<UIButton> aboutbutton;
      std::shared_ptr<UIButton> startbutton;
    std::shared_ptr<UIProgressBar> progressbar;
    std::shared_ptr<UILabel> statustext;
  

  std::shared_ptr<Console> console;
  
  bool start_in_progress = false;
};

} //namespace AlgAudio
#endif // LAUNCHCONFIGWINDOW_HPP
