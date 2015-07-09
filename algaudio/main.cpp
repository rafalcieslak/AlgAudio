#include <iostream>
#include "ModuleCollection.hpp"
#include "ModuleFactory.hpp"
#include "SDLMain.hpp"
#include "UI/UIButton.hpp"
#include "UI/UIMarginBox.hpp"
#include "SCLauncher.hpp"

using namespace AlgAudio;

int main(){
  try{
    ModuleCollectionBase::InstallDir("modules");
    std::cout << ModuleCollectionBase::ListInstalledTemplates();
    auto module1 = ModuleFactory::CreateNewInstance("debug/helloworld");
    auto module2 = ModuleFactory::CreateNewInstance("debug/pipe");
    //auto module3 = ModuleFactory::CreateNewInstance("debug/window");

    SCLauncher::Start();

    auto blah_window = Window::Create("BLAH",400,300);
    auto marginbox = blah_window->Create<UIMarginBox>(100,200,40,110);
    auto button    = blah_window->Create<UIButton>("This is a button");
    // Alternative syntax
    // auto button = UIButton::Create(blah_window,"Button");
    button->on_clicked.Subscribe([&](){
      std::cout << "Button clicked!" << std::endl;
      button->SetText("Tada!");
    });

    blah_window->Insert(marginbox);
    marginbox->Insert(button);

    SDLMain::RegisterWindow(blah_window);
    SDLMain::Run();

  }catch(Exception ex){
    std::cout << "An unhandled exception occured: " << ex.what() << std::endl;
  }
  SCLauncher::Stop();
}
