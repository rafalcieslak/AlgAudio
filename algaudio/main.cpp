#include <iostream>
#include "ModuleCollection.hpp"
#include "ModuleFactory.hpp"
#include "SDLMain.hpp"
#include "UI/UIButton.hpp"
#include "UI/UIMarginBox.hpp"
#include "UI/UITextArea.hpp"
#include "UI/UIVBox.hpp"
#include "SCLang.hpp"

using namespace AlgAudio;

static const std::string sclang_path = "C:\\Program Files (x86)\\SuperCollider-3.6.6\\sclang.exe";

int main(){
  try{
    ModuleCollectionBase::InstallDir("modules");
    std::cout << ModuleCollectionBase::ListInstalledTemplates();
    auto module1 = ModuleFactory::CreateNewInstance("debug/helloworld");
    auto module2 = ModuleFactory::CreateNewInstance("debug/pipe");
    auto module3 = ModuleFactory::CreateNewInstance("debug/console");

    auto blah_window = Window::Create("BLAH",250,200);
    auto marginbox = blah_window->Create<UIMarginBox>(10,10,10,10);
    auto button1   = blah_window->Create<UIButton>("Start SCLang");
    auto button2   = blah_window->Create<UIButton>("Quit App");
    auto vbox = UIVBox::Create(blah_window);
    // Alternative syntax
    // auto button = UIButton::Create(blah_window,"Button");
    button1->on_clicked.Subscribe([&](){
      if(!SCLang::IsRunning()){
        SCLang::Start(sclang_path);
        button1->SetText("Stop SCLang");
      }else{
        SCLang::Stop();
        button1->SetText("Restart SCLang");
      }
    });
    blah_window->on_close.Subscribe([&](){
      // Let closing the main window close the whole app.
      SDLMain::Quit();
    });

    blah_window->Insert(marginbox);
    marginbox->Insert(button1);
    //marginbox->Insert(vbox);
    //vbox->Insert(button1,UIVBox::PackMode::TIGHT);
    //vbox->Insert(button2,UIVBox::PackMode::WIDE);

    SDLMain::RegisterWindow(blah_window);
    SDLMain::running = true;
    while(SDLMain::running){
      SDLMain::Step();
      SCLang::Poll();
    }

  }catch(Exception ex){
    std::cout << "An unhandled exception occured: " << ex.what() << std::endl;
  }
}
