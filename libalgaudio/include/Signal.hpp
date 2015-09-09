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

namespace AlgAudio{

class SignalBase;

/** A valid Subscription is created by calling Sinnal::Subscribe. The main 
 *  purpose of this class is to automatically unsibscribe the function when
 *  this Subscription is destructed. This way it's easy to make sure that 
 *  a subscribed function won't be called after your class instance was
 *  destructed - simply keep the Subscription as a member of that class, or
 *  use a SubscriptionsManager.
 *
 *  A Subscription may be empty (when default-constructed or released), in such
 *  case it represents no subscription and can be assigned a new one.
 *
 * \see Signal, SubscriptionsManager */
class Subscription{
public:
  /** Default constructor creates an empty Subscription which represents no event. */
  Subscription() : id(0), target(nullptr) {}
  /** Subscriptions are movable. */
  Subscription(Subscription&& other);
  /** Subscriptions are movable. */
  Subscription& operator=(Subscription&& other);
  ~Subscription(){ Release(); }
  /** Returns true if this instance represents no subscription. */
  inline bool IsEmpty() const { return target == nullptr;}
  /** Use this function to remove a subscription from a signal. This way the 
   *  subscribed function will no longer be called when the Signal happens.*/
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

/** Base class for all signals regardless of their template parameters. You
 *  should not use this class, use Signal instead. \see Signal */
class SignalBase{
protected:
  SignalBase();
  SignalBase(const SignalBase&) = delete;
  virtual ~SignalBase();
  SignalBase& operator=(const SignalBase&) = delete;
  std::list< Subscription* > subscriptions;
  void SubscriptionAddressChanged(Subscription* old, Subscription* n);
  virtual void RemoveSubscriptionByID(int, Subscription*) = 0;
  static int subscription_id_counter;
  friend class Subscription;
};


/** 
A Signal represents an event happening from the application's point of view.
Clicking a button may be represented as a Signal. Getting reply from SCLang
may be represented as a Signal, etc.
Signals are usually publicly exposed to other classes. They can subscribe to a
Signal; a request to subscribe essentially means asking "please call this
function when this event happens". This way code can be easily bound to be
executed on user click. The class managing the signal decides when the event
happens, and triggers all subscriber functions by calling .Happen(). A Signal
can carry additional data, e.g. "toggle" signal of a checkbox will also carry
one boolean, the toggle state. That argument should be passed when .Happen()ing
the signal, and all subscribers will receive it.

There are 3 modes of subscription:

 - SubscribeOnce() - The function will be called the first time the signal happens
                    after it was subscribed, it will be automatically
                    unsubscribed afterwards.
 - SubscribeForever() - The function will be called always from now on, there is
                    no way to unsibscribe. This is useful if you wish to react
                    on a signal repetivelly, but don't want to care about
                    releasing subscription.
 - Subscribe()     - The standard way of subscribing. It returns a Subscription
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
\code
  subscriptions += some_signal.Subscribe([](){ ... });
\endcode

\see SubsctiptionManager
\see LateReturn
*/
template <typename... Types>
class Signal : public SignalBase{
private:
    std::list< std::function<void(Types...)> > subscribers_forever;
    std::map< int, std::function<void(Types...)> > subscribers_with_id;
    std::list< std::function<void(Types...)> > subscribers_once;
    void RemoveSubscriptionByID(int id, Subscription* ptr) override{
      auto it = subscribers_with_id.find(id);
      if(it == subscribers_with_id.end()){
        std::cout << "WARNING: removing an unexisting subscription!" << std::endl;
      }else{
        //std::cout << "WARNING: Erasing sub with id " << id << " size = "<< subscribers_with_id.size() << std::endl;
        subscribers_with_id.erase(it);
      }
      subscriptions.remove(ptr);
    }
public:
    Signal<Types...>() {}
    ~Signal(){}
    Signal(const Signal& other) = delete;
    Signal(Signal&& other) = delete;
    Signal& operator=(const Signal& other) = delete;
    Signal& operator=(Signal&& other) = delete;
    /** Creates a standards, releasable subscription for this Signal.
     *  \param f The function to be called when this event happens. */
    Subscription Subscribe( std::function<void(Types...)> f ) __attribute__((warn_unused_result));
    /** Subscribes a function to this Signal forever. 
     *  \param f The function to be called when this event happens. */
    void SubscribeForever( std::function<void(Types...)> f ) { subscribers_forever.push_back(f); }
    /** Subscribes a function to this Signal once. The sucscription will be
     *  releases after the function is called for the first time. 
     *  \param f The function to be called when this event happens. */
    void SubscribeOnce( std::function<void(Types...)> f ) { subscribers_once.push_back(f); }
    template<class C>
    Subscription Subscribe( C* class_ptr, void (C::*member_ptr)(Types...) ) __attribute__((warn_unused_result));
    template<class C>
    void SubscribeForever( C* class_ptr, void (C::*member_ptr)(Types...)) { subscribers_forever.push_back( std::bind(member_ptr,class_ptr,std::placeholders::_1) ); }
    template<class C>
    void SubscribeOnce( C* class_ptr, void (C::*member_ptr)(Types...)) { subscribers_once.push_back( std::bind(member_ptr,class_ptr,std::placeholders::_1) ); }
    
    /** Triggers this event. The signal owner should call this method to trigger
     *  all subscribers. The arguments to this function will be passed to all
     *  subscribers. \returns True if anyone was called.*/
    bool Happen(Types... t) {
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
        return !list_to_call.empty();
    }
    friend class Subscription;
    /** Returns the number of subscribers currently subscribed to this signal. */
    unsigned int Count(){return subscribers_forever.size() + subscribers_once.size() + subscribers_with_id.size();}
};

template <typename... Types>
Subscription __attribute__((warn_unused_result)) Signal<Types...>::Subscribe( std::function<void(Types...)> f ) {
  int sub_id = ++subscription_id_counter;
  subscribers_with_id[sub_id] = f;
  //std::cout << "New SUB registered " << sub_id << std::endl;
  auto p = Subscription(sub_id, this);
  subscriptions.push_back(&p);
  return p;
}
template <typename... Types> template <class C>
Subscription __attribute__((warn_unused_result)) Signal<Types...>::Subscribe( C* class_ptr, void (C::*member_ptr)(Types...)  ) {
  int sub_id = ++subscription_id_counter;
  subscribers_with_id[sub_id] = std::bind(member_ptr,class_ptr,std::placeholders::_1);
  //std::cout << "New SUB registered " << sub_id << std::endl;
  auto p = Subscription(sub_id, this);
  subscriptions.push_back(&p);
  return p;
}

/** A container for subscriptions. */
class SubscriptionList{
public:
  SubscriptionList(const SubscriptionList& other) = delete;
  std::list<Subscription> list;
  void ReleaseAll() { list.clear(); }
  SubscriptionList& operator+=(Subscription&& s) {
    list.push_back(std::move(s));
    return *this;
  }
  SubscriptionList() : list(0){}
};
/** This class provides a single helper member field: subscriptions.
 *  You can use it to store all subscriptions that shall be released when your
 *  object is destroyed.
 *  Use the += operator to add stored subcriptions. They will be released
 *  automatically when this object is destroyed.
 *  To use this class, inherit from it, it is recommended to use public *virtual*
 *  inheritance, since a lot of classes uses a SubscriptionsManager as one of its
 *  bases. Also note that may classes (widgets, modules) already inherit from
 *  SubscriptionsManager, so you are welcome to use that subscriptions field. */
class SubscriptionsManager{
public:
  SubscriptionList subscriptions;
};



} //namespace AlgAudio

#endif //CSIGNAL_HPP
