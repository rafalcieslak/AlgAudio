#include "Module.hpp"
#include <iostream>
#include <cstring>

#include "Window.hpp"
#include "UI/UITextArea.hpp"
#include "SDLMain.hpp"
#include "Utilities.hpp"
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
  std::shared_ptr<AlgAudio::Window> console_window;
  void on_init(){
    console_window = AlgAudio::Window::Create("SCLang console", 500, 400);
    auto textarea = AlgAudio::UITextArea::Create(console_window, AlgAudio::Color(255,255,255), AlgAudio::Color(0,0,0));
    textarea->SetBottomAligned(true);
    AlgAudio::SCLang::on_line_received.Subscribe([=](std::string line){
      textarea->PushLine(line);
    });
    console_window->on_close.Subscribe([&](){
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
