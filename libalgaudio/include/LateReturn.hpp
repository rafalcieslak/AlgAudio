#ifndef LATEREPLY_HPP
#define LATEREPLY_HPP
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

#include <map>
#include <functional>
#include <tuple>
#include <iostream>
#include <typeinfo>
#include <typeindex>
#include <memory>
#include "Exception.hpp"

namespace AlgAudio{

/* The LateReturn is a mechanism that alows functions to return a value with
some asynchronic delay. It is useful when a function waits for OSC reply from
SCLang, and therefore would block execution for a while. Instead of returning
a value, it returns LateReturn<value>. The caller can use the .Then method
of LateReturn to set a function which should be called when the reply is ready.
The callee can only prepare a LateReturn by creating a Relay. The relay is useful
only within the calee, it has two interesting methods: .Return, which should be
called when the reply value is ready - it will execute the "Then" procedure,
and GetLateReturn, which creates a corresponding LateReturn, which the function
should return. For convience, Relay is imnplicitly convertible to LateReturn.
If the callee happens to return immediatelly, this is not a problem, in such
case the Then procedure will be executed as soon as it is set.

Example LateReturn function:

LateReturn<int> GetReplyFromSubprocess(){
  auto r = Relay<int>::Create();
  AsyncActivities::CallThisFunctionWhenSubprocessSendsReply([](lo::Message msg){
    r.Return( msg[1]->i32 );
  });
  return r;
}

Example usage:

GetSCVersion.Then([](int v){
  std::cout << "Version " << v << std::endl;
});
std::cout << "Asked for version" << std::endl;

Keep in mind that the lambda above will be (most likely) executed AFTER the last
line of the example, at the time when the OSC reply arrives.

*/


template <typename... Types>
class LateReturn;
template <typename... Types>
class Relay;

/* A Sync is a helper class for waiting for several LateReplies to arrive.
 * Example usage:

Sync s(3);
PerformLongActionA(args1).ThenSync(s);
PerformLongActionB(args2).ThenSync(s);
PerformLongActionC(args3).ThenSync(s);
s.WhenAll([](){
  std::cout << "Done!" << std::endl;
});

 */
class Sync{
public:
  Sync(int count);
  void WhenAll(std::function<void()> f) const;
  void Trigger() const;
private:
  const unsigned int id;
  struct SyncEntry{
    SyncEntry(int c) : count(c) {}
    int count = 2;
    bool stored = false;
    std::function<void()> stored_func;
  };
  static std::map<unsigned int, SyncEntry*> entries;
  static unsigned int id_counter;
};

template <typename T>
std::function<void()> bind_tuple(std::function<void(T)> f, std::tuple<T> t){
  return std::bind(f, std::get<0>(t));
}
template <typename T, typename S>
std::function<void()> bind_tuple(std::function<void(T, S)> f, std::tuple<T, S> t){
  return std::bind(f, std::get<0>(t), std::get<1>(t));
}
template <typename T, typename S, typename R>
std::function<void()> bind_tuple(std::function<void(T, S, R)> f, std::tuple<T, S, R> t){
  return std::bind(f, std::get<0>(t), std::get<1>(t), std::get<2>(t));
}
inline std::function<void()> bind_tuple(std::function<void()> f, std::tuple<>){
  return f;
}

class LateReturnEntryBase{
protected:
  virtual ~LateReturnEntryBase(){}
  bool triggered = false;
public:
  static std::map<int, LateReturnEntryBase*> entries;
  static int id_counter;
};
template <typename... Types>
class LateReturnEntry : public LateReturnEntryBase{
public:
  friend class LateReturn<Types...>;
  friend class Relay<Types...>;
private:
  LateReturnEntry(){};
  void Invoke(){
    std::function<void()> f = bind_tuple(stored_func, stored_args);
    try{
      f();
    }catch(...){
      std::cout << "Exception while invoking a latereturn continuation" << std::endl;
    }
  }
  std::function<void(Types...)> stored_func;
  std::tuple<Types...> stored_args;
  std::map<std::type_index, std::function<void(std::shared_ptr<Exception>)>> catchers;
  std::shared_ptr<Exception> stored_exception;
  std::function<void(std::shared_ptr<Exception>)> default_catcher;
  bool stored = false;
};

template <typename... Types>
class LateReturn{
public:
  const LateReturn& Then(std::function<void(Types...)> f) const{
    auto it = LateReturnEntryBase::entries.find(id);
    if(it == LateReturnEntryBase::entries.end()){
      std::cout << "ERROR: LateReturn Then called, but it is not in the base!" << std::endl;
      return *this;
    }
    LateReturnEntry<Types...>* entry = dynamic_cast<LateReturnEntry<Types...>*>(it->second);
    if(!entry->triggered){
      entry->stored_func = f;
      entry->stored = true;
    }else{
      // The Relay has already returned, but it was not bound until now.
      entry->stored_func = f;
      entry->Invoke();
      delete entry;
      LateReturnEntryBase::entries.erase(it);
    }
    return *this;
  }
  
