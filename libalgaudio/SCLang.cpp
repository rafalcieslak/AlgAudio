#include <winsock2.h>
#include "SCLang.hpp"
#include "SCLangSubprocess.hpp"
#include <lo/lo.h>
#include <lo/lo_cpp.h>
#include <iostream>

namespace AlgAudio{

std::unique_ptr<SCLangSubprocess> SCLang::subprocess;
Signal<std::string> SCLang::on_line_received;
lo_address addr;

void SCLang::Start(std::string command){
  if(subprocess) return;
  subprocess = std::make_unique<SCLangSubprocess>(command);
  subprocess->on_line_received.Subscribe([&](std::string l){
    on_line_received.Happen(l);
  });
  // The SC dir should be in current directory.
  // TODO: Check if the directories exist.
  SendInstruction("(\"sc/main.sc\").loadPaths;");
  std::string port = subprocess->WaitForReply("NetAddr.localAddr.port.postln;");
  port = Utilities::SplitString(port,"\n")[1];
  std::cout << "SCLang is using port " << port << std::endl;
  addr = lo_address_new("localhost", port.c_str());
  //subprocess->SendInstruction("OSCFunc.trace(true);");
}
void SCLang::Restart(std::string command){
  Stop();
  Start(command);
}
void SCLang::Stop(){
  subprocess.reset(); // Resets the unique_ptr, not the process.
}
void SCLang::SendInstruction(std::string i){
  if(subprocess) subprocess->SendInstruction(i + "\n");
}
void SCLang::SendOSCSimple(std::string a){
  if(!subprocess) return;
  lo_send(addr, a.c_str(), "");
}
void SCLang::Poll(){
  if(subprocess) subprocess->PollOutput();
}
} // namespace AlgAudio
