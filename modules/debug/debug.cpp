#include "Module.hpp"
#include <iostream>
#include <cstring>

#include "Window.hpp"
#include "SDLMain.hpp"

// The custom class NEVER takes ownership of the instances
class HelloWorld : public AlgAudio::Module{
public:
  void on_init(){
    std::cout << "~~~ Hello world! ~~~" << std::endl;
  }
};


class Window : public AlgAudio::Module{
public:
  void on_init(){
    my_window = AlgAudio::Window::Create("My window!", 500, 50);
    AlgAudio::SDLMain::RegisterWindow(my_window);
  }
  std::shared_ptr<AlgAudio::Window> my_window;
};


extern "C"{
void delete_instance(void* obj){
  delete reinterpret_cast<AlgAudio::DynamicallyLoadableClass*>(obj);
}
void* create_instance(const char* name){
  if(strcmp(name,"HelloWorld")==0) return new HelloWorld();
  if(strcmp(name,"Window")==0) return new Window();
  else return nullptr;
}
} // extern C
