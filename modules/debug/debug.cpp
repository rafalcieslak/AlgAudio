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
#include "Module.hpp"
#include <iostream>
#include <cstring>

#include "Window.hpp"
#include "UI/UITextArea.hpp"
#include "SDLMain.hpp"
#include "SCLang.hpp"

// The custom class NEVER takes ownership of the instances
class HelloWorld : public AlgAudio::Module{
public:
  void on_init(){
    std::cout << "~~~ Hello world! ~~~" << std::endl;
  }
};

//
class Window : public AlgAudio::Module{
public:
  void on_init(){
    my_window = AlgAudio::Window::Create("My window!", 500, 50);
    AlgAudio::SDLMain::RegisterWindow(my_window);
  }
  std::shared_ptr<AlgAudio::Window> my_window;
};


// ----- The console module -----

class Console : public AlgAudio::Module{
public:
  void on_init(){
    std::shared_ptr<AlgAudio::Window> console_window = AlgAudio::Window::Create("SCLang console", 500, 400);
    auto textarea = AlgAudio::UITextArea::Create(console_window, AlgAudio::Color(255,255,255), AlgAudio::Color(0,0,0));
    textarea->SetBottomAligned(true);
    subscriptions += AlgAudio::SCLang::on_line_received.Subscribe([=](std::string line){
      textarea->PushLine(line);
    });
    console_window->on_close.SubscribeForever([&](){
      AlgAudio::SDLMain::UnregisterWindow(console_window);
    });
    console_window->Insert(textarea);
    AlgAudio::SDLMain::RegisterWindow(console_window);
  }
};
// ------------------------------

extern "C"{
void delete_instance(void* obj){
  delete reinterpret_cast<AlgAudio::DynamicallyLoadableClass*>(obj);
}
void* create_instance(const char* name){
  if(strcmp(name,"HelloWorld")==0) return new HelloWorld();
  if(strcmp(name,"Window")==0) return new Window();
  if(strcmp(name,"Console")==0) return new Console();
  else return nullptr;
}
} // extern C
