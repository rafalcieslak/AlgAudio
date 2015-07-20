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
#include <iostream>

namespace AlgAudio{

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
  bool stored = false;
};

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
    entry->stored_func = f;
    entry->stored = true;
  }
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
  void Return(Types... args) const{
    auto it = LateReplyEntryBase::entries.find(id);
    if(it == LateReplyEntryBase::entries.end()){
      std::cout << "ERROR: Relay reply triggered, but the latereply does not exist anymore!" << std::endl;
      return;
    }
    LateReplyEntry<Types...>* entry = dynamic_cast<LateReplyEntry<Types...>*>(it->second);
    if(entry->stored){
      (entry->stored_func)(args...);
      LateReplyEntryBase::entries.erase(it);
    }else{
      std::cout << "ERROR: LateReply returning too fast!" << std::endl;
    }
  }
  LateReply<Types...> GetLateReply() const{
    return LateReply<Types...>(id);
  }
  operator LateReply<Types...>() const{
    return GetLateReply();
  }
  static Relay Create(){
    int newid = LateReplyEntryBase::id_counter++;
    LateReplyEntryBase::entries[newid] = new LateReplyEntry<Types...>();
    return Relay(newid);
  }
private:
  Relay(int i) : id(i) {};
  const int id;
};

} // namespace AlgAudio

#endif // LATEREPLY_HPP
