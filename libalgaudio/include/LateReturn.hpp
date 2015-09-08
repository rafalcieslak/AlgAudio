#ifndef LATERETURN_HPP
#define LATERETURN_HPP
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
#include <unordered_map>
#include <functional>
#include <tuple>
#include <iostream>
#include <typeinfo>
#include <typeindex>
#include <memory>
#include "Exception.hpp"

namespace AlgAudio{

template <typename... Types>
class LateReturn;
template <typename... Types>
class Relay;

/** Sync is a helper class that makes it easy to wait for multiple LateReplies
 *  to arrive.
 *  
 *  Sync maintains an internal counter that is initially set to some
 *  value. Each finished LateReturn decrements the counter, when it reaches zero
 *  the stored function (set by WhenAll) is called.
 * 
 *  Example usage:
 * 
 *  \code
 *  Sync s(3);
 *  PerformLongActionA(args1).ThenSync(s);
 *  PerformLongActionB(args2).ThenSync(s);
 *  PerformLongActionC(args3).ThenSync(s);
 *  s.WhenAll([](){
 *    std::cout << "Done!" << std::endl;
 *  });
 *  \endcode
 *  
 *  Sync instances are shared; when copy-assigning or copy-constructing a sync,
 *  it will use the same counter. Therefore, when using Sync in lambda functions,
 *  it is recommended to pass Sync instances by-value.
 */
class Sync{
public:
  /** Constructs a new instance of a Sync.
   *  \param count The number of LateReplies to wait for.
   */
  Sync(int count);
  /** Sets the function that is supposed to happen when the counter gets down
   *  to zero.
   *  \param func The parram-less function to call.
   */
  void WhenAll(std::function<void()> func) const;
  /** Decrements the internal counter, and, potentially, invokes the stored function.*/
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

/** This class is an internal implementation of LateReturn mechanism. 
 *  You should never use this class on your own.
  */ 
class LateReturnEntryBase{
protected:
  virtual ~LateReturnEntryBase(){}
  bool triggered = false;
public:
  static std::map<int, LateReturnEntryBase*> entries;
  static int id_counter;
};
/** This class is an internal implementation of LateReturn mechanism. 
 *  You should never use this class on your own.
 *
 *  Instances of this class store information concerning a particular
 *  LateReturn <-> Relay pair, such as the function set with Then.
 */ 
template <typename... Types>
class LateReturnEntry : public LateReturnEntryBase{
public:
  friend class LateReturn<Types...>;
  friend class Relay<Types...>;
private:
  LateReturnEntry(){};
  /** This method calls the stored function with stored arguments. */
  void Invoke(){
    std::function<void()> f = bind_tuple(stored_func, stored_args);
    try{
      f();
    }catch(...){
      std::cout << "Exception while invoking a latereturn continuation" << std::endl;
    }
  }
  /** The stored function that is meant to be called when the corresponding relay returns */
  std::function<void(Types...)> stored_func;
  /** The returned arguments may be stored if a relay returned before a continuation function was set with LateReturn::Then */
  std::tuple<Types...> stored_args;
  /** The collection of exception handling functions, mapped by exception type */
  std::unordered_map<std::type_index, std::function<void(std::shared_ptr<Exception>)>> catchers;
  /** The exception that was LateThrown before a catcher was set */
  std::shared_ptr<Exception> stored_exception;
  /** The catcher function set with LateReturn::CatchAll, it will be called on any exception that is not present in the map of catchers */
  std::function<void(std::shared_ptr<Exception>)> default_catcher;
  /** This flag gets set to true when any function is stored with for this LateReturn */
  bool stored = false;
};

/** The LateReturn class template provides a global, universal mechanism for
 *  managing asynchronous code execution. The idea is that some functions may
 *  not be able to return their value immediatelly, but only after some longer
 *  time. Usually, such situation might be resolved by allowing the function to
 *  block execution, and simply call the function in another thread. Instead,
 *  a function can return a LateReturn<X>, which represents a value that is 
 *  not yet available. The called can then use that returned LateReturn instance
 *  to define what action should be taken when that value arrives. A simple
 *  example of usage might be:
 * 
 *  \code
 *  LateReturn<std::string> GetServerVersion(); // Uses TCP connection to request server version from a remote host.
 *  ...
 *  void PrintServerInfo(){
 *    GetServerVersion().Then([](std::string version){
 *      std::cout << "Sever version is " << version << std::endl;
 *    });
 *    std::cout << "Requested version info from server, will report when it arrives..." << std::endl;
 *  }
 *  ...
 *  \endcode
 *
 *  In the presented example, the code within lambda function is likely to be
 *  executed when PrintServerInfo() has already returned. However, if
 *  GetServerVersion has cached data, it may have the resulting value ready
 *  immediatelly, and in such case the lambda function will be called before
 *  the other text message is printed. The order in which continuation functions
 *  (the functions set with Then(), which represent action which should be called
 *  when the returned value is ready) are called may appear random and
 *  confusing, but the exact order should never be significant. What is
 *  guaranteed is that each continuation function is called as soon as both
 *  the latereturned value and the continuation function are set.
 *
 *  The lifetime of the stored function is kept to minumum. This is especially
 *  important when using lambdas to define continuations:
 *
 *  \code
 *  void function(){
 *    Object x;
 *    LateReturningFunc().Then([&x](){
 *      x.value = 5;
 *    });
 *  }
 *  \endcode
 *  
 *  In the above example, setting x.value may be an invalid operation, because
 *  the x Object may have been already destructed when the LateReturningFunc
 *  completes it's work.
 *
 *  One possible solution is to use shared pointers to prolong object lifetime
 *  as long as the stored function is needed. The stored function will be
 *  destructed just after it is called, and so will be the captured shared_ptr.
 *  However, it is possible to create ownership cycles, if such a shared 
 *  pointer gets stored in a LateReturn that is owned by the same object.
 *
 *  Similarly, there is nothing wrong in capturing this by a lambda that is
 *  passed to Then(), just keep in mind what the LateReturn lifetime will be,
 *  and whether it is guaranteed that this will still exist by the time there
 *  continuation function is invoked.
 *
 *  The return value may never become ready, for example if the latereturning
 *  function keeps waiting for en event that never happens. In such case the 
 *  continuation will never get called.
 *
 *  It is also possible that when LateReturning function has failed and cannot
 *  provide a value, it decides to thow an exception, to notify whoever called
 *  it about some kind of a problem. Obviously, using a try{}catch(){} block
 *  won't work, because at the time an exception occurs, the actual stack for
 *  the call to latereturning function no longer exists. Instead, you can use
 *  Catch() to set a handler for a given exception. Example:
 *
 *  \code
 *  GetServerVersion().Then([](std::string version){
 *    std::cout << version << std::endl;
 *  }).Catch<CommunicationException>([](std::shared_ptr<Exception> ex){
 *    std::cout << "Failed to get server version, reason: " << ex->reason << std::endl;
 *  });
 *  \endcode
 *
 *  Similarly to continuations, exception handling functions are also called when
 *  appropriate, which may mean a long delay.
 *
 *  See Relay documentation for details on how to write a LateReturning
 *  function.
 *  \see Relay
 */
template <typename... Types>
class LateReturn{
public:
  /** This method is used define continuation action.
   *  \param f The continuation function that should be called when latereturned value becomes available.
   *
   *  \warning f may be called immediatelly, if the value is already available.
   * 
   *  Only one continuation function may be set. Setting another overrides the previous.
   */
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
  
