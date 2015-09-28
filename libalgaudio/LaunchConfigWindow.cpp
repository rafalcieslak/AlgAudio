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
#include "LaunchConfigWindow.hpp"
#include "SCLang.hpp"
#include "Theme.hpp"
#include "Canvas.hpp"
#include "MainWindow.hpp"
#include "SDLMain.hpp"
#include "Version.hpp"
#include "Config.hpp"

namespace AlgAudio{

LaunchConfigWindow::LaunchConfigWindow() : Window("AlgAudio config",290,425,true,false){
}

void LaunchConfigWindow::init(){
  std::weak_ptr<Window> w = shared_from_this();
  marginbox = UIMarginBox::Create(w,10,10,2,10);
  startbutton = UIButton::Create(w,"Start!");
  aboutbutton = UIButton::Create(w,"About");
  quitbutton = UIButton::Create(w,"Quit");
  titlelabel = UILabel::Create(w,"AlgAudio",52);
  chk_oscdebug = UICheckbox::Create(w,"Debug OSC");
  chk_debug = UICheckbox::Create(w,"Debug tools");
  chk_nosclang = UICheckbox::Create(w,"Disable SC");
  chk_supernova = UICheckbox::Create(w,"Supernova");
#ifndef __unix__
  chk_supernova->SetDisplayMode(UIWidget::DisplayMode::EmptySpace);
#endif
  chk_advconfig = UICheckbox::Create(w,"Advanced configuration");
  mainvbox = UIVBox::Create(w);
  configbox = UIVBox::Create(w);
  configbox->SetPadding(4);
  config_sep = UISeparator::Create(w);
  config_sep->SetCustomSize(Size2D(0,10));
  config_widesep = UISeparator::Create(w);
  config_audio = UIHBox::Create(w);
  config_audio->SetPadding(2);
  config_audioA = UIVBox::Create(w);
  config_audioA->SetPadding(2);
  config_audioB = UIVBox::Create(w);
  config_audioB->SetPadding(2);
  config_inchannels  = UISpinEntry::Create(w, "In channels:", 2, 0, 64);
  config_outchannels = UISpinEntry::Create(w, "Out channels:", 2, 0, 64);
  config_samplerate = UISpinEntry::Create(w, "Sample rate:", 48000, 8000, 300000);
  config_blocksize = UISpinEntry::Create(w, "Block size:", 64, 1, 65536);
  config_adv = UIVBox::Create(w);
  config_adv->SetPadding(2);
  config_adv_driver_box = UIHBox::Create(w);
#ifdef __unix__
  config_adv_driver_box->SetDisplayMode(UIWidget::DisplayMode::EmptySpace);
#endif
  config_adv_driver_label = UILabel::Create(w,"SC audio device name: ",14);
  config_adv_driver_entry = UITextEntry::Create(w);
  config_adv_driver_entry->SetFontSize(14);
  config_adv_driver_entry->SetDefaultText("(Default device)");
  config_adv_chbox = UIHBox::Create(w);
  config_adv->SetDisplayMode(UIWidget::DisplayMode::EmptySpace);
  config_advA = UIVBox::Create(w);
  config_advA->SetPadding(2);
  config_advB = UIVBox::Create(w);
  config_advB->SetPadding(2);
  buttonhbox = UIHBox::Create(w);
  progressbar = UIProgressBar::Create(w);
  statustext = UILabel::Create(w,"AlgAudio (C) CeTA 2015, released on GNU LGPL 3",12);
  statustext->SetCustomSize(Size2D(0,32));
  layered = UILayered::Create(w);
  about_box = UIVBox::Create(w);
  about_box->SetDisplayMode(UIWidget::DisplayMode::Invisible);
  about_text = UILabel::Create(w,
    "Copyright (C) 2015 CeTA - Audiovisual Technology Center\n"
    "Copyright (C) 2015 Rafal Cieslak\n"
    "\n"
    "Released on the terms of the GNU Lesser General Public\n"
    "License version 3. See LICENCE file for details, or go to:\n"
    "      https://www.gnu.org/licenses/lgpl.txt"
    ,12);
  about_text->SetAlignment(HorizAlignment_CENTERED, VertAlignment_TOP);
  version_label = UILabel::Create(w, ALGAUDIO_VERSION_SHORT, 16);
  version_label->SetAlignment(HorizAlignment_CENTERED, VertAlignment_TOP);
  config_separator = UISeparator::Create(w);
  config_separator->SetCustomSize(Size2D(0,20));
  sclang_path_selector = UIPathSelector::Create(w, Config::Global().path_to_sclang);
  sclang_path_selector->SetName("Select the location of '" + Utilities::OSSCLangBinName + "' file");
  path_label = UILabel::Create(w, "Path to SuperCollider's binary '" + Utilities::OSSCLangBinName + "'", 14);

  Insert(marginbox);
  marginbox->Insert(mainvbox);
   mainvbox->Insert(titlelabel, UIBox::PackMode::TIGHT);
   mainvbox->Insert(version_label, UIBox::PackMode::TIGHT);
   mainvbox->Insert(config_separator, UIBox::PackMode::TIGHT);
   mainvbox->Insert(layered, UIBox::PackMode::WIDE);
    layered->Insert(configbox);
     configbox->Insert(path_label, UIBox::PackMode::TIGHT);
     configbox->Insert(sclang_path_selector, UIBox::PackMode::TIGHT);
     configbox->Insert(config_sep, UIBox::PackMode::TIGHT);
     configbox->Insert(config_audio, UIBox::PackMode::TIGHT);
       config_audio->Insert(config_audioA, UIBox::PackMode::WIDE);
         config_audioA->Insert(config_inchannels, UIBox::PackMode::TIGHT);
         config_audioA->Insert(config_outchannels, UIBox::PackMode::TIGHT);
       config_audio->Insert(config_audioB, UIBox::PackMode::WIDE);
         config_audioB->Insert(config_samplerate, UIBox::PackMode::TIGHT);
         config_audioB->Insert(config_blocksize , UIBox::PackMode::TIGHT);
     configbox->Insert(config_adv, UIBox::PackMode::TIGHT);
       config_adv->Insert(config_adv_driver_box, UIBox::PackMode::TIGHT);
         config_adv_driver_box->Insert(config_adv_driver_label, UIBox::PackMode::TIGHT);
         config_adv_driver_box->Insert(config_adv_driver_entry, UIBox::PackMode::WIDE);
       config_adv->Insert(config_adv_chbox, UIBox::PackMode::TIGHT);
         config_adv_chbox->Insert(config_advA, UIBox::PackMode::WIDE);
           config_advA->Insert(chk_debug, UIBox::PackMode::TIGHT);
           config_advA->Insert(chk_oscdebug, UIBox::PackMode::TIGHT);
         config_adv_chbox->Insert(config_advB, UIBox::PackMode::WIDE);
           config_advB->Insert(chk_nosclang, UIBox::PackMode::TIGHT);
           config_advB->Insert(chk_supernova, UIBox::PackMode::TIGHT);
     configbox->Insert(config_widesep, UIBox::PackMode::WIDE);
     configbox->Insert(chk_advconfig, UIBox::PackMode::TIGHT);
    layered->Insert(about_box);
      about_box->Insert(about_text, UIBox::PackMode::WIDE);
   mainvbox->Insert(buttonhbox, UIBox::PackMode::TIGHT);
    buttonhbox->Insert(quitbutton, UIHBox::PackMode::WIDE);
    buttonhbox->Insert(aboutbutton, UIHBox::PackMode::WIDE);
    buttonhbox->Insert(startbutton, UIHBox::PackMode::WIDE);
   mainvbox->Insert(progressbar, UIBox::PackMode::TIGHT);
   mainvbox->Insert(statustext, UIBox::PackMode::TIGHT);

  startbutton->SetColors(Theme::Get("text-button"), Theme::Get("bg-button-positive"));
  quitbutton->SetColors(Theme::Get("text-button"), Theme::Get("bg-button-negative"));

  subscriptions += startbutton->on_clicked.Subscribe([this](){
    
    ApplyToGlobalConfig();
    
    statustext->SetTextColor(Theme::Get("text-generic"));
    statustext->SetBold(false);
    statustext->SetText("Starting...");
    // Hide the start button
    startbutton->SetDisplayMode(UIWidget::DisplayMode::EmptySpace);
    
    if(Config::Global().use_sc){
      start_in_progress = true;
      SCLang::Start();
    }else{
      on_complete.Happen();
    }
    
  });
  subscriptions += SCLang::on_start_progress.Subscribe([this](int n, std::string msg){
    progressbar->SetAmount(n/10.0);
    statustext->SetAlignment(HorizAlignment_CENTERED,VertAlignment_TOP);
    statustext->SetText(msg);
  });
  subscriptions += aboutbutton->on_clicked.Subscribe([this](){
    ToggleAbout();
  });

  subscriptions += quitbutton->on_clicked.Subscribe([this](){
    on_close.Happen();
  });
  subscriptions += chk_oscdebug->on_toggled.Subscribe([](bool state){
    SCLang::SetOSCDebug(state);
  });
  subscriptions += chk_debug->on_toggled.Subscribe([this](bool enabled){
    if(enabled){
      console = Console::Create();
      subscriptions += console->on_close.Subscribe([this](){
        chk_debug->SetActive(false); // This will also unregister the window.
      });
      SDLMain::RegisterWindow(console);
    }else{
      SDLMain::UnregisterWindow(console);
      console = nullptr;
    }
  });
  subscriptions += chk_advconfig->on_toggled.Subscribe([this](bool enabled){
    if(enabled){
      config_adv->SetDisplayMode(UIWidget::DisplayMode::Visible);
    }else{
      config_adv->SetDisplayMode(UIWidget::DisplayMode::EmptySpace);
    }
  });
  subscriptions += SCLang::on_start_completed.Subscribe([this](bool success, std::string message){
    if(success){
      on_complete.Happen();
    }else{
      start_in_progress = false;
      statustext->SetText(message);
      statustext->SetBold(true);
      statustext->SetTextColor(Theme::Get("text-error"));
      statustext->SetAlignment(HorizAlignment_LEFT,VertAlignment_TOP);
      progressbar->SetAmount(0);
      startbutton->SetDisplayMode(UIWidget::DisplayMode::Visible);
    }
  });
}

std::shared_ptr<LaunchConfigWindow> LaunchConfigWindow::Create(){
  auto res = std::shared_ptr<LaunchConfigWindow>( new LaunchConfigWindow());
  res->init();
  return res;
}

void LaunchConfigWindow::ToggleAbout(){
  about_displayed = !about_displayed;
  if(about_displayed){
    quitbutton->SetDisplayMode(UIWidget::DisplayMode::EmptySpace);
    startbutton->SetDisplayMode(UIWidget::DisplayMode::EmptySpace);
    progressbar->SetDisplayMode(UIWidget::DisplayMode::EmptySpace);
    statustext->SetDisplayMode(UIWidget::DisplayMode::EmptySpace);
    
    configbox->SetDisplayMode(UIWidget::DisplayMode::Invisible);
    about_box->SetDisplayMode(UIWidget::DisplayMode::Visible);
    
    aboutbutton->SetText("Back");
  }else{
    quitbutton->SetDisplayMode(UIWidget::DisplayMode::Visible);
    startbutton->SetDisplayMode(start_in_progress ? UIWidget::DisplayMode::EmptySpace : UIWidget::DisplayMode::Visible);
    progressbar->SetDisplayMode(UIWidget::DisplayMode::Visible);
    statustext->SetDisplayMode(UIWidget::DisplayMode::Visible);
    
    configbox->SetDisplayMode(UIWidget::DisplayMode::Visible);
    about_box->SetDisplayMode(UIWidget::DisplayMode::Invisible);
    
    aboutbutton->SetText("About");
  }
}

void LaunchConfigWindow::ApplyToGlobalConfig(){
  Config& c = Config::GlobalWriteable();
  c.path_to_sclang = sclang_path_selector->GetPath();
  c.supernova = chk_supernova->GetActive();
  c.use_sc = ! chk_nosclang->GetActive();
  c.debug = chk_debug->GetActive();
  c.debug_osc = chk_oscdebug->GetActive();
  c.scsynth_audio_driver_name = config_adv_driver_entry->GetText();
  c.input_channels = config_inchannels->GetValue();
  c.output_channels = config_outchannels->GetValue();
  c.sample_rate = config_samplerate->GetValue();
  c.block_size = config_blocksize->GetValue();
}

} // namespace AlgAudio
