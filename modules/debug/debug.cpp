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
#include "UI/UIButton.hpp"
#include "UI/UIBox.hpp"
#include "Theme.hpp"
#include "SDLMain.hpp"
#include "SCLang.hpp"
#include "ModuleUI/ModuleGUI.hpp"

// The custom class NEVER takes ownership of the instances
class HelloWorld : public AlgAudio::Module{
public:
  void on_init(){
    std::cout << "~~~ Hello world! ~~~" << std::endl;
  }
  void on_destroy(){
    std::cout << "~~~ Bye world! ~~~" << std::endl;
  }
};

//
class Window : public AlgAudio::Module{
public:
  void on_init(){
    my_window = AlgAudio::Window::Create("My window!", 500, 50);
    AlgAudio::SDLMain::RegisterWindow(my_window);
  }
  void on_destroy(){
    AlgAudio::SDLMain::UnregisterWindow(my_window);
    my_window = nullptr;
  }
  // This demonstrates how you can modify the automatically built module GUI
  virtual void on_gui_build(std::shared_ptr<AlgAudio::ModuleGUI> gui) {
    gui->SetCustomSize(AlgAudio::Size2D(100,100));
  };
  std::shared_ptr<AlgAudio::Window> my_window;
};


// ----- The console module -----

class Console : public AlgAudio::Module{
public:
  std::shared_ptr<AlgAudio::Window> console_window;
  void on_init(){
    console_window = AlgAudio::Window::Create("SCLang console", 500, 400, false);
    auto mainvbox = AlgAudio::UIVBox::Create(console_window);
    auto textarea = AlgAudio::UITextArea::Create(console_window, AlgAudio::Color(255,255,255), AlgAudio::Color(0,0,0));
    auto clipboard_button = AlgAudio::UIButton::Create(console_window, "Copy console output to clipboard");
    textarea->SetBottomAligned(true);
    clipboard_button->SetFontSize(13);
    clipboard_button->SetColors(AlgAudio::Theme::Get("text-generic"),AlgAudio::Theme::Get("bg-button-neutral"));

    mainvbox->Insert(textarea, AlgAudio::UIBox::PackMode::WIDE);
    mainvbox->Insert(clipboard_button, AlgAudio::UIBox::PackMode::TIGHT);
    console_window->Insert(mainvbox);

    console_window->subscriptions += AlgAudio::SCLang::on_line_received.Subscribe([=](std::string line){
      textarea->PushLine(line);
    });
    subscriptions += clipboard_button->on_clicked.Subscribe([=](){
      AlgAudio::Utilities::CopyToClipboard(textarea->GetAllText());
    });
    // When subscribing to signals, be careful not to create shared_ptr loops.
    // In the example below, if console_window was captured by value, it would
    // copy the shared_ptr, increase refcount and store it INSIDE console window
    // (lambda stored inside signal), and in such case the shared_ptr would
    // never free.
    console_window->on_close.SubscribeForever([&](){
      AlgAudio::SDLMain::UnregisterWindow(console_window);
    });
    AlgAudio::SDLMain::RegisterWindow(console_window);
  }
  void on_destroy(){
    AlgAudio::SDLMain::UnregisterWindow(console_window);
    console_window = nullptr;
  }
};
// ------------------------------

class Measure : public AlgAudio::Module{
public:
  void on_reply(std::string id, float val){
    std::cout << "Measure got a reply: " << id << " " << val << std::endl;
  }
};

// --------------------------------


extern "C"{
void delete_instance(void* obj){
  delete reinterpret_cast<AlgAudio::DynamicallyLoadableClass*>(obj);
}
void* create_instance(const char* name){
  if(strcmp(name,"HelloWorld")==0) return new HelloWorld();
  if(strcmp(name,"Window")==0) return new Window();
  if(strcmp(name,"Console")==0) return new Console();
  if(strcmp(name,"Measure")==0) return new Measure();
  else return nullptr;
}
} // extern C