  /** Sets the handler for a given type of exceptions.
   *  \param func The handler function to be called when selected exception happen.
   *
   *  Only one handler for each exception class may be set. Setting another overrides the previous.
   */
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
  
  /** Sets the default handler for all exceptions.
   *  \param func The handler function to be called when any exception happen, and
   *  there is no specific handler set for that type of exception.
   * 
   *  Only one default handler may be set. Setting another overrides the previous.
   */
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
  
  /** Tells a LateReturn to pass all latethrown exceptions to another Relay.
   *  Chaining relays this way enables correct logical stack unwinding.
   *  \param r The parent Relay.
   *  \see Relay
   */
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
  
  /** A helper method for triggering a Sync when returned value is available.
   *  \param s The Sync to trigger.
   *  \see Sync
   */
  const LateReturn& ThenSync(Sync& s) const{
    Then([=](Types...)mutable{
      s.Trigger();
    });
    return *this;
  }
  /** A helper method for returning another Relay's value when the result
   *  becomes ready.
   *  \param r The Relay that should return then.
   *  \see Relay
   */
  const LateReturn& ThenReturn(Relay<Types...> r) const{
    Then([r](Types... result){
      r.Return(result...);
    });
    return *this;
  }
  /** Implicit converting constructor from a Relay. */
  LateReturn(const Relay<Types...>& r) : id(r.id) {}
  LateReturn(const LateReturn& other) = delete; /**< \warning Deleted. No copy-constructing. */
  LateReturn& operator=(const LateReturn& other) = delete; /**< \warning Deleted. No copy-assigning. */
  LateReturn(LateReturn&& other) : id(other.id) {}
  LateReturn& operator=(LateReturn&& other) {id = other.id;}
  friend class Relay<Types...>;
private:
  LateReturn(int i) : id(i) {};
  const int id;
};

/** The Relay is a helper class for creating functions that cannot return their
 *  value imediatelly. It can create a LateReturn to be given to the caller,
 *  as well as return the value when it is ready, or throw a late exception.
 *
 *  Example usage:
 *  \code
 *  LateReturn<int> GetProcessExitCode(Process& p){
 *    Relay<int> r;
 *    p.CallThisFuncWhenExitted([r,p](){
 *      int exitcode = p.Status();
 *      r.Return(exitcode);
 *    });
 *    return r;
 *  }
 *  \endcode
 *
 *  When writing a LateReturning function, the first step is to create a relay.
 *  The template types must match the types of LateReturn this function returns.
 *  This does not have to be a single type, a Relay can return multiple values.
 *
 *  The return statement in the above example implicitly converts the relay to
 *  a corresponding LateReturn object. The parent called will then define
 *  when action should be called when the Relay's value is ready.
 *
 *  Also note how the lambda captures r by-value. It makes little sense to
 *  capture it by-reference, since r will no longer exist once
 *  GetProcessExitCode returns. When capturing it by-value, the copy-constructed
 *  clone of r that is stored with the function will be linked to the same
 *  LateReturn as the original r. Thus storing relays by-value should give
 *  desired results.
 *
 *  When calling Return() you pass a returned value as argument (or not, if
 *  the Relay has no types, i.e. the function returns LateReturn<>). If the
 *  corresponding continuation function has been already set, calling Return
 *  will also invoke that function. Notice that while logical stack goes down
 *  (we are returning a value), the actual stack goes up (we call function
 *  Return which in turn calls some other function that needs our return value).
 *
 *  Do not throw C++ exceptions in a LateReturning function. Because of how the
 *  stack is inverted, the exception would propagate in the wrong direction.
 *  It is often not obvious who called the code of a latereturning function or
 *  an embedded lambda. Where the exception should be passed is upwards,
 *  to a corresponding LateReturn, so that whoever called this function can
 *  define a handler. To do this, use LateThrow<Type>(constuction args), which
 *  will correctly pass the exception to the LateReturn's user-defined 
 *  exception handler.
 */
template <typename... Types>
class Relay{
public:
  /** Constructs a new Relay. */
  Relay(){
    int newid = LateReturnEntryBase::id_counter++;
    LateReturnEntryBase::entries[newid] = new LateReturnEntry<Types...>();
    id = newid;
  }
  /** Passes the returned value. Call this when the value you wish to return becomes available.
   *  Calling Return() will invoke corresponding continuation functions, if set.
   */
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
  /** Creates and passes an exception to the corresponding LateReturn, so that it may catch it.
   *  
   *  Example:
   *  \code
   *  LateReturn<std::string> GetServerCodename(){
   *    Relay<std::string> r;
   *    GetServerVersion().Then([r](int v){
   *      if(v == 1) r.Return("Fluffy");
   *      if(v == 2) r.Return("Puffy");
   *      else r.LateThrow<UnknownVersionException>("Version " + std::to_string(v) + " unrecognized.");
   *    });
   *  }
   *  \endcode
   */
  template<typename Ex, typename... ConstructArgs>
  const Relay& LateThrow(ConstructArgs... args) const{
    std::shared_ptr<Ex> exception = std::make_shared<Ex>(args...);
    PassException(exception);
    return *this;
  }
  /** Passes an alredy created exception to the corresponding LateReturn, so that it may catch it. */
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
  /** Returns the LateReturn corresponding to this Relay */
  LateReturn<Types...> GetLateReturn() const{
    return LateReturn<Types...>(id);
  }
  friend class LateReturn<Types...>;
  int GetID() const {return id;}
private:
  int id = -42;
  Relay(int i) : id(i) {};
};

/** This is a wrapper method for setting variable values as returned by a LateReturn.
 *  \param[out] to_set This variable will be set to value returned by lr as soon as it becomes available.
 *  \param lr This is the LateReturn that is expected to provide the value to be assigned.
 *  \return This function LateReturns void when the assignment is complete.*/
template <typename T>
LateReturn<> LateAssign(T& to_set, LateReturn<T> lr){
  Relay<> r;
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
  Relay<Y> r;
  first.Then([=](X... args){
    r.Return( then(args...) );
  });
  return r;
}

template <typename... X, typename... Y>
LateReturn<Y...> operator>>=(LateReturn<X...>&& first, typename identity<std::function<LateReturn<Y...>(X...)>>::type then){
  Relay<Y...> r;
  first.Then([=](X... args){
    then(args...).ThenReturn(r);
  });
  return r;
}

} // namespace AlgAudio

#endif // LATERETURN_HPP
