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

namespace AlgAudio{

#ifdef __unix__
  static const std::string sclang_path = "/usr/bin/sclang";
  static const std::string scsynth_path = "/usr/bin/scsynth";
#else
  static const std::string sclang_path = "C:\\Program Files (x86)\\SuperCollider-3.6.6\\sclang.exe";
  static const std::string scsynth_path = "C:\\Program Files (x86)\\SuperCollider-3.6.6\\scsynth.exe";
#endif


LaunchConfigWindow::LaunchConfigWindow() : Window("AlgAudio config",280,400){
}

void LaunchConfigWindow::init(){
  marginbox = UIMarginBox::Create(shared_from_this(),10,10,10,10);
  startbutton = UIButton::Create(shared_from_this(),"Start AlgAudio");
  testbutton = UIButton::Create(shared_from_this(),"Test UI");
  testbutton->SetVisible(false);
  quitbutton = UIButton::Create(shared_from_this(),"Quit App");
  titlelabel = UILabel::Create(shared_from_this(),"AlgAudio",52);
  configlabel = UILabel::Create(shared_from_this(),"This place is left for config.");
  oscchkbox = UICheckbox::Create(shared_from_this(),"Enable OSC debugging");
  oscchkbox->SetVisible(false);
  supernovachkbox = UICheckbox::Create(shared_from_this(),"Enable Supernova mode");
  debugchkbox = UICheckbox::Create(shared_from_this(),"Enable debug tools");
  mainvbox = UIVBox::Create(shared_from_this());
  buttonhbox = UIHBox::Create(shared_from_this());
  progressbar = UIProgressBar::Create(shared_from_this());
  statustext = UILabel::Create(shared_from_this(),"AlgAudio (C) CeTA 2015, released on GNU LGPL 3",12);

  //mainvbox->SetPadding(10);
  Insert(marginbox);
  marginbox->Insert(mainvbox);
  mainvbox->Insert(titlelabel, UIBox::PackMode::TIGHT);
  mainvbox->Insert(configlabel, UIBox::PackMode::WIDE);
  mainvbox->Insert(oscchkbox, UIBox::PackMode::TIGHT);
  mainvbox->Insert(supernovachkbox, UIBox::PackMode::TIGHT);
  mainvbox->Insert(debugchkbox, UIBox::PackMode::TIGHT);
  mainvbox->Insert(buttonhbox, UIBox::PackMode::TIGHT);
  mainvbox->Insert(progressbar, UIBox::PackMode::TIGHT);
  mainvbox->Insert(statustext, UIBox::PackMode::TIGHT);
  buttonhbox->Insert(quitbutton, UIHBox::PackMode::WIDE);
  buttonhbox->Insert(testbutton, UIHBox::PackMode::WIDE);
  buttonhbox->Insert(startbutton, UIHBox::PackMode::WIDE);

  startbutton->SetColors(Theme::Get("text-button"), Theme::Get("bg-button-positive"));
  quitbutton->SetColors(Theme::Get("text-button"), Theme::Get("bg-button-negative"));

  subscriptions += startbutton->on_clicked.Subscribe([this](){
    if(!SCLang::IsRunning()){
      SCLang::Start(sclang_path, supernovachkbox->GetActive());
      startbutton->SetText("Stop SCLang");
    }else{
      SCLang::Stop();
      progressbar->SetAmount(0.0);
      statustext->SetText("AlgAudio (C) CeTA 2015, released on GNU LGPL 3");
      startbutton->SetText("Start AlgAudio");
    }
    statustext->SetTextColor("text-generic");
    statustext->SetBold(false);
  });
  subscriptions += SCLang::on_start_progress.Subscribe([this](int n, std::string msg){
    std::cout << "Progress " << n << std::endl;
    progressbar->SetAmount(n/10.0);
    statustext->SetText(msg);
  });

  subscriptions += testbutton->on_clicked.Subscribe([this](){
    std::cout << "COMPLETE" << std::endl;
    on_complete.Happen();
  });

  subscriptions += quitbutton->on_clicked.Subscribe([this](){
    on_close.Happen();
  });
  subscriptions += oscchkbox->on_toggled.Subscribe([](bool state){
    SCLang::SetOSCDebug(state);
  });
  subscriptions += debugchkbox->on_toggled.Subscribe([this](bool enabled){
    if(enabled){
      console = Console::Create();
      subscriptions += console->on_close.Subscribe([this](){
        debugchkbox->SetActive(false); // This will also unregister the window.
      });
      SDLMain::RegisterWindow(console);
      oscchkbox->SetVisible(true);
      testbutton->SetVisible(true);
    }else{
      SDLMain::UnregisterWindow(console);
      oscchkbox->SetVisible(false);
      testbutton->SetVisible(false);
      console = nullptr;
    }
  });
  subscriptions += SCLang::on_start_completed.Subscribe([this](bool success, std::string message){
    if(success){
      on_complete.Happen();
    }else{
      statustext->SetText(message);
      statustext->SetBold(true);
      statustext->SetTextColor("text-error");
      progressbar->SetAmount(0);
    }
  });
}

std::shared_ptr<LaunchConfigWindow> LaunchConfigWindow::Create(){
  auto res = std::shared_ptr<LaunchConfigWindow>( new LaunchConfigWindow());
  res->init();
  return res;
}

} // namespace AlgAudio
