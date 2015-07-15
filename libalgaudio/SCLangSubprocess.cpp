#include "SCLangSubprocess.hpp"
#include "Color.hpp"
#include "Utilities.hpp"
#include <iostream>
#include <algorithm>

namespace AlgAudio{

SCLangSubprocess::SCLangSubprocess(std::string cmd) : command(cmd)
 {
   started = false;
   run = false;
 }

SCLangSubprocess::~SCLangSubprocess(){
  Stop();
  the_thread.join();
}

void SCLangSubprocess::Start(){
  the_thread = std::thread(&SCLangSubprocess::ThreadMain, this);
  run = true;
}
void SCLangSubprocess::Stop(){
  run = false;
}

void SCLangSubprocess::ThreadMain(){
  subprocess = std::make_unique<Subprocess>(command);
  started = true;
  while(run) Step();
  // Close everything
  SendInstruction("\n\n0.exit;\n\n");
  Step();
  subprocess = nullptr;
  started = false;
}
void SCLangSubprocess::Step(){

  Utilities::WaitOS(20);
  // Take an instruction, send it, wait for prompt collecting reply, store
  // the reply in out buffer

  io_mutex.lock();
  for(auto& p : instructions_actions){
    std::string reply = WaitForReply(p.first);
    replies.push_back(std::make_pair(reply,p.second));
  }
  for(auto& p : instructions){
    SendInstructionRaw(p);
  }
  instructions_actions.clear();
  instructions.clear();
  io_mutex.unlock();

  PollOutput();
}

void SCLangSubprocess::SendInstruction(std::string i){
  io_mutex.lock();
  instructions.push_back(i);
  io_mutex.unlock();
}
void SCLangSubprocess::SendInstruction(std::string i, std::function<void(std::string)> f){
  io_mutex.lock();
  instructions_actions.push_back(std::make_pair(i,f));
  io_mutex.unlock();
}
void SCLangSubprocess::TriggerSignals(){
  if( ! io_mutex.try_lock()) return;
  if(started != last_started){
    if(started){

      on_started.Happen();
      // autofire
    }else{
      // stop autofire
    }
    last_started = started;
  }
  for(std::string& l : lines_received) on_any_line_received.Happen(l);
  for(auto& p : replies){
     (p.second)(p.first);
  }
  lines_received.clear();
  replies.clear();
  io_mutex.unlock();

  // Call the signals once the mutexes are freed
}



//====================================================================

void SCLangSubprocess::PollOutput(){
  std::string data = subprocess->ReadData();
  Utilities::Replace(data,"\t","  ");
  data.erase(std::remove(data.begin(), data.end(), '\r'), data.end());
  buffer += data;
  ProcessBuffer();
}

void SCLangSubprocess::SendInstructionRaw(std::string i){
  WaitForPrompt();
  prompts--;
  subprocess->SendData(i + '\n');
}

void SCLangSubprocess::WaitForPrompt(){
  while(prompts < 1){
    Utilities::WaitOS(20);
    PollOutput();
  }
}

std::string SCLangSubprocess::WaitForReply(std::string command){
  SendInstructionRaw(command);
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
    io_mutex.lock();
    lines_received.push_back(l);
    io_mutex.unlock();
    if(l.length() >= 5 && l.substr(0,5) == "sc3> "){
      // prompt
      prompts++;
      l = l.substr(5);
    }
    if(collecting_reply){
      if(reply_buffer.length() == 0) reply_buffer = l;
      else reply_buffer += "\n" + l;
    }
  }
  if(buffer.substr(0,5) == "sc3> "){
    // late prompt
    io_mutex.lock();
    lines_received.push_back("sc3> "); // Pretend it is full output (flush)
    io_mutex.unlock();
    buffer = buffer.substr(5);
    prompts++;
  }
}

} // namespace AlgAudio
