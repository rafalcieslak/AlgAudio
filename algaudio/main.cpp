#include <iostream>
#include "ModuleCollection.hpp"
#include "ModuleFactory.hpp"
#include "SDLMain.hpp"
#include "UI/UIButton.hpp"

using namespace AlgAudio;

int main(){
  try{
    ModuleCollectionBase::InstallDir("modules");
    std::cout << ModuleCollectionBase::ListInstalledTemplates();
    auto module1 = ModuleFactory::CreateNewInstance("debug/helloworld");
    auto module2 = ModuleFactory::CreateNewInstance("debug/pipe");

    auto blah_window = std::make_shared<UIWindow>("BLAH");
    SDLMain::RegisterWindow(blah_window);
    auto button = std::make_shared<UIButton>("Button");
    blah_window->Insert(button);

    SDLMain::Run();

  }catch(Exception ex){
    std::cout << "An unhandled exception occured: " << ex.what() << std::endl;
  }
}
