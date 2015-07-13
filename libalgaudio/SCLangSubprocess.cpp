#include "SCLangSubprocess.hpp"
#include "Color.hpp"

namespace AlgAudio{

SCLangSubprocess::SCLangSubprocess(std::string command)
 : Subprocess(command)
 {}

SCLangSubprocess::~SCLangSubprocess(){
  // Ask the interpreter to quit now
  SendInstruction("\n\n0.exit;\n\n");
  Utilities::Wait(20); // Give it a chance to close gracefully
  PollOutput(); // We wish to capture the farewell message.
}

void SCLangSubprocess::SendInstruction(std::string i){
  SendData(i);
}

void SCLangSubprocess::PollOutput(){
  std::string data = ReadData();
  Utilities::Replace(data,"\t","  ");
  buffer += data;
  ProcessBuffer();
}

void SCLangSubprocess::ProcessBuffer(){
  auto vs = Utilities::SplitString(buffer, "\n");
  buffer = vs.back();
  vs.pop_back();
  for(auto& l : vs) on_line_received.Happen(l);
  if(buffer.substr(0,5) == "sc3> "){
    // PROMPT!
    on_line_received.Happen("sc3> "); // Pretend it is full output (flush)
    buffer = buffer.substr(5);
  }
}

} // namespace AlgAudio
