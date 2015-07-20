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
Signal<> SCLang::on_server_started;
Signal<int,std::string> SCLang::on_start_progress;
bool SCLang::osc_debug = false;
bool SCLang::ready = false;
std::unique_ptr<OSC> SCLang::osc;

void SCLang::Start(std::string command){
  if(subprocess) return;
  subprocess = std::make_unique<SCLangSubprocess>(command);
  subprocess->on_any_line_received.SubscribeForever([&](std::string l){
    on_line_received.Happen(l);
  });
  subprocess->on_started.SubscribeOnce([](){ SCLang::Start2(); });
  on_start_progress.Happen(1,"Starting SCLang...");
  subprocess->Start();
}
void SCLang::Start2(){
  // The SC dir should be in current directory.
  SetOSCDebug(osc_debug);
  on_start_progress.Happen(3,"Loading scripts...");
  // TODO: Check if the directories and files exist.
  std::string path_to_scripts = Utilities::ConvertOSpathToUniPath( Utilities::GetCurrentDir() ) + "/" + "sc";
  SendInstruction("(\"" + path_to_scripts + "/main.sc\").loadPaths;");
  subprocess->SendInstruction("NetAddr.localAddr.port.postln;", [&](std::string port){
    port = Utilities::SplitString(port,"\n")[1];
    std::cout << "SCLang is using port " << port << std::endl;
    on_start_progress.Happen(4,"Starting OSC...");
    osc = std::make_unique<OSC>("localhost", port);
    SendOSCSimple([](lo::Message){
      on_start_progress.Happen(5,"Booting server...");
      on_server_started.SubscribeOnce([&](){
        ready = true;
        on_start_progress.Happen(8,"Complete");
        on_start_completed.Happen();
      });
      BootServer();
    }, "/algaudioSC/hello");
  });
}

void SCLang::Restart(std::string command){
  Stop();
  Start(command);
}
void SCLang::Stop(){
  ready = false;
  subprocess.reset(); // Resets the unique_ptr, not the process.
  osc.reset();
}
void SCLang::SendInstruction(std::string i){
  if(subprocess) subprocess->SendInstruction(i);
}
void SCLang::SendOSCSimple(std::string a){
  if(!osc) return;
  osc->Send(a);
}
void SCLang::SendOSCSimple(std::function<void(lo::Message)> f, std::string a){
  if(!osc) return;
  lo::Message m;
  osc->Send(a, f, m);
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
void SCLang::BootServer(bool supernova){
  // TODO: Device selection
  SendInstruction("s.options.device = \"ASIO\"");
  // TODO: Server options
  if(supernova) SendInstruction("Server.supernova;");
  else SendInstruction("Server.scsynth;");
  SendOSCSimple([&](lo::Message m){
    int status = m.argv()[0]->i32;
    if(status){
      on_server_started.Happen();
    }else{
      std::cout << "WARNING: sc server failed to boot!" << std::endl;
    }
  },"/algaudioSC/boothelper");
}
void SCLang::StopServer(){
  SendInstruction("s.quit;");
}

void SCLang::DebugQueryInstalled(){
  SendOSCSimple("/algaudioSC/listall");
}

} // namespace AlgAudio
