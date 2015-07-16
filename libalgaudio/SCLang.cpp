#ifndef __unix
  #include <winsock2.h>
#endif
#include "SCLang.hpp"
#include <lo/lo.h>
#include <lo/lo_cpp.h>
#include <iostream>
#include "SCLangSubprocess.hpp"
#include "ModuleTemplate.hpp"
#include "OSC.hpp"

namespace AlgAudio{

std::unique_ptr<SCLangSubprocess> SCLang::subprocess;
std::set<std::string> SCLang::installed_templates;
Signal<std::string> SCLang::on_line_received;
Signal<> SCLang::on_start_completed;
bool SCLang::osc_debug = false;
std::unique_ptr<OSC> SCLang::osc;

void SCLang::Start(std::string command){
  if(subprocess) return;
  subprocess = std::make_unique<SCLangSubprocess>(command);
  subprocess->on_any_line_received.SubscribeForever([&](std::string l){
    on_line_received.Happen(l);
  });
  subprocess->on_started.SubscribeOnce([](){ SCLang::Start2(); });
  subprocess->Start();
}
void SCLang::Start2(){
  // The SC dir should be in current directory.
  SetOSCDebug(osc_debug);
  // TODO: Check if the directories and files exist.
  SendInstruction("(\"sc/main.sc\").loadPaths;");
  subprocess->SendInstruction("NetAddr.localAddr.port.postln;", [&](std::string port){
    port = Utilities::SplitString(port,"\n")[1];
    std::cout << "SCLang is using port " << port << std::endl;
    osc = std::make_unique<OSC>("localhost", port);
  });
  on_start_completed.Happen();
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
void SCLang::SendOSCSimple(std::string a){
  if(!osc) return;
  osc->Send(a);
}
void SCLang::SendOSC(const std::string &path, const std::string &tag, ...)
{
  if(!osc) return;
  va_list q;
  va_start(q, tag);
  lo::Message m;
  std::string t = tag + "$$";
  m.add_varargs(t, q);
  osc->Send(path, m);
}
void SCLang::SendOSC(std::function<void(lo::Message)> f, const std::string &path, const std::string &tag, ...){
  if(!osc) return;
  va_list q;
  va_start(q, tag);
  lo::Message m;
  std::string t = tag + "$$";
  m.add_varargs(t, q);
  osc->Send(path, f, m);
}

void SCLang::Poll(){
  if(subprocess) subprocess->TriggerSignals();
  if(osc) osc->TriggerReplies();
}
void SCLang::SetOSCDebug(bool enabled){
  if(enabled) SendInstruction("OSCFunc.trace(true);");
  else SendInstruction("OSCFunc.trace(false);");
  osc_debug = enabled;
}
void SCLang::InstallTemplate(const ModuleTemplate& t){
  if(!t.has_sc_code) return;
  SendOSC([&](lo::Message){
    std::cout << "Got install reply!" << std::endl;
  }, "/algaudioSC/installtemplate", "ss", t.GetFullID().c_str(), t.sc_code.c_str());
  installed_templates.insert(t.GetFullID());
}
bool SCLang::WasInstalled(const std::string& s){
  auto it = installed_templates.find(s);
  return (it != installed_templates.end());
}

} // namespace AlgAudio
