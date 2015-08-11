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
#include "ModuleCollection.hpp"
#include "Module.hpp"
#include "OSC.hpp"

namespace AlgAudio{

std::unique_ptr<SCLangSubprocess> SCLang::subprocess;
std::set<std::string> SCLang::installed_templates;
Signal<std::string> SCLang::on_line_received;
Signal<bool, std::string> SCLang::on_start_completed;
Signal<bool> SCLang::on_server_started;
Signal<int,std::string> SCLang::on_start_progress;
bool SCLang::osc_debug = false;
bool SCLang::ready = false;
std::unique_ptr<OSC> SCLang::osc;

std::map<std::pair<int,int>, std::weak_ptr<SendReplyController>> SCLang::sendreply_map;
int SCLang::sendreply_id = 0;

void SCLang::Start(std::string command, bool supernova){
  if(subprocess) return;
  subprocess = std::make_unique<SCLangSubprocess>(command);
  subprocess->on_any_line_received.SubscribeForever([&](std::string l){
    on_line_received.Happen(l);
  });
  on_start_progress.Happen(1,"Starting SCLang...");
  subprocess->Start();
  subprocess->on_started.SubscribeOnce([supernova](){
    // The SC dir should be in current directory.
    SetOSCDebug(osc_debug);
    on_start_progress.Happen(3,"Loading scripts...");

    // TODO: Check if the directories and files exist.
    std::string main_script = Utilities::ConvertOSpathToUniPath(Utilities::GetCurrentDir()) + "/sc/main.sc";
    if(!Utilities::GetFileExists( Utilities::ConvertUnipathToOSPath(main_script))){
      on_start_completed.Happen(false,"Main SC scripts are missing");
      return;
    }
    SendInstruction("(\"" + main_script + "\").loadPaths;");

    subprocess->SendInstruction("NetAddr.localAddr.port.postln;", [&,supernova](std::string port){
      port = Utilities::SplitString(port,"\n")[1];
      std::cout << "SCLang is using port " << port << std::endl;
      on_start_progress.Happen(4,"Starting OSC...");
      osc = std::make_unique<OSC>("localhost", port);
      osc->SetSendreplyCacher([](int x, int y, float z){SCLang::SendReplyCatcher(x,y,z);});
      SendOSCWithEmptyReply("/algaudioSC/hello").Then([supernova](){
        on_start_progress.Happen(5,"Booting server...");
        BootServer(supernova);
        on_server_started.SubscribeOnce([&](bool success){
          if(success){
            ready = true;
            on_start_progress.Happen(8,"Installing module templates...");
            ModuleCollectionBase::InstallAllTemplatesIntoSC().Then([=](){
              on_start_progress.Happen(10,"Complete.");
              on_start_completed.Happen(true,"");
            });
          }else{
            // Server failed to boot
            on_start_completed.Happen(false,"SC Server failed to start");
          }
        });
      }); // /algaudioSC/hello
    }); // sendinstruction port
  }); // subprocess started
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
void SCLang::SendOSC(const std::string& path){
  if(!osc) {std::cout << "WARNING: Failed to send OSC message to server, OSC not ready" << std::endl; return;}// throw SCLangException("Failed to send OSC message to server, OSC not yet ready");
  osc->Send(path);
}
void SCLang::SendOSCCustom(const std::string& path, const lo::Message& m){
  if(!osc) {std::cout << "WARNING: Failed to send OSC message to server, OSC not ready" << std::endl; return;}// throw SCLangException("Failed to send OSC message to server, OSC not yet ready");
  osc->Send(path,m);
}
LateReturn<lo::Message> SCLang::SendOSCWithLOReply(const std::string& path){
  auto r = Relay<lo::Message>::Create();
  if(!osc) {std::cout << "WARNING: Failed to send OSC message to server, OSC not ready" << std::endl; return r;}// throw SCLangException("Failed to send OSC message to server, OSC not yet ready");
  lo::Message m;
  osc->Send(path, [=](lo::Message msg){
    r.Return(msg);
  }, m);
  return r;
}
LateReturn<lo::Message> SCLang::SendOSCCustomWithLOReply(const std::string& path, const lo::Message &m){
  auto r = Relay<lo::Message>::Create();
  if(!osc) {std::cout << "WARNING: Failed to send OSC message to server, OSC not ready" << std::endl; return r;}// throw SCLangException("Failed to send OSC message to server, OSC not yet ready");
  osc->Send(path, [=](lo::Message msg){
    r.Return(msg);
  }, m);
  return r;
}
void SCLang::SendOSC(const std::string &path, std::string tag, ...)
{
  if(!osc) {std::cout << "WARNING: Failed to send OSC message to server, OSC not ready" << std::endl; return;}// throw SCLangException("Failed to send OSC message to server, OSC not yet ready");
  va_list q;
  va_start(q, tag);
  lo::Message m;
  std::string t = tag + "$$";
  m.add_varargs(t, q);
  osc->Send(path, m);
}
LateReturn<lo::Message> SCLang::SendOSCWithLOReply(const std::string &path, std::string tag, ...){
  auto r = Relay<lo::Message>::Create();
  if(!osc) {std::cout << "WARNING: Failed to send OSC message to server, OSC not ready" << std::endl; return r;} // throw SCLangException("Failed to send OSC message to server, OSC not yet ready");
  va_list q;
  va_start(q, tag);
  lo::Message m;
  std::string t = tag + "$$";
  m.add_varargs(t, q);
  osc->Send(path, [=](lo::Message msg){
    r.Return(msg);
  }, m);
  return r;
}

void SCLang::PollSubprocess(){
  if(subprocess) subprocess->TriggerSignals();
}
void SCLang::PollOSC(){
  if(osc) osc->TriggerReplies();
}
void SCLang::SetOSCDebug(bool enabled){
  if(enabled) SendInstruction("OSCFunc.trace(true);");
  else SendInstruction("OSCFunc.trace(false);");
  osc_debug = enabled;
}
LateReturn<> SCLang::InstallTemplate(const std::shared_ptr<ModuleTemplate> t){
  auto r = Relay<>::Create();
  if(!t->has_sc_code) return r.Return();
  SendOSCWithEmptyReply("/algaudioSC/installtemplate", "ss", t->GetFullID().c_str(), t->sc_code.c_str()).Then([=](){
    installed_templates.insert(t->GetFullID());
    std::cout << "Got install reply!" << std::endl;
    r.Return();
  });
  return r;
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
  SendOSCWithReply<int>("/algaudioSC/boothelper", "si", "ASIO", (supernova)?1:0).Then([&](int status){
    if(status){
      on_server_started.Happen(true);
    }else{
      std::cout << "WARNING: sc server failed to boot!" << std::endl;
      on_server_started.Happen(false);
    }
  });
}
void SCLang::StopServer(){
  SendInstruction("s.quit;");
}

void SCLang::DebugQueryInstalled(){
  SendOSC("/algaudioSC/listall");
}

void SCLang::SendReplyCatcher(int synth_id, int reply_id, float value){
  auto it = sendreply_map.find({reply_id, synth_id});
  if(it == sendreply_map.end()){
    std::cout << "WARNING: recieved an unregistered sendreply " << synth_id << " " << reply_id << std::endl;
    return;
  }
  auto ctrl = it->second.lock();
  if(!ctrl){
    std::cout << "WARNING: recieved a registered sendreply, but the controller does not exist anymore" << std::endl;
    return;
  }
  ctrl->Got(value);
}

int SCLang::RegisterSendReply(int synth_id, std::weak_ptr<SendReplyController> ctrl){
  sendreply_id++;
  sendreply_map[{sendreply_id, synth_id}] = ctrl;
  return sendreply_id;
}

void SCLang::UnregisterSendReply(int synth_id, int reply_id){
  auto it = sendreply_map.find({reply_id, synth_id});
  if(it == sendreply_map.end()){
    std::cout << "WARNING: failed to unregister a sendreply, it was not registered" << std::endl;
    return;
  }
  sendreply_map.erase(it);
}

} // namespace AlgAudio
