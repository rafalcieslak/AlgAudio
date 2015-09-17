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

#include "Config.hpp"
#include "Utilities.hpp"

namespace AlgAudio{

std::unique_ptr<Config> Config::global;

const Config& Config::Global(){
  if(!global) global = std::make_unique<Config>(Default());
  return *global;
}
Config& Config::GlobalWriteable(){
  if(!global) global = std::make_unique<Config>(Default());
  return *global;
}

Config Config::Default(){
  Config c;
  c.use_sc = true;
  c.supernova = false;
  c.path_to_sclang = Utilities::FindSCLang();
  c.debug = false;
  c.debug_osc = false;
  c.scsynth_audio_driver_name = ""; // default audio driver
  c.sample_rate = 44100;
  c.input_channels = 2;
  c.output_channels = 2;
  c.block_size = 64;
  return c;
}

} // namespace AlgAudio
