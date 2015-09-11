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
#include "SCLang.hpp"
#include <cstring>
#include <iostream>

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

extern "C"{
void delete_instance(void* obj){
  delete reinterpret_cast<AlgAudio::Module*>(obj);
}
void* create_instance(const char* name){
  if(strcmp(name,"MIDICtrl")==0) return new MIDICtrl();
  if(strcmp(name,"MIDINote")==0) return new MIDINote();
  if(strcmp(name,"DataLin")==0) return new DataLin();
  return nullptr;
}

} // extern C
