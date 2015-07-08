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
    auto module3 = ModuleFactory::CreateNewInstance("debug/window");

    auto blah_window = UIWindow::Create("BLAH");
    auto marginbox = UIMarginBox::Create(100,50,20,5);
    auto button = UIButton::Create("Button");
    
    blah_window->Insert(marginbox);
    marginbox->Insert(button);

    SDLMain::RegisterWindow(blah_window);
    SDLMain::Run();

  }catch(Exception ex){
    std::cout << "An unhandled exception occured: " << ex.what() << std::endl;
  }
}