  template<typename Ex>
  const LateReturn& Catch(std::function<void(std::shared_ptr<Exception>)> func) const{
    auto it = LateReturnEntryBase::entries.find(id);
    if(it == LateReturnEntryBase::entries.end()){
      // Catch is called, but the entry has already returned. Therefore, ignore the catcher.
      return *this;
    }
    LateReturnEntry<Types...>* entry = dynamic_cast<LateReturnEntry<Types...>*>(it->second);
    if(entry->stored_exception){
      std::cout << "There is a stored exception already" << std::endl;
      func(entry->stored_exception);
    }else{
      entry->catchers[typeid(Ex)] = func;
    }
    return *this;
  }
  
  template<typename Ex>
  const LateReturn& CatchAll(std::function<void(std::shared_ptr<Exception>)> func) const{
    auto it = LateReturnEntryBase::entries.find(id);
    if(it == LateReturnEntryBase::entries.end()){
      // Catch is called, but the entry has already returned. Therefore, ignore the catcher.
      return *this;
    }
    LateReturnEntry<Types...>* entry = dynamic_cast<LateReturnEntry<Types...>*>(it->second);
    if(entry->default_catcher){
      std::cout << "ERROR: Cannot add another default cather to the same latereturn" << std::endl;
      return;
    }
    if(entry->stored_exception){
      std::cout << "There is a stored exception already" << std::endl;
      func(entry->stored_exception);
    }else{
      entry->default_catcher = func;
    }
    return *this;
  }
  
  // Tells a latereturn to pass all latethrown exceptions to another relay.
  // Chaining relays this way enables correct logical stack unwinding.
  template<typename... X>
  const LateReturn& Catch(const Relay<X...>& r) const{
    auto it = LateReturnEntryBase::entries.find(id);
    if(it == LateReturnEntryBase::entries.end()){
      // Catch is called, but the entry has already returned. Therefore, ignore the catcher.
      return *this;
    }
    LateReturnEntry<Types...>* entry = dynamic_cast<LateReturnEntry<Types...>*>(it->second);
    if(entry->default_catcher){
      std::cout << "ERROR: Cannot add another default cather to the same latereturn" << std::endl;
      return *this;
    }
    if(entry->stored_exception){
      std::cout << "There is a stored exception already" << std::endl;
      // Pass the exception to that parent relay
      r.PassException(entry->stored_exception);
    }else{
      // Pass all exceptions to parent relay
      entry->default_catcher = [r](std::shared_ptr<Exception> ex){
        r.PassException(ex);
      };
    }
    return *this;
  }
  
