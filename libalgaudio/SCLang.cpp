#include "SCLang.hpp"
#include "SCLangSubprocess.hpp"

namespace AlgAudio{

std::unique_ptr<SCLangSubprocess> SCLang::subprocess;
Signal<std::string> SCLang::on_line_received;

void SCLang::Start(std::string command){
  if(subprocess) return;
  subprocess = std::make_unique<SCLangSubprocess>(command);
  subprocess->on_line_received.Subscribe([&](std::string l){
    on_line_received.Happen(l);
  });
}
void SCLang::Restart(std::string command){
  Stop();
  Start(command);
}
void SCLang::Stop(){
  subprocess.reset(); // Resets the unique_ptr, not the process.
}
void SCLang::SendInstruction(std::string i){
  if(subprocess) subprocess->SendInstruction(i);
}
void SCLang::Poll(){
  if(subprocess) subprocess->PollOutput();
}
} // namespace AlgAudio
