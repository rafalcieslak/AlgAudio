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

namespace AlgAudio{

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
  // TODO: Return a handle
  static void Schedule(float seconds, std::function<void()> f);
  
  /** Triggers a stored timer callback. Called by SDLMain when a timer event is
   *  received. */
  static void Trigger(void* param);
  
private:
  static std::map<int, std::function<void()>> awaiting_callbacks;
  static int counter;
};
  
} // namespace AlgAudio

#endif // TIMER_HPP
