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

#include "ParamController.hpp"
#include "SCLang.hpp"
#include "Module.hpp"
#include <unordered_set>

namespace AlgAudio{

ParamController::ParamController(std::shared_ptr<Module> m, const std::shared_ptr<ParamTemplate> t)
  : id(t->id), templ(t), range_min(t->default_min), range_max(t->default_max), module(m)
{
}
std::shared_ptr<ParamController> ParamController::Create(std::shared_ptr<Module> m, std::shared_ptr<ParamTemplate> templ)
{
  return std::shared_ptr<ParamController>(new ParamController(m,templ));
}

void ParamController::Set(float value){
  ///@{
  /** These structures prevent stack overflows when setting one param results
   *  in setting another. This might lead to loops, and thus setting same
   *  params endlessly. To prevent this, whenever any param is set, it sets
   *  inside_set_chain flag, and stores its address in already_set set. If the
   *  action of any handlers to on_set or after_set results in setting som other
   *  param, it will add itself to the set. If at any point of such chain the 
   *  same param would be set again, it will find itself in already_set, and
   *  will ignore new value.
   *  Note that these variables are declared static.
   */
  static bool inside_set_chain = false;
  static std::unordered_set<ParamController*> already_set;
  ///@}
  
  bool this_is_set_chain_root = false;
  if(!inside_set_chain){
    // Apparently this is the beggining of a set-chain.
    inside_set_chain = true;
    this_is_set_chain_root = true;
    already_set.insert(this);
  }else{
    if(already_set.find(this) != already_set.end()) // If already set...
      return; // IGNORE this Set() to avoid passing  parameter data in a loop!
    already_set.insert(this);
  }
  
  if(templ->step > 0.0f){
    value = round(value/templ->step)*templ->step;
  }
  current_val = value;
  
  float relative = GetRelative();
  on_set.Happen(current_val, relative);
  
  auto m = module.lock();
  if(m){
    if(templ->action == ParamTemplate::ParamAction::SC){
      SCLang::SendOSC("/algaudioSC/setparam", "isf", m->sc_id, templ->id.c_str(), value);
    }else if(templ->action == ParamTemplate::ParamAction::Custom){
      m->on_param_set(templ->id, value);
    }else if(templ->action == ParamTemplate::ParamAction::None){
      // NOP
    }
  }
  after_set.Happen(value, relative);
  
  if(this_is_set_chain_root){
    // Cleanup
    already_set.clear();
    inside_set_chain = false;
  }
}

void ParamController::SetRelative(float q){
  if(templ->scale == ParamTemplate::ParamScale::Linear){
    Set(range_min + q*(range_max - range_min));
  }else if(templ->scale == ParamTemplate::ParamScale::Logarithmic){
    Set(range_min * pow(range_max / range_min, q));
  }else{
    // ??? Wil not happen.
  }
}

float ParamController::GetRelative() const {
  if(templ->scale == ParamTemplate::ParamScale::Linear){
    return (current_val - range_min)/(range_max - range_min);
  }else if(templ->scale == ParamTemplate::ParamScale::Logarithmic){
    return log(current_val / range_min) / log(range_max / range_min);
  }else{
    return 0.0; // ??? Will not happen.
  }
}

void ParamController::Reset(){
  Set(templ->default_val);
}

SendReplyController::SendReplyController(std::shared_ptr<Module> m, std::string i, std::shared_ptr<ParamController> ctrl) : id(i), controller(ctrl), module(m){
}
std::shared_ptr<SendReplyController> SendReplyController::Create(std::shared_ptr<Module> m, std::string id, std::shared_ptr<ParamController> ctrl){
  auto res = std::shared_ptr<SendReplyController>( new SendReplyController(m, id, ctrl) );
  res->module_id = m->sc_id;
  res->sendreply_id = SCLang::RegisterSendReply(m->sc_id, res);
  // TODO: Specialize /setparam into /bindsendreply
  SCLang::SendOSC("/algaudioSC/setparam", "isi", m->sc_id, id.c_str(), res->sendreply_id);
  return res;
}
SendReplyController::~SendReplyController(){
  SCLang::UnregisterSendReply(module_id, sendreply_id);
}
  
} // namespace AlgAudio
