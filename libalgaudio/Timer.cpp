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

#include "Timer.hpp"
#include <SDL2/SDL.h>
#include "SDLMain.hpp"

namespace AlgAudio{
  
std::map<int, std::function<void()>> awaiting_callbacks;
int Timer::counter = 0;

static void send_event(void* param){
  SDL_Event event;
  event.type = SDL_USEREVENT;
  event.user.code = SDLMain::CustomEventCodes::NOTIFY_TIMER;
  event.user.data1 = param;
  event.user.data2 = NULL;
  SDL_PushEvent(&event);
}

static unsigned int timer_callback_once(unsigned int, void* param){
  send_event(param);
  return 0;
}
/*
static unsigned int timer_callback_repeat(unsigned int interval, void* param){
  send_event(param);
  return interval;
}
*/
TimerHandle Timer::Schedule(float seconds, std::function<void()> f){
  int* id = new int(counter++);
  awaiting_callbacks[*id] = f;
  SDL_AddTimer(seconds*1000.0f, timer_callback_once, id);
  return TimerHandle(*id);
}

void Timer::Trigger(void* param){
  int id = *((int*)param);
  delete (int*)param;
  auto it = awaiting_callbacks.find(id);
  if(it == awaiting_callbacks.end()){
    // Apparently the callback has been realeased.
    return;
  }
  std::function<void()> f = it->second;
  awaiting_callbacks.erase(it);
  f();
}

// ====== HANDLE ======

void TimerHandle::Release(){
  auto it = awaiting_callbacks.find(id);
  if(it == awaiting_callbacks.end()){
    // It was already released!
    return;
  }
  awaiting_callbacks.erase(it);
}

TimerHandle::TimerHandle(TimerHandle&& other) : id(std::move(other.id)){
  
}
TimerHandle& TimerHandle::operator=(TimerHandle&& other){
  std::swap(id,other.id);
  return *this;
}

} // namespace AlgAudio
