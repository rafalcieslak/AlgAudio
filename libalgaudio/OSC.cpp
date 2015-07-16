#include "OSC.hpp"
#include <iostream>

namespace AlgAudio{

int OSC::msg_id = 0;

OSC::OSC(std::string address, std::string port) : addr(""){
  server = std::make_unique<lo::ServerThread>(2542);
  if(!server->is_valid()){
    server = std::make_unique<lo::ServerThread>(2543);
    if(!server->is_valid()){
      server = std::make_unique<lo::ServerThread>(2544);
    }
  }

  server->add_method("/algaudio/reply", NULL, [&](lo_message msg){
    std::lock_guard<std::recursive_mutex> lock(osc_mutex);
    int n = lo_message_get_argc(msg);
    int id = lo_message_get_argv(msg)[n-1]->i32;
    auto it = waiting_for_reply.find(id);
    if(it == waiting_for_reply.end()){
      // nobody waiting for this reply
      return;
    }
    replies_to_call.push_back( std::bind( it->second, lo::Message(msg) ) );
    waiting_for_reply.erase(it);
  });

  server->start();

  // The constructor for lo::Address is messed up. Creating an lo_address
  // manually and binding it to lo::Address fixes the issue.
  lo_address a = lo_address_new(address.c_str(), port.c_str());
  addr = lo::Address(a,true);
}

OSC::~OSC(){
  server->stop();
}

void OSC::Send(std::string a){
  addr.send(a);
}
void OSC::Send(std::string a, lo::Message m){
  msg_id++;
  m.add_int32(msg_id);
  addr.send(a,m);
}
void OSC::Send(std::string a, std::function<void(lo::Message)> reply_action, lo::Message m){
  msg_id++;
  m.add_int32(msg_id);
  osc_mutex.lock();
  waiting_for_reply[msg_id] = reply_action;
  osc_mutex.unlock();
  addr.send(a,m);
}

void OSC::TriggerReplies(){
  osc_mutex.lock();
  for(auto& r : replies_to_call) r();
  replies_to_call.clear();
  osc_mutex.unlock();
}

}
