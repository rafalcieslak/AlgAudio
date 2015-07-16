#ifndef OSC_HPP
#define OSC_HPP
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
