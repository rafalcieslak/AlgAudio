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
#include "UI/UICheckbox.hpp"

using namespace AlgAudio;

#ifdef __unix__
  static const std::string sclang_path = "/usr/bin/sclang";
#else
  static const std::string sclang_path = "C:\\Program Files (x86)\\SuperCollider-3.6.6\\sclang.exe";
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
    auto module1 = ModuleFactory::CreateNewInstance("debug/helloworld");
    auto module2 = ModuleFactory::CreateNewInstance("debug/pipe");
    auto module3 = ModuleFactory::CreateNewInstance("debug/console");

    auto mainwindow = Window::Create("AlgAudio config",280,400);
    auto marginbox = mainwindow->Create<UIMarginBox>(10,10,10,10);
    auto startbutton = mainwindow->Create<UIButton>("Start SCLang");
    auto oscbutton = mainwindow->Create<UIButton>("OSC test");
    auto quitbutton = mainwindow->Create<UIButton>("Quit App");
    auto titlelabel = mainwindow->Create<UILabel>("AlgAudio",36);
    auto configlabel = mainwindow->Create<UILabel>("This place is left for config.");
    auto chkbox = mainwindow->Create<UICheckbox>("Enable OSC debugging");
    auto mainvbox = UIVBox::Create(mainwindow);
    auto buttonhbox = UIHBox::Create(mainwindow);

    mainvbox->SetPadding(10);
    mainwindow->Insert(marginbox);
    marginbox->Insert(mainvbox);
    mainvbox->Insert(titlelabel, UIBox::PackMode::TIGHT);
    mainvbox->Insert(configlabel, UIBox::PackMode::WIDE);
    mainvbox->Insert(chkbox, UIBox::PackMode::TIGHT);
    mainvbox->Insert(buttonhbox, UIBox::PackMode::TIGHT);
    buttonhbox->Insert(quitbutton, UIHBox::PackMode::WIDE);
    buttonhbox->Insert(oscbutton, UIHBox::PackMode::WIDE);
    buttonhbox->Insert(startbutton, UIHBox::PackMode::WIDE);

    startbutton->SetColors(Theme::Get("text-button"), Theme::Get("bg-button-positive"));
    quitbutton->SetColors(Theme::Get("text-button"), Theme::Get("bg-button-negative"));

    startbutton->on_clicked.SubscribeForever([&](){
      if(!SCLang::IsRunning()){
        SCLang::Start(sclang_path);
        startbutton->SetText("Stop SCLang");
      }else{
        SCLang::Stop();
        startbutton->SetText("Restart SCLang");
      }
    });
    oscbutton->on_clicked.SubscribeForever([&](){
      ModuleCollectionBase::InstallAllTemplatesIntoSC();
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
