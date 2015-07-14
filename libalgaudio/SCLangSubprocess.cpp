#include "SCLangSubprocess.hpp"
#include "Color.hpp"
#include "Utilities.hpp"
#include <iostream>
#include <algorithm>

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
  WaitForPrompt();
  at_prompt = false;
  SendData(i + '\n');
}

void SCLangSubprocess::PollOutput(){
  std::string data = ReadData();
  Utilities::Replace(data,"\t","  ");
  data.erase(std::remove(data.begin(), data.end(), '\r'), data.end());
  buffer += data;
  ProcessBuffer();
}

void SCLangSubprocess::WaitForPrompt(){
  while(!at_prompt){
    Utilities::Wait(100);
    PollOutput();
  }
}

std::string SCLangSubprocess::WaitForReply(std::string command){
  SendInstruction(command);
  collecting_reply = true;
  reply_buffer = "";
  WaitForPrompt();
  collecting_reply = false;
  return reply_buffer;
}

void SCLangSubprocess::ProcessBuffer(){
  auto vs = Utilities::SplitString(buffer, "\n");
  buffer = vs.back();
  vs.pop_back();
  for(auto& l : vs){
    on_line_received.Happen(l);
    //if(l.length() >= 5 && l.substr(0,5) == "sc3> "){
      // prompt
    //  at_prompt = true;
      //l = l.substr(5);
    //}
    if(collecting_reply){
      if(reply_buffer.length() == 0) reply_buffer = l;
      else reply_buffer += "\n" + l;
    }
  }
  if(buffer.substr(0,5) == "sc3> "){
    // late prompt
    on_line_received.Happen("sc3> "); // Pretend it is full output (flush)
    buffer = buffer.substr(5);
    at_prompt = true;
  }
}

} // namespace AlgAudio
