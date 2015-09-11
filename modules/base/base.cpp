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
#include "Module.hpp"
#include <cstring>
#include <iostream>
#include "SCLang.hpp"
#include "Timer.hpp"

class MIDICtrl : public AlgAudio::Module{
public:
  void on_init(){
    subscriptions += AlgAudio::SCLang::on_midi_message_received.Subscribe([this](AlgAudio::MidiMessage m){
      // Ignore non-control messages
      if(m.type != AlgAudio::MidiMessage::Type::Control) return;
      // Ignore channel
      // Test ctrl no.
      if(m.number > 7) return;
      // Set the param controler.
      GetParamControllerByID( "ctrl" + std::to_string(m.number) )->Set(m.value);
    });
  }
};

class MIDINote : public AlgAudio::Module{
  std::shared_ptr<AlgAudio::ParamController> gate, note, velocity;
  int notecount;
public:
  void on_init(){

    note     = GetParamControllerByID("note");
    velocity = GetParamControllerByID("velocity");
    gate     = GetParamControllerByID("gate");

    subscriptions += AlgAudio::SCLang::on_midi_message_received.Subscribe([this](AlgAudio::MidiMessage m){
      if(m.type == AlgAudio::MidiMessage::Type::NoteOn){
        note->Set( AlgAudio::Utilities::mtof(m.number) );
        velocity->Set( m.velocity );
        notecount++;
        gate->Set(notecount);
      }else if(m.type == AlgAudio::MidiMessage::Type::NoteOff){
        notecount--;
        gate->Set(notecount);
      }
    });
  }
};

class DataLin : public AlgAudio::Module{
public:
    void on_param_set(std::string, float){
      GetParamControllerByID("d")->Set(
        GetParamControllerByID("a")->Get() * GetParamControllerByID("b")->Get() + GetParamControllerByID("c")->Get()
      );
    }
};

class Seq8 : public AlgAudio::Module{
public:
  int i = 7;
  float fill = 0.8;
  void on_init(){
    step();
  }
  void step(){
    i = (i+1)%8;
    int note = GetParamControllerByID("note" + std::to_string(i+1))->Get();
    float period = GetParamControllerByID("period")->Get();
    GetParamControllerByID("freq")->Set( AlgAudio::Utilities::mtof(note) );
    GetParamControllerByID("gate")->Set(1.0f);
    timerhandles += AlgAudio::Timer::Schedule(period, [this](){
      step();
    });
    timerhandles += AlgAudio::Timer::Schedule(period * fill, [this](){
      GetParamControllerByID("gate")->Set(0.0f);
    });
  }
  
};

extern "C"{
void delete_instance(void* obj){
  delete reinterpret_cast<AlgAudio::Module*>(obj);
}
void* create_instance(const char* name){
  if(strcmp(name,"MIDICtrl")==0) return new MIDICtrl();
  if(strcmp(name,"MIDINote")==0) return new MIDINote();
  if(strcmp(name,"DataLin")==0) return new DataLin();
  if(strcmp(name,"Seq8")==0) return new Seq8();
  return nullptr;
}

} // extern C
