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
#include <iostream>
#include <SDL2/SDL.h>
#include "ModuleCollection.hpp"
#include "ModuleFactory.hpp"
#include "SDLMain.hpp"
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

using namespace AlgAudio;

#ifdef __unix__
  static const std::string sclang_path = "/usr/bin/sclang";
  static const std::string scsynth_path = "/usr/bin/scsynth";
#else
  static const std::string sclang_path = "C:\\Program Files (x86)\\SuperCollider-3.6.6\\sclang.exe";
  static const std::string scsynth_path = "C:\\Program Files (x86)\\SuperCollider-3.6.6\\scsynth.exe";
#endif

void TestSubscriptions(){
  Signal<> signal1;
  signal1.SubscribeForever([](){
    std::cout << "1 for" << std::endl;
  });
  {
    Subscription s = signal1.Subscribe([](){
      std::cout << "1 temporary" << std::endl;
    });
    signal1.Happen();
  }
  signal1.Happen();
}

int main(int argc, char *argv[]){
  (void)argc;
  (void)argv;
  try{
    //TestSubscriptions(); return 0;
    Theme::Init();

    ModuleCollectionBase::InstallDir("modules");
    std::cout << ModuleCollectionBase::ListInstalledTemplates();
    std::shared_ptr<Module> module1, module2, console_module;
    std::shared_ptr<Canvas> main_canvas;
    console_module = ModuleFactory::CreateNewInstance("debug/console");

    auto mainwindow = Window::Create("AlgAudio config",280,400);
    auto marginbox = mainwindow->Create<UIMarginBox>(10,10,10,10);
    auto startbutton = mainwindow->Create<UIButton>("Start SCLang");
    auto testbutton = mainwindow->Create<UIButton>("Test button");
    auto quitbutton = mainwindow->Create<UIButton>("Quit App");
    auto titlelabel = mainwindow->Create<UILabel>("AlgAudio",52);
    auto configlabel = mainwindow->Create<UILabel>("This place is left for config.");
    auto chkbox = mainwindow->Create<UICheckbox>("Enable OSC debugging");
    auto mainvbox = UIVBox::Create(mainwindow);
    auto buttonhbox = UIHBox::Create(mainwindow);
    auto progressbar = UIProgressBar::Create(mainwindow);
    auto statustext = UILabel::Create(mainwindow,"AlgAudio (C) CeTA 2015, released on GNU LGPL 3",10);

    //mainvbox->SetPadding(10);
    mainwindow->Insert(marginbox);
    marginbox->Insert(mainvbox);
    mainvbox->Insert(titlelabel, UIBox::PackMode::TIGHT);
    mainvbox->Insert(configlabel, UIBox::PackMode::WIDE);
    mainvbox->Insert(chkbox, UIBox::PackMode::TIGHT);
    mainvbox->Insert(buttonhbox, UIBox::PackMode::TIGHT);
    mainvbox->Insert(progressbar, UIBox::PackMode::TIGHT);
    mainvbox->Insert(statustext, UIBox::PackMode::TIGHT);
    buttonhbox->Insert(quitbutton, UIHBox::PackMode::WIDE);
    buttonhbox->Insert(testbutton, UIHBox::PackMode::WIDE);
    buttonhbox->Insert(startbutton, UIHBox::PackMode::WIDE);

    startbutton->SetColors(Theme::Get("text-button"), Theme::Get("bg-button-positive"));
    quitbutton->SetColors(Theme::Get("text-button"), Theme::Get("bg-button-negative"));

    startbutton->on_clicked.SubscribeForever([&](){
      if(!SCLang::IsRunning()){
        SCLang::Start(sclang_path);
        startbutton->SetText("Stop SCLang");
      }else{
        SCLang::Stop();
        progressbar->SetAmount(0.0);
        statustext->SetText("AlgAudio (C) CeTA 2015, released on GNU LGPL 3");
        startbutton->SetText("Start SCLang");
      }
    });
    SCLang::on_start_progress.SubscribeForever([&](int n, std::string msg){
      progressbar->SetAmount(n/8.0);
      statustext->SetText(msg);
    });
    testbutton->on_clicked.SubscribeForever([&](){
      module1 = nullptr;
      module2 = nullptr;
      console_module = nullptr;
      if(!main_canvas) return;
      module1 = ModuleFactory::CreateNewInstance("base/simplein");
      module2 = ModuleFactory::CreateNewInstance("base/simpleout");
      main_canvas->InsertModule(module1);
      main_canvas->InsertModule(module2);
    });
    quitbutton->on_clicked.SubscribeForever([&](){
      SDLMain::Quit();
    });
    chkbox->on_toggled.SubscribeForever([&](bool state){
      SCLang::SetOSCDebug(state);
    });
    mainwindow->on_close.SubscribeForever([&](){
      // Let closing the main window close the whole app.
      SDLMain::Quit();
    });
    SCLang::on_start_completed.SubscribeForever([&](){
      // Pretend we are creating new instances
      ModuleCollectionBase::InstallAllTemplatesIntoSC();
      //SCLang::DebugQueryInstalled();
      main_canvas = Canvas::CreateEmpty();
    });

    Utilities::global_idle.SubscribeForever([&](){
      SCLang::Poll();
    });

    SDLMain::RegisterWindow(mainwindow);

    SDLMain::Loop();

    SCLang::Stop();
    // SDL seems to have problems when the destroy functions are called from
    // the DLL destroy call. Thus, we explicitly free all our window pointers
    // before the global cleanup.
    SDLMain::UnregisterAll();

  }catch(Exception ex){
    std::cout << "An unhandled exception occured: " << ex.what() << std::endl;
  }
  return 0;
}
