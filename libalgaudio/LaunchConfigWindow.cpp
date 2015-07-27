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
#include "UI/UIButton.hpp"
#include "UI/UIMarginBox.hpp"
#include "UI/UITextArea.hpp"
#include "UI/UIBox.hpp"
#include "UI/UILabel.hpp"
#include "SCLang.hpp"
#include "Theme.hpp"
#include "Canvas.hpp"
#include "UI/UICheckbox.hpp"
#include "UI/UIProgressBar.hpp"
#include "MainWindow.hpp"

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
  auto marginbox = UIMarginBox::Create(shared_from_this(),10,10,10,10);
  auto startbutton = UIButton::Create(shared_from_this(),"Start SCLang");
  auto testbutton = UIButton::Create(shared_from_this(),"Test button");
  auto quitbutton = UIButton::Create(shared_from_this(),"Quit App");
  auto titlelabel = UILabel::Create(shared_from_this(),"AlgAudio",52);
  auto configlabel = UILabel::Create(shared_from_this(),"This place is left for config.");
  auto chkbox = UICheckbox::Create(shared_from_this(),"Enable OSC debugging");
  auto supernovachkbox = UICheckbox::Create(shared_from_this(),"Enable Supernova mode");
  auto mainvbox = UIVBox::Create(shared_from_this());
  auto buttonhbox = UIHBox::Create(shared_from_this());
  auto progressbar = UIProgressBar::Create(shared_from_this());
  auto statustext = UILabel::Create(shared_from_this(),"AlgAudio (C) CeTA 2015, released on GNU LGPL 3",12);

  //mainvbox->SetPadding(10);
  Insert(marginbox);
  marginbox->Insert(mainvbox);
  mainvbox->Insert(titlelabel, UIBox::PackMode::TIGHT);
  mainvbox->Insert(configlabel, UIBox::PackMode::WIDE);
  mainvbox->Insert(chkbox, UIBox::PackMode::TIGHT);
  mainvbox->Insert(supernovachkbox, UIBox::PackMode::TIGHT);
  mainvbox->Insert(buttonhbox, UIBox::PackMode::TIGHT);
  mainvbox->Insert(progressbar, UIBox::PackMode::TIGHT);
  mainvbox->Insert(statustext, UIBox::PackMode::TIGHT);
  buttonhbox->Insert(quitbutton, UIHBox::PackMode::WIDE);
  buttonhbox->Insert(testbutton, UIHBox::PackMode::WIDE);
  buttonhbox->Insert(startbutton, UIHBox::PackMode::WIDE);

  startbutton->SetColors(Theme::Get("text-button"), Theme::Get("bg-button-positive"));
  quitbutton->SetColors(Theme::Get("text-button"), Theme::Get("bg-button-negative"));

  subscriptions += startbutton->on_clicked.Subscribe([=](){
    if(!SCLang::IsRunning()){
      SCLang::Start(sclang_path, supernovachkbox->active);
      startbutton->SetText("Stop SCLang");
    }else{
      SCLang::Stop();
      progressbar->SetAmount(0.0);
      statustext->SetText("AlgAudio (C) CeTA 2015, released on GNU LGPL 3");
      startbutton->SetText("Start SCLang");
    }
    statustext->SetTextColor("text-generic");
    statustext->SetBold(false);
  });
  subscriptions += SCLang::on_start_progress.Subscribe([=](int n, std::string msg){
    progressbar->SetAmount(n/10.0);
    statustext->SetText(msg);
  });
  subscriptions += testbutton->on_clicked.Subscribe([=](){
    std::cout << "COMPLETE" << std::endl;
    on_complete.Happen();
  });
  subscriptions += quitbutton->on_clicked.Subscribe([=](){
    on_close.Happen();
  });
  subscriptions += chkbox->on_toggled.Subscribe([](bool state){
    SCLang::SetOSCDebug(state);
  });
  subscriptions += SCLang::on_start_completed.Subscribe([=](bool success, std::string message){
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
