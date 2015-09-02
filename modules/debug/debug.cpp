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

#include "Console.hpp"
#include "ModuleUI/UISlider.hpp"
#include "Theme.hpp"
#include "SDLMain.hpp"
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
  std::shared_ptr<AlgAudio::Console> console;
  void on_init(){
    console = AlgAudio::Console::Create();
    console->on_close.SubscribeForever([&](){
      AlgAudio::SDLMain::UnregisterWindow(console);
    });
    AlgAudio::SDLMain::RegisterWindow(console);
  }
  void on_destroy(){
    AlgAudio::SDLMain::UnregisterWindow(console);
    console = nullptr;
  }
};

// --------------------------------
class GUIDemo : public AlgAudio::Module{
public:
  void on_param_set(std::string, float val) override{
    //std::cout << "GUIDemo executes some custom code on param set! " << id << " " << val << std::endl;
    auto controller = GetParamControllerByID("stdout2");
    controller->Set(val*5);
  }
  void on_gui_build(std::shared_ptr<AlgAudio::ModuleGUI> gui) override{
    auto slider = std::dynamic_pointer_cast<AlgAudio::UISlider>( gui->FindChild(AlgAudio::UIWidget::ID("gainslider")) );
    if(!slider) std::cout << "Oops, our child widget was not found?" << std::endl;
    else slider->SetName("Custom name");
  }
};

// --------  PORTALS --------------
class PortalBase : public AlgAudio::Module{
public:
  // Map from channel number, to a pair of bus pointer, and a list of all portals that use that channel.
  static std::map<int, std::pair<std::shared_ptr<AlgAudio::Bus>, std::list<std::shared_ptr<PortalBase>>> > channels;
  int current_channel = -1;
  void on_init() override{
    AttachToChannel(0);
  }
  void on_destroy() override{
    DetachFromChannel();
  }
  void AttachToChannel(int n){
    std::cout << "Portal attaching to channel " << n << std::endl;
    auto it = channels.find(n);
    if(it == channels.end()){
      // This channel is not yet used. Create it.
      std::list<std::shared_ptr<PortalBase>> list;
      list.push_back( std::static_pointer_cast<PortalBase>(shared_from_this()) );
      channels[n] = {nullptr, list};
      AlgAudio::Bus::CreateNew().Then( [n](std::shared_ptr<AlgAudio::Bus> b){
        OnBusReady(b, n);
      });
    }else{
      // This channel already exists. Append this module to the channel.
      auto& pair = it->second;
      if(pair.first != nullptr) // If the bus for this channel already arrived
        SetChannelBusID( pair.first->GetID() );
      pair.second.push_back( std::static_pointer_cast<PortalBase>(shared_from_this()) );
    }
    current_channel = n;
  }
  void DetachFromChannel(){
    std::cout << "Portal detaching from channel " << std::endl;
    auto it = channels.find(current_channel);
    if(it == channels.end()){
      std::cout << "ERROR: Unable to detach a portal from a bus, the channel entry does not exist" << std::endl;
      return;
    }
    it->second.second.remove( std::static_pointer_cast<PortalBase>(shared_from_this()) );
    if( it->second.second.size() == 0){
      channels.erase(it); // This will also loose the last reference to the bus, effectivelly freeing it.
    }
  }
  void SetChannelBusID(int id){
    GetParamControllerByID("portalbus")->Set(id);
  }
  static void OnBusReady(std::shared_ptr<AlgAudio::Bus> b, int channel);
  void on_param_set(std::string s, float val) override{
    if(s == "channel"){
      int new_channel = val;
      if(new_channel == current_channel) return;
      DetachFromChannel();
      AttachToChannel(new_channel);
    }
  }
private:
};

void PortalBase::OnBusReady(std::shared_ptr<AlgAudio::Bus> b, int channel){
  auto it = channels.find(channel);
  if(it == channels.end()) return;
  it->second.first = b;
  for(auto portal_ptr : it->second.second){
    portal_ptr->SetChannelBusID(b->GetID());
  }
}

std::map<int, std::pair<std::shared_ptr<AlgAudio::Bus>, std::list<std::shared_ptr<PortalBase>>> > PortalBase::channels;


extern "C"{
void delete_instance(void* obj){
  delete reinterpret_cast<AlgAudio::Module*>(obj);
}
void* create_instance(const char* name){
  if(strcmp(name,"HelloWorld")==0) return new HelloWorld();
  if(strcmp(name,"Window")==0) return new Window();
  if(strcmp(name,"Console")==0) return new Console();
  if(strcmp(name,"GUIDemo")==0) return new GUIDemo();
  if(strcmp(name,"PortalBase")==0) return new PortalBase();
  else return nullptr;
}
} // extern C
