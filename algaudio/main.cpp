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
#include "LaunchConfigWindow.hpp"
#include "MainWindow.hpp"
#include "SDLMain.hpp"
#include "SCLang.hpp"
#include "Version.hpp"

using namespace AlgAudio;

Relay<int> r;
/*
LateReturn<int> CheckFunc(){
  r.LateThrow<Exceptions::Unimplemented>("Blah.");
  return r;
}

LateReturn<int> CheckFuncB(){
  Relay<int> a;
  CheckFunc().Then([a](auto i){
    a.Return(i);
  }).Catch(a);
  return a;
}

void LRCatchCheck(){
  CheckFuncB().Then([](int x){
    std::cout << x << std::endl;
  }).Catch<Exceptions::Unimplemented>([](auto ex){
    std::cout << "EXCEPTION " << ex->what() << std::endl;
  });
  r.Return(5);
};
*/
int main(int argc, char *argv[]){
  (void)argc;
  (void)argv;
  
  //LRCatchCheck(); return 0;

  std::cout << "Algaudio " << ALGAUDIO_VERSION << " starting." << std::endl;

  try{
    Theme::Init();
    SDLMain::Init();

    // TODO: Load modules later, with SC start.
    ModuleCollectionBase::InstallDir("modules");
    std::cout << ModuleCollectionBase::ListInstalledTemplates();

    std::shared_ptr<MainWindow> mainwindow = nullptr;
    auto configwindow = LaunchConfigWindow::Create();
    configwindow->on_close.SubscribeForever([&](){
      // Let closing the config window close the whole app.
      SDLMain::Quit();
    });
    configwindow->on_complete.SubscribeForever([&](){
      // When the config was completed, create the main window.
      mainwindow = MainWindow::Create();
      mainwindow->on_close.SubscribeForever([&](){
        // Let closing the main window close the whole app.
        SDLMain::Quit();
      });
      SDLMain::UnregisterWindow(configwindow);
      configwindow = nullptr; // loose reference
      SDLMain::RegisterWindow(mainwindow);
    });

    SDLMain::RegisterWindow(configwindow);

    SDLMain::Loop();

    SCLang::Stop();
    // SDL seems to have problems when the destroy functions are called from
    // the DLL destroy call. Thus, we explicitly free all our window pointers
    // before the global cleanup.
    SDLMain::UnregisterAll();

  }catch(Exceptions::Exception ex){
    std::cout << "An unhandled exception occured: " << ex.what() << std::endl;
  }
  return 0;
}
