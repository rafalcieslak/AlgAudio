#include <iostream>
#include "ModuleCollection.hpp"
#include "ModuleFactory.hpp"
#include "SDLMain.hpp"
#include "UI/UIButton.hpp"
#include "UI/UIMarginBox.hpp"
#include "UI/UITextArea.hpp"
#include "UI/UIBox.hpp"
#include "UI/UILabel.hpp"
#include "SCLang.hpp"

using namespace AlgAudio;

#ifdef __unix__
  static const std::string sclang_path = "/usr/bin/sclang";
#else
  static const std::string sclang_path = "C:\\Program Files (x86)\\SuperCollider-3.6.6\\sclang.exe";
#endif

int main(){
  try{
    ModuleCollectionBase::InstallDir("modules");
    std::cout << ModuleCollectionBase::ListInstalledTemplates();
    auto module1 = ModuleFactory::CreateNewInstance("debug/helloworld");
    auto module2 = ModuleFactory::CreateNewInstance("debug/pipe");
    auto module3 = ModuleFactory::CreateNewInstance("debug/console");

    auto mainwindow = Window::Create("AlgAudio config",280,400);
    auto marginbox = mainwindow->Create<UIMarginBox>(10,10,10,10);
    auto startbutton = mainwindow->Create<UIButton>("Start SCLang");
    auto quitbutton = mainwindow->Create<UIButton>("Quit App");
    auto titlelabel = mainwindow->Create<UILabel>("AlgAudio",36);
    auto configlabel = mainwindow->Create<UILabel>("This place is left for config.");
    auto mainvbox = UIVBox::Create(mainwindow);
    auto buttonhbox = UIHBox::Create(mainwindow);

    mainvbox->SetPadding(10);
    mainwindow->Insert(marginbox);
    marginbox->Insert(mainvbox);
    mainvbox->Insert(titlelabel, UIBox::PackMode::TIGHT);
    mainvbox->Insert(configlabel, UIBox::PackMode::WIDE);
    mainvbox->Insert(buttonhbox, UIBox::PackMode::TIGHT);
    buttonhbox->Insert(quitbutton, UIHBox::PackMode::WIDE);
    buttonhbox->Insert(startbutton, UIHBox::PackMode::WIDE);

    startbutton->on_clicked.Subscribe([&](){
      if(!SCLang::IsRunning()){
        SCLang::Start(sclang_path);
        startbutton->SetText("Stop SCLang");
      }else{
        SCLang::Stop();
        startbutton->SetText("Restart SCLang");
      }
    });
    mainwindow->on_close.Subscribe([&](){
      // Let closing the main window close the whole app.
      SDLMain::Quit();
    });

    SDLMain::RegisterWindow(mainwindow);
    SDLMain::running = true;
    while(SDLMain::running){
      SDLMain::Step();
      SCLang::Poll();
    }

    SCLang::Stop();
    // SDL seems to have problems when the destroy functions are called from
    // the DLL destroy call. Thus, we explicitly free all our window pointers
    // before the global cleanup.
    SDLMain::UnregisterAll();

  }catch(Exception ex){
    std::cout << "An unhandled exception occured: " << ex.what() << std::endl;
  }
}