  const LateReturn& ThenSync(Sync& s) const{
    Then([=](Types...)mutable{
      s.Trigger();
    });
    return *this;
  }
  const LateReturn& ThenReturn(Relay<Types...> r) const{
    Then([r](Types... result){
      r.Return(result...);
    });
    return *this;
  }
  LateReturn(const Relay<Types...>& r) : id(r.id) {}
  LateReturn(const LateReturn& other) = delete;
  LateReturn& operator=(const LateReturn& other) = delete;
  LateReturn(LateReturn&& other) : id(other.id) {}
  LateReturn& operator=(LateReturn&& other) {id = other.id;}
  friend class Relay<Types...>;
private:
  LateReturn(int i) : id(i) {};
  const int id;
};

template <typename... Types>
class Relay{
public:
  Relay(){
    int newid = LateReturnEntryBase::id_counter++;
    LateReturnEntryBase::entries[newid] = new LateReturnEntry<Types...>();
    id = newid;
  }
  const Relay& Return(Types... args) const{
    auto it = LateReturnEntryBase::entries.find(id);
    //std::cout << "Returning " << id << std::endl;
    if(it == LateReturnEntryBase::entries.end()){
      std::cout << "ERROR: Return() used on the same relay " << id << " twice!" << std::endl;
      std::cout << "Did you remember to capture the relay by-value?" << std::endl;
      return *this;
    }
    LateReturnEntry<Types...>* entry = dynamic_cast<LateReturnEntry<Types...>*>(it->second);
    if(entry->stored){
      entry->stored_args = std::tuple<Types...>(args...);
      entry->Invoke();
      delete entry;
      LateReturnEntryBase::entries.erase(it);
    }else{
      entry->stored_args = std::tuple<Types...>(args...);
      entry->triggered = true;
    }
    return *this;
  }
  template<typename Ex, typename... ConstructArgs>
  const Relay& LateThrow(ConstructArgs... args) const{
    std::shared_ptr<Ex> exception = std::make_shared<Ex>(args...);
    
    auto it = LateReturnEntryBase::entries.find(id);
    if(it != LateReturnEntryBase::entries.end()){
      LateReturnEntry<Types...>* entry = dynamic_cast<LateReturnEntry<Types...>*>(it->second);
      // Check if there is a catcher registered for this exception.
      auto it2 = entry->catchers.find(typeid(Ex));
      if(it2 != entry->catchers.end()){
        // If so, call the cather.
        (it2->second)(exception);
      }else{
        if(entry->default_catcher){
          // Use the default catcher.
          (entry->default_catcher)(exception);
        }else{
          // Otherwise store it for later.
          entry->stored_exception = exception;
          
          if(entry->stored){
            // There is a stored then entry, but no catcher. This ususally means trouble.
            // TODO : Detect when the last referencet to LastReturn is lost. If
            // there are no references, and no catcher, there is no chance it could
            // be aded later. Warn about this.
            std::cout << "WARNING: A LateThrow cannot be caught and is ignored: " << exception->what() << std::endl;
          }
        }
      }
    }else{
      std::cout << "ERROR: A relay may not LateThrow(...) after Return(...)ing." << std::endl; 
    }
    return *this;
  }
  const Relay& PassException(std::shared_ptr<Exception> ex) const{
    auto it = LateReturnEntryBase::entries.find(id);
    if(it != LateReturnEntryBase::entries.end()){
      LateReturnEntry<Types...>* entry = dynamic_cast<LateReturnEntry<Types...>*>(it->second);
      // Check if there is a catcher registered for this exception.
      auto it2 = entry->catchers.find(typeid(*ex));
      if(it2 != entry->catchers.end()){
        // If so, call the cather.
        (it2->second)(ex);
      }else{
        if(entry->default_catcher){
          // Use the default catcher.
          (entry->default_catcher)(ex);
        }else{
          // Otherwise store it for later.
          entry->stored_exception = ex;
          
          if(entry->stored){
            // There is a stored then entry, but no catcher. This ususally means trouble.
            // TODO : Detect when the last referencet to LastReturn is lost. If
            // there are no references, and no catcher, there is no chance it could
            // be aded later. Warn about this.
            std::cout << "WARNING: A LateThrow cannot be caught and is ignored: " << ex->what() << std::endl;
          }
        }
      }
    }else{
      std::cout << "ERROR: A relay may not PassException(...) after Return(...)ing." << std::endl; 
    }
    return *this;
  }
  LateReturn<Types...> GetLateReturn() const{
    return LateReturn<Types...>(id);
  }
  static Relay Create(){
    int newid = LateReturnEntryBase::id_counter++;
    LateReturnEntryBase::entries[newid] = new LateReturnEntry<Types...>();
    return Relay(newid);
  }
  friend class LateReturn<Types...>;
  int GetID() const {return id;}
private:
  int id = -42;
  Relay(int i) : id(i) {};
};

/* This is a wrapper method for setting variable values as returned by a
   latereply. */
template <typename T>
LateReturn<> LateAssign(T& to_set, LateReturn<T> lr){
  auto r = Relay<>::Create();
  lr.Then([r,&to_set](T val)mutable{
    to_set = val;
    r.Return();
  });
  return r;
}


template <typename T> struct identity
{
  typedef T type;
};
template <typename... X>
void operator>>=(LateReturn<X...>&& first, typename identity<std::function<void(X...)>>::type then){
  first.Then(then);
}

template <typename... X, typename Y>
LateReturn<Y> operator>>=(LateReturn<X...>&& first, typename identity<std::function<Y(X...)>>::type then){
  auto r = Relay<Y>::Create();
  first.Then([=](X... args){
    r.Return( then(args...) );
  });
  return r;
}

template <typename... X, typename... Y>
LateReturn<Y...> operator>>=(LateReturn<X...>&& first, typename identity<std::function<LateReturn<Y...>(X...)>>::type then){
  auto r = Relay<Y...>::Create();
  first.Then([=](X... args){
    then(args...).ThenReturn(r);
  });
  return r;
}

} // namespace AlgAudio

#endif // LATEREPLY_HPP
