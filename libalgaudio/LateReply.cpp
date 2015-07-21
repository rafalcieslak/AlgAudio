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
#include "LateReply.hpp"

namespace AlgAudio{

std::map<unsigned int, Sync::SyncEntry*> Sync::entries;
unsigned int Sync::id_counter = 0;
std::map<int, LateReplyEntryBase*> LateReplyEntryBase::entries;
int LateReplyEntryBase::id_counter;

Sync::Sync(int count) : id(id_counter++){
  entries[id] = new SyncEntry(count);
}

void Sync::WhenAll(std::function<void()> f) const{
  auto it = entries.find(id);
  if(it == entries.end()){
    std::cout << "Sync store called for unexisting entry" << std::endl;
    return;
  }
  SyncEntry* entry = it->second;
  if(entry->count <= 0){
    f();
    delete entry;
    entries.erase(it);
  }else{
    entry->stored_func = f;
    entry->stored = true;
  }
}
void Sync::Trigger() const{
  auto it = entries.find(id);
  if(it == entries.end()){
    std::cout << "Sync trigger called for unexisting entry" << std::endl;
    return;
  }
  SyncEntry* entry = it->second;
  entry->count--;
  if(entry->stored && entry->count <= 0){
    entry->stored_func();
    delete entry;
    entries.erase(it);
  }
}

} // namespace AlgAudio
