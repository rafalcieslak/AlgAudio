#ifndef CSIGNAL_HPP
#define CSIGNAL_HPP
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

#include <vector>
#include <list>
#include <map>
#include <functional>
#include <iostream>
#include <memory>

/*
 ==== A quick explanation of signal semantics ====

A signal represents an event happening from the application's point of view.
Clicking a button may be represented as a Signal. Getting reply from SCLang
may be represented as a Signal, etc.
Signals are usually publicly exposed to other classes. They can subscribe to a
Signal; a request to subscribe essentially means asking "please call this
function when this event happens". This way code can be easily bound to be
executed on user click. The class managing the signal decides when then event
happens, and triggers all subscriber functions by calling .Happen(). A Signal
can carry additional data, e.g. "toggle" signal of a checkbox will also carry
one boolean, the toggle state. That argument should be passed when .Happen()ing
the signal, and all subscribers will receive it.

There are 3 mores of subscription:

SubscribeOnce    - The function will be called the first time the signal happens
                    after it was subscribed, it will be automatically
                    unsubscribed afterwards.
SubscribeForever - The function will be called always from now on, there is
                    no way to unsibscribe. This is useful if you wish to react
                    on a signal repetivelly, but don't want to care about
                    releasing subscription.
Subscribe        - The standard way of subscribing. It returns a Subscription
                    instance. The time for which the function stays subscribed
                    is no longer then the time of life of that Subscription
                    object. This is useful if your calee function refers to an
                    instance of your class and makes no sense once the instance
                    is destroyed - simply store the Subscription as a field
                    of your class, and it will be unsubcsribed when your class
                    is destroyed.
You can also unsubscribe from a signal by calling .Release() on a Subscription.
This resets the Subscription to the default, empty state. A Subscription is
not copiable and not copy-constructible, but it is movable.

If you subscribed to a signal using SubscribeForever or SubscribeOnce, and the
signal gets destroyed, nothing wrong should happen, your calee function will
simply never get executed anymore. However, if a signal you subscribed to using
Subscribe is destroyed, then the calee function will never get called AND the
Subscription stays in invalid state - but that shouldn't matter, as it's always
safe to release a Subscription.

If your class has a lot of Subscription-type members which are used solely to
automatically unregister subscriptions when the class is destructed, you
may wish to inherit from SubsctiptionManager. This will inherit `subscriptions`
field, where new subscriptions can be added with a += operator:
  subscriptions += some_signal.Subscribe([](){ ... });

*/
namespace AlgAudio{

class SignalBase;

class Subscription{
public:
  Subscription() : id(0), target(nullptr) {}
  Subscription(Subscription&& other)
    : id(std::move(other.id)),
      target(std::move(other.target)) {
    other.id = 0;
  }
  Subscription& operator=(Subscription&& other) {
    id = std::move(other.id);
    target = std::move(other.target);
    other.id = 0;
    return *this;
  }
  ~Subscription(){ Release(); }
  inline bool IsEmpty() const { return id == 0;}
  void Release();
  template <typename...>
  friend class Signal;
  Subscription(const Subscription& other) = delete; // no copy-constructing
  Subscription& operator=(const Subscription& other) = delete; // no copy assignment
  friend class SignalBase;
private:
  Subscription(int id_, SignalBase* parent) : id(id_), target(parent) {}
  int id;
  // This field is used to track who is my parent, and who where should I
  // unsubscribe from when releasing. This cannot be a pointer, because the
  // signal might have been already destroyed and we would be left with an
  // invalid pointer. Therefore we store all signals in a global list, and
  // remove then when they are destroyed.
  // int parent_signal_id;
  SignalBase* target = nullptr;
};

class SignalBase{
protected:
  SignalBase();
  SignalBase(const SignalBase&) = delete;
  virtual ~SignalBase();
  SignalBase& operator=(const SignalBase&) = delete;
  std::list< std::shared_ptr<Subscription> > subscriptions;
  virtual void RemoveSubscriptionByID(int) = 0;
  static int subscription_id_counter;
  friend class Subscription;
};

template <typename... Types>
class Signal : public SignalBase{
private:
    std::list< std::function<void(Types...)> > subscribers_forever;
    std::map< int, std::function<void(Types...)> > subscribers_with_id;
    std::list< std::function<void(Types...)> > subscribers_once;
    void RemoveSubscriptionByID(int id) override{
      auto it = subscribers_with_id.find(id);
      if(it == subscribers_with_id.end()){
        std::cout << "WARNING: removing an unexisting subscription!" << std::endl;
      }else{
        std::cout << "WARNING: Erasing sub with id " << id << " size = "<< subscribers_with_id.size() << std::endl;
        subscribers_with_id.erase(it);
      }
    }
public:
    Signal<Types...>() {}
    ~Signal(){}
    Signal(const Signal& other) = delete;
    Signal(Signal&& other) = delete;
    Signal& operator=(const Signal& other) = delete;
    Signal& operator=(Signal&& other) = delete;
    std::shared_ptr<Subscription> Subscribe( std::function<void(Types...)> f ) __attribute__((warn_unused_result));
    void SubscribeForever( std::function<void(Types...)> f ) { subscribers_forever.push_back(f); }
    void SubscribeOnce( std::function<void(Types...)> f ) { subscribers_once.push_back(f); }
    template<class C>
    std::shared_ptr<Subscription> Subscribe( C* class_ptr, void (C::*member_ptr)(Types...) ) __attribute__((warn_unused_result));
    template<class C>
    void SubscribeForever( C* class_ptr, void (C::*member_ptr)(Types...)) { subscribers_forever.push_back( std::bind(member_ptr,class_ptr,std::placeholders::_1) ); }
    template<class C>
    void SubscribeOnce( C* class_ptr, void (C::*member_ptr)(Types...)) { subscribers_once.push_back( std::bind(member_ptr,class_ptr,std::placeholders::_1) ); }
    void Happen(Types... t) {
        // First, gather the list of all stuff to call. It has to be a temporary
        // local list, because the calees may modify some of the lists by
        // inserting new subscriptions.
        std::list< std::function<void(Types...)> > list_to_call;
        for(auto& f : subscribers_forever) list_to_call.push_back(f);
        for(auto& f : subscribers_once) list_to_call.push_back(f);
        for(auto& it : subscribers_with_id) list_to_call.push_back(it.second);
        // Clearing the list before calling calees, so that if they add new
        // one-time-subscriptions, they won't be lost.
        subscribers_once.clear();
        // Call everybody.
        for(auto f : list_to_call) f(t...);
    }
    friend class Subscription;
};

template <typename... Types>
std::shared_ptr<Subscription> __attribute__((warn_unused_result)) Signal<Types...>::Subscribe( std::function<void(Types...)> f ) {
  int sub_id = ++subscription_id_counter;
  subscribers_with_id[sub_id] = f;
  std::cout << "New SUB registered " << sub_id << std::endl;
  auto p = std::shared_ptr<Subscription>(new Subscription(sub_id, this));
  subscriptions.push_back(p);
  return p;
}
template <typename... Types> template <class C>
std::shared_ptr<Subscription> __attribute__((warn_unused_result)) Signal<Types...>::Subscribe( C* class_ptr, void (C::*member_ptr)(Types...)  ) {
  int sub_id = ++subscription_id_counter;
  subscribers_with_id[sub_id] = std::bind(member_ptr,class_ptr,std::placeholders::_1);
  std::cout << "New SUB registered " << sub_id << std::endl;
  auto p = std::shared_ptr<Subscription>(new Subscription(sub_id, this));
  subscriptions.push_back(p);
  return p;
}

class SubscriptionsManager{
public:
  class SubscriptionList{
  public:
    std::list<std::shared_ptr<Subscription>> list;
    void ReleaseAll() { list.clear(); }
    SubscriptionList& operator+=(std::shared_ptr<Subscription>&& s) {
      list.emplace_back(std::move(s));
      return *this;
    }
    SubscriptionList() : list(0){}
  };
  SubscriptionList subscriptions;
};



} //namespace AlgAudio

#endif //CSIGNAL_HPP
