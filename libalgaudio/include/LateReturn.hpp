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
  std::function<void(Types...)> stored_func;
  std::tuple<Types...> stored_args;
  bool stored = false;
};

template <typename T> constexpr
std::function<void()> bind_tuple(std::function<void(T)> f, std::tuple<T> t){
  return std::bind(f, std::get<0>(t));
}
template <typename T, typename S> constexpr
std::function<void()> bind_tuple(std::function<void(T, S)> f, std::tuple<T, S> t){
  return std::bind(f, std::get<0>(t), std::get<1>(t));
}
template <typename T, typename S, typename R> constexpr
std::function<void()> bind_tuple(std::function<void(T, S, R)> f, std::tuple<T, S, R> t){
  return std::bind(f, std::get<0>(t), std::get<1>(t), std::get<2>(t));
}
inline std::function<void()> bind_tuple(std::function<void()> f, std::tuple<>){
  return f;
}

template <typename... Types>
class LateReturn{
public:
  void Then(std::function<void(Types...)> f) const{
    auto it = LateReturnEntryBase::entries.find(id);
    if(it == LateReturnEntryBase::entries.end()){
      std::cout << "ERROR: LateReturn Then called, but it is not in the base!" << std::endl;
      return;
    }
    LateReturnEntry<Types...>* entry = dynamic_cast<LateReturnEntry<Types...>*>(it->second);
    if(!entry->triggered){
      entry->stored_func = f;
      entry->stored = true;
    }else{
      // The Relay has already returned, but it was not bound until now.
      std::function<void()> g = bind_tuple(f,entry->stored_args);
      g();
      delete entry;
      LateReturnEntryBase::entries.erase(it);
    }
  }
  void ThenSync(Sync& s) const{
    Then([=](Types... args)mutable{
      s.Trigger();
    });
  }
  template<typename... Ts>
  void ThenReturn(Relay<Ts...> r, Ts... args){ r.Return(args...);}
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
  const Relay& Return(Types... args) const{
    auto it = LateReturnEntryBase::entries.find(id);
    //std::cout << "Returning " << id << std::endl;
    if(it == LateReturnEntryBase::entries.end()){
      std::cout << "ERROR: Return() used on the same relay twice!" << std::endl;
      return *this;
    }
    LateReturnEntry<Types...>* entry = dynamic_cast<LateReturnEntry<Types...>*>(it->second);
    if(entry->stored){
      (entry->stored_func)(args...);
      delete entry;
      LateReturnEntryBase::entries.erase(it);
    }else{
      entry->stored_args = std::tuple<Types...>(args...);
      entry->triggered = true;
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
  const int id;
private:
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


} // namespace AlgAudio

#endif // LATEREPLY_HPP
