#include <iostream>
#include "ModuleCollection.hpp"
#include "ModuleFactory.hpp"
#include "SDLMain.hpp"
#include "UI/UIButton.hpp"
#include "UI/UIMarginBox.hpp"

using namespace AlgAudio;

int main(){
  try{
    ModuleCollectionBase::InstallDir("modules");
    std::cout << ModuleCollectionBase::ListInstalledTemplates();
    auto module1 = ModuleFactory::CreateNewInstance("debug/helloworld");
    auto module2 = ModuleFactory::CreateNewInstance("debug/pipe");
    //auto module3 = ModuleFactory::CreateNewInstance("debug/window");

    auto blah_window = UIWindow::Create("BLAH",400,200);
    auto marginbox = blah_window->Create<UIMarginBox>(100,50,20,5);
    auto button    = blah_window->Create<UIButton>("Button");
    // Alternative syntax
    // auto button = UIButton::Create(blah_window,"Button");
    button->on_clicked.Subscribe([](){
      std::cout << "Button clicked!" << std::endl;
    });

    blah_window->Insert(marginbox);
    marginbox->Insert(button);

    SDLMain::RegisterWindow(blah_window);
    SDLMain::Run();

  }catch(Exception ex){
    std::cout << "An unhandled exception occured: " << ex.what() << std::endl;
  }
}
