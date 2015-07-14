#ifndef CSIGNAL_HPP
#define CSIGNAL_HPP

#include <list>
#include <functional>

template <typename... Types>
class Signal {
private:
    std::list< std::function<void(Types...)> > subscribers;
    std::list< std::function<void(Types...)> > once_subscribers;
    std::list< std::function<void() > > deaf_subscribers;
public:
    Signal<Types...>() { subscribers.clear(); }
    void Subscribe( std::function<void(Types...)> f ) { subscribers.push_back(f); }
    void SubscribeOnce( std::function<void(Types...)> f ) { once_subscribers.push_back(f); }
    void Subscribe( std::function<void()> f ) { deaf_subscribers.push_back(f); };
    template<class C>
    void Subscribe( C* class_ptr, void (C::*member_ptr)(Types...)) { subscribers.push_back( std::bind(member_ptr,class_ptr,std::placeholders::_1) ); }
    template<class C>
    void SubscribeOnce( C* class_ptr, void (C::*member_ptr)(Types...)) { once_subscribers.push_back( std::bind(member_ptr,class_ptr,std::placeholders::_1) ); }
    template<class C>
    void Subscribe( C* class_ptr, void (C::*member_ptr)()) { deaf_subscribers.push_back( std::bind(member_ptr,class_ptr) ); }
    void Happen(Types... t) {
        for(auto f : subscribers) f(t...);
        for(auto f : deaf_subscribers) f();
        for(auto f : once_subscribers) f(t...);
        once_subscribers.clear();
    }
    void Clear() {
      deaf_subscribers.clear();
      once_subscribers.clear();
      subscribers.clear();
    }
};

// Specialisation for 0 args, because the general case cannot distinguish
// subscribers from deaf subscribers
template <>
class Signal<> {
private:
    std::list< std::function<void()> > subscribers;
    std::list< std::function<void()> > once_subscribers;
public:
    Signal<>() { subscribers.clear(); }
    void Subscribe( std::function<void()> f ) { subscribers.push_back(f); }
    void SubscribeOnce( std::function<void()> f ) { once_subscribers.push_back(f); }
    template<class C>
    void Subscribe( C* class_ptr, void (C::*member_ptr)()) { subscribers.push_back( std::bind(member_ptr,class_ptr) ); }
    template<class C>
    void SubscribeOnce( C* class_ptr, void (C::*member_ptr)()) { once_subscribers.push_back( std::bind(member_ptr,class_ptr) ); }
    void Happen() {
        for(auto f : subscribers) f();
        for(auto f : once_subscribers) f();
        once_subscribers.clear();
    }
    void Clear() {
      once_subscribers.clear();
      subscribers.clear();
    }
};

#endif //CSIGNAL_HPP
