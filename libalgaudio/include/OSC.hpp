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

namespace AlgAudio{

class OSC{
public:
  OSC(std::string address, std::string port);
  ~OSC();
  void Send(std::string path);
  void Send(std::string path, lo::Message);
  void Send(std::string path, std::function<void(lo::Message)> reply_action, lo::Message);
  //void Send(lo::Message msg, );
  void TriggerReplies();
private:
  std::unique_ptr<lo::ServerThread> server;
  lo::Address addr;

  static int msg_id;

  std::recursive_mutex osc_mutex;
  std::map<  int, std::function< void(lo::Message) > > waiting_for_reply;
  std::list< std::function<void()> > replies_to_call;
};

} // namespace AlgAudio

#endif // OSC_HPP
