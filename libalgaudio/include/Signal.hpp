#ifndef CSIGNAL_HPP
#define CSIGNAL_HPP

#include <list>
#include <map>
#include <functional>

/* A quick explanation of signal semantics
I will write the full docs once things get a bit more stable.

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
SubscriveForever - The function will be called always from now on, there is
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
You can also unsubscribe from a signal by calling .release() on a Subscription.
This resets the Subscription to the default, empty state. A Subscription is
not copiable and not copy-constructible, but it is movable.

*/

class Subscription{
private:

public:
  ~Subscription();
  //template <typename...>
  //friend class Signal;
};

template <typename... Types>
class Signal {
private:
    std::list< std::function<void(Types...)> > subscribers_forever;
    std::map< int, std::function<void(Types...)> > subscribers_with_id;
    std::list< std::function<void(Types...)> > subscribers_once;
    int id_counter = false;
public:
    Signal<Types...>() { }
    void SubscribeForever( std::function<void(Types...)> f ) { subscribers_forever.push_back(f); }
    void SubscribeOnce( std::function<void(Types...)> f ) { subscribers_once.push_back(f); }
    template<class C>
    void SubscribeForever( C* class_ptr, void (C::*member_ptr)(Types...)) { subscribers_forever.push_back( std::bind(member_ptr,class_ptr,std::placeholders::_1) ); }
    template<class C>
    void SubscribeOnce( C* class_ptr, void (C::*member_ptr)(Types...)) { subscribers_once.push_back( std::bind(member_ptr,class_ptr,std::placeholders::_1) ); }
    void Happen(Types... t) {
        // First, gather the list of all stuff to call. It has to be a temporary
        // local list, because the calees may modify some of the lists by
        // inserting new subscriptions.
        std::list< std::function<void(Types...)> > list_to_call;
        for(auto f : subscribers_forever) list_to_call.push_back(f);
        for(auto f : subscribers_once) list_to_call.push_back(f);
        // Clearing the list before calling calees, so that if they add new
        // one-time-subscriptions, they won't be lost.
        subscribers_once.clear();
        // Call everybody.
        for(auto f : list_to_call) f(t...);
    }
    friend class Subscription;
};
#endif //CSIGNAL_HPP
