#ifndef TIMER_HPP
#define TIMER_HPP
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
#include <functional>
#include <map>
#include <list>

namespace AlgAudio{


struct TimerHandle{
public:
  friend class Timer;
  /** Invalidates the represented timer scheduled event, so that it won't
   *  be called anymore. */
  void Release();
  
  /** TimerHandles are movable. */
  TimerHandle(TimerHandle&& other);
  /** TimerHandles are movable. */
  TimerHandle& operator=(TimerHandle&& other);
  
  TimerHandle& operator=(const TimerHandle&) = delete; // No copying.
  TimerHandle(const TimerHandle&) = delete; // No copying.
  TimerHandle() : id(-1) {}
private:
  TimerHandle(int i) : id(i) {}
  int id;
};


/** This class provides time management mechanisms, such as timed calls.
 */
class Timer{
public:
  /** Calling schedule specifies an action that has to happen after some time
   *  has elapsed.
   *  \param seconds A floating point number specifying the number of seconds
   *    to wait before calling f.
   *  \param f The function to be called. It will be invoked by the main thread.
   */
  static TimerHandle Schedule(float seconds, std::function<void()> f);
  
  /** Triggers a stored timer callback. Called by SDLMain when a timer event is
   *  received. */
  static void Trigger(void* param);
  
private:
  static int counter;
};
  
/** A container for timer handles. */
class TimerHandleList{
public:
  TimerHandleList(const TimerHandleList& other) = delete;
  std::list<TimerHandle> list;
  void ReleaseAll() { 
    for(auto& th : list) th.Release();
    list.clear();
  }
  TimerHandleList& operator+=(TimerHandle&& s) {
    list.push_back(std::move(s));
    return *this;
  }
  TimerHandleList() : list(0){}
  ~TimerHandleList() {ReleaseAll();}
};

/** This class provides a single helper member field: timerhandles.
 *  You can use it to store all TimerHandles that shall be released when your
 *  object is destroyed.
 *  Use the += operator to add a TimerHandle to be stored. They will be released
 *  automatically when this object is destroyed.
 *  To use this class, inherit from it, it is recommended to use public *virtual*
 *  inheritance, since a lot of classes uses a SubscriptionsManager as one of its
 *  bases. Also note that may classes (widgets, modules) already inherit from
 *  TimerHandleManager, so you are welcome to use that subscriptions field.
 * \see TimerHandle*/
class TimerHandleManager{
public:
 TimerHandleList timerhandles;
};

} // namespace AlgAudio

#endif // TIMER_HPP
