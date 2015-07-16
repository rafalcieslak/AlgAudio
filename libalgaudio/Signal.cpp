#include "Signal.hpp"
#include <iostream>

namespace AlgAudio{

int SignalBase::id_counter = 1;
int SignalBase::subscription_id_counter = 1;

void Subscription::Release(){
  if(!IsEmpty()){
    auto it = SignalBase::all_signals().find(parent_signal_id);
    if(it == SignalBase::all_signals().end()){
      // Huh? Our signal was already destroyed. Okay then, we are free.
    }else{
      it->second->RemoveSubscriptionByID(id);
    }
    id = 0;
  }
}

SignalBase::SignalBase(){
  my_id = ++id_counter;
  //std::cout << "Creating signal " <<  my_id << std::endl;
  all_signals()[my_id] = this;
}

SignalBase::~SignalBase(){
  //std::cout << "Destroying signal " <<  my_id << std::endl;
  auto it = all_signals().find(my_id);
  if(it == all_signals().end()){
    std::cout << "ERROR: Removing a signal which is not present in the all_signals list!" << std::endl;
  }
  all_signals().erase(it);
}

std::map<int, SignalBase*>& SignalBase::all_signals(){
  // Construct Members On First Use, see
  // https://isocpp.org/wiki/faq/ctors#static-init-order-on-first-use
  static std::map<int, SignalBase*>* q = new std::map<int, SignalBase*>();
  return *q;
}


} // namespace AlgAudio
