#include <winsock2.h>
#include "SCLang.hpp"
#include "SCLangSubprocess.hpp"
#include <lo/lo.h>
#include <lo/lo_cpp.h>

namespace AlgAudio{

std::unique_ptr<SCLangSubprocess> SCLang::subprocess;
Signal<std::string> SCLang::on_line_received;

void SCLang::Start(std::string command){
  if(subprocess) return;
  subprocess = std::make_unique<SCLangSubprocess>(command);
  subprocess->on_line_received.Subscribe([&](std::string l){
    on_line_received.Happen(l);
  });
  // The SC dir should be in current directory.
  // TODO: Check if the directories exist.
  SendInstruction("(\"sc/main.sc\").loadPaths;");
  SendInstruction("NetAddr.localAddr.postln;");
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
  lo::Address("localhost",57121).send(a);
}
void SCLang::Poll(){
  if(subprocess) subprocess->PollOutput();
}
} // namespace AlgAudio
