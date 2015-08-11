#ifndef OSC_HPP
#define OSC_HPP
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
#include <string>
#include <map>
#include <list>
#include <thread>
#include <mutex>
#include <functional>
#ifndef __unix__
  #include <winsock2.h>
  // Othrewise lo is confused when building under MSYS
  #define WIN32
#endif
#include <lo/lo_cpp.h>
#include "Utilities.hpp"

namespace AlgAudio{

struct OSCException : public Exception{
  OSCException(std::string t) : Exception(t) {}
};

template<typename T>
inline T UnpackLOMessage(const lo::Message& msg, unsigned int n);
template<>
inline int UnpackLOMessage<int>(const lo::Message& msg, unsigned int n){ return msg.argv()[n]->i32; }
template<>
inline float UnpackLOMessage<float>(const lo::Message& msg, unsigned int n){ return msg.argv()[n]->f; }
template<>
inline double UnpackLOMessage<double>(const lo::Message& msg, unsigned int n){ return msg.argv()[n]->d; }
template<>
inline std::string UnpackLOMessage<std::string>(const lo::Message& msg, unsigned int n){ return std::string(&msg.argv()[n]->s); }

class OSC{
public:
  OSC(std::string address, std::string port);
  ~OSC();
  void Send(std::string path);
  void Send(std::string path, lo::Message);
  void Send(std::string path, std::function<void(lo::Message)> reply_action, lo::Message);
  //void Send(lo::Message msg, );
  void SetSendreplyCacher(std::function<void(int, int, float)> f){
    sendreply_catcher = f;
  }

  void TriggerReplies();

private:
  std::unique_ptr<lo::ServerThread> server;
  lo::Address addr;

  std::function<void(int, int, float)> sendreply_catcher;

  static int msg_id;

  std::recursive_mutex osc_mutex;
  std::map<  int, std::function< void(lo::Message) > > waiting_for_reply;
  std::list< std::function<void()> > replies_to_call;
};

} // namespace AlgAudio

#endif // OSC_HPP
