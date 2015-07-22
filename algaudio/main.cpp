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
#include "LateReturn.hpp"
#include "MainWindow.hpp"

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
    auto s = signal1.Subscribe([](){
      std::cout << "1 temporary" << std::endl;
    });
    signal1.Happen();
  }
  signal1.Happen();
}

void TestSync(){
  Sync s(2);
  s.WhenAll([](){
    std::cout << "All!" << std::endl;
  });
  s.Trigger();
  s.Trigger();
}

int main(int argc, char *argv[]){
  (void)argc;
  (void)argv;
  try{
    //TestSubscriptions(); return 0;
    //TestSync(); return 0;
    Theme::Init();

    std::shared_ptr<MainWindow> mainwindow = nullptr;

    ModuleCollectionBase::InstallDir("modules");
    std::cout << ModuleCollectionBase::ListInstalledTemplates();
    std::shared_ptr<Module> module1, module2, console_module;
    std::shared_ptr<Canvas> main_canvas;
    ModuleFactory::CreateNewInstance("debug/console").Then([&](auto mod){
      console_module = mod;
    });

    auto configwindow = Window::Create("AlgAudio config",280,400);
    auto marginbox = configwindow->Create<UIMarginBox>(10,10,10,10);
    auto startbutton = configwindow->Create<UIButton>("Start SCLang");
    auto testbutton = configwindow->Create<UIButton>("Test button");
    auto quitbutton = configwindow->Create<UIButton>("Quit App");
    auto titlelabel = configwindow->Create<UILabel>("AlgAudio",52);
    auto configlabel = configwindow->Create<UILabel>("This place is left for config.");
    auto chkbox = configwindow->Create<UICheckbox>("Enable OSC debugging");
    auto supernovachkbox = configwindow->Create<UICheckbox>("Enable Supernova mode");
    auto mainvbox = UIVBox::Create(configwindow);
    auto buttonhbox = UIHBox::Create(configwindow);
    auto progressbar = UIProgressBar::Create(configwindow);
    auto statustext = UILabel::Create(configwindow,"AlgAudio (C) CeTA 2015, released on GNU LGPL 3",12);

    //mainvbox->SetPadding(10);
    configwindow->Insert(marginbox);
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

    startbutton->on_clicked.SubscribeForever([&](){
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
    SCLang::on_start_progress.SubscribeForever([&](int n, std::string msg){
      progressbar->SetAmount(n/10.0);
      statustext->SetText(msg);
    });
    testbutton->on_clicked.SubscribeForever([&](){
      if(!main_canvas) return;

      Sync s(2);
      LateAssign(module2, ModuleFactory::CreateNewInstance("base/stereoout")).ThenSync(s);
      LateAssign(module1, ModuleFactory::CreateNewInstance("base/sine")).ThenSync(s);
      s.WhenAll([&](){
        std::cout << "Both modules ready" << std::endl;
        main_canvas->InsertModule(module1);
        main_canvas->InsertModule(module2);
        //module1->SetParram("amp", 0.3);
        Module::Connect(module1->outlets[0], module2->inlets[0]);
        //Module::Connect(module1->outlets[0], module2->inlets[1]);
      });

    });
    quitbutton->on_clicked.SubscribeForever([&](){
      SDLMain::Quit();
    });
    chkbox->on_toggled.SubscribeForever([&](bool state){
      SCLang::SetOSCDebug(state);
    });
    configwindow->on_close.SubscribeForever([&](){
      // Let closing the config window close the whole app.
      SDLMain::Quit();
    });
    SCLang::on_start_completed.SubscribeForever([&](bool success, std::string message){
      if(success){
        main_canvas = Canvas::CreateEmpty();
        mainwindow = MainWindow::Create();
        mainwindow->on_close.SubscribeForever([&](){
          // Let closing the main window close the whole app.
          SDLMain::Quit();
        });
        SDLMain::UnregisterWindow(configwindow);
        configwindow = nullptr; // loose reference
        SDLMain::RegisterWindow(mainwindow);
      }else{
        statustext->SetText(message);
        statustext->SetBold(true);
        statustext->SetTextColor("text-error");
        progressbar->SetAmount(0);
      }
    });

    Utilities::global_idle.SubscribeForever([&](){
      SCLang::Poll();
    });

    SDLMain::RegisterWindow(configwindow);

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
