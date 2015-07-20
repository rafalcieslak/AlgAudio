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

/* The LateReplay is a mechanism that alows functions to return a value with
some asynchronic delay. It is useful when a function waits for OSC reply from
SCLang, and therefore would block execution for a while. Instead of returning
a value, it returns LateReply<value>. The caller can use the .Then method
of LateReply to set a function which should be called when the reply is ready.
The callee can only prepare a LateReply by creating a Relay. The relay is useful
only within the calee, it has two interesting methods: .Return, which should be
called when the reply value is ready - it will execute the "Then" procedure,
and GetLateReply, which creates a corresponding LateReply, which the function
should return. For convience, Relay is imnplicitly convertible to LateReply.
If the callee happens to return immediatelly, this is not a problem, in such
case the Then procedure will be executed as soon as it is set.

Example LateReturn function:

LateReturn<int> GetSCVersion(){
  auto r = Relay<int>::Create();
  SCLang::SendOSC([](lo::Message msg){
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
class LateReply;
template <typename... Types>
class Relay;

class LateReplyEntryBase{
protected:
  virtual ~LateReplyEntryBase(){}
  bool triggered = false;
public:
  static std::map<int, LateReplyEntryBase*> entries;
  static int id_counter;
};
template <typename... Types>
class LateReplyEntry : public LateReplyEntryBase{
public:
  friend class LateReply<Types...>;
  friend class Relay<Types...>;
private:
  LateReplyEntry(){};
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
class LateReply{
public:
  void Then(std::function<void(Types...)> f) const{
    auto it = LateReplyEntryBase::entries.find(id);
    if(it == LateReplyEntryBase::entries.end()){
      std::cout << "ERROR: LateReplay Then called, but it is not in the base!" << std::endl;
      return;
    }
    LateReplyEntry<Types...>* entry = dynamic_cast<LateReplyEntry<Types...>*>(it->second);
    if(!entry->triggered){
      entry->stored_func = f;
      entry->stored = true;
    }else{
      // The Relay has already returned, but it was not bound until now.
      std::function<void()> g = bind_tuple(f,entry->stored_args);
      g();
      LateReplyEntryBase::entries.erase(it);
    }
  }
  template<typename... Ts>
  void ThenReturn(Relay<Ts...> r, Ts... args){ r.Return(args...);}
  LateReply(const Relay<Types...>& r) : id(r.id) {}
  LateReply(const LateReply& other) = delete;
  LateReply& operator=(const LateReply& other) = delete;
  LateReply(LateReply&& other) : id(other.id) {}
  LateReply& operator=(LateReply&& other) {id = other.id;}
  friend class Relay<Types...>;
private:
  LateReply(int i) : id(i) {};
  const int id;
};
template <typename... Types>
class Relay{
public:
  const Relay& Return(Types... args) const{
    auto it = LateReplyEntryBase::entries.find(id);
    //std::cout << "Returning " << id << std::endl;
    if(it == LateReplyEntryBase::entries.end()){
      std::cout << "ERROR: Return() used on the same relay twice!" << std::endl;
      return *this;
    }
    LateReplyEntry<Types...>* entry = dynamic_cast<LateReplyEntry<Types...>*>(it->second);
    if(entry->stored){
      (entry->stored_func)(args...);
      LateReplyEntryBase::entries.erase(it);
    }else{
      entry->stored_args = std::tuple<Types...>(args...);
      entry->triggered = true;
    }
    return *this;
  }
  LateReply<Types...> GetLateReply() const{
    return LateReply<Types...>(id);
  }
  static Relay Create(){
    int newid = LateReplyEntryBase::id_counter++;
    LateReplyEntryBase::entries[newid] = new LateReplyEntry<Types...>();
    return Relay(newid);
  }
  friend class LateReply<Types...>;
private:
  Relay(int i) : id(i) {};
  const int id;
};

} // namespace AlgAudio

#endif // LATEREPLY_HPP
