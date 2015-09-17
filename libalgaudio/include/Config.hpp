#ifndef CONFIG_HPP
#define CONFIG_HPP
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

#include <string>
#include <memory>

namespace AlgAudio{

/** This class provides configuration settings for AlgAudio. This includes 
 *  supercollider's audio settings. The static member Global() returns a
 *  reference to an instance of this class which shall be used when accessing
 *  configuration. */
class Config{
public:
	/** This flag marks whether we use supercollider or not. Usually supercollider
	 *  is in use, but the user can manually specify not to use it. In such case,
	 *  no audio processing will happen. This may be, however, useful, when, for
	 *  example, testing UI. */
	bool use_sc;
	/** If set to true, supernova will be launched instead of scsynth. This setting
	 *  has no effect on Windows, which does not support supernova. */
	bool supernova;
	/** False by default. If set to true, various debuggin features will be enabled,
	 *  including sc console window. */
	bool debug;
	/** False by default. If set to true, will enable OSCFunc.trace in sclang. */
	bool debug_osc;
	/** The path to sclang binary executable file. */
	std::string path_to_sclang;
	/** The name of driver device to be used by scsynth for audio I/O. If set to
	 *  an empty string, scsynth will use the default device. */
	std::string scsynth_audio_driver_name;
	
	int  input_channels;
	int output_channels;
	
	int sample_rate;
	int block_size;
	
	Config(const Config& other) = default;
	
	/** Reuturns a reference to the global Config instance. Usually, you will
	 *  access configuration properties this way:
	 *  \code
	 *    ... = Config::Global().path_to_sclang;
	 *  \endcode */
	static const Config& Global();
	static Config& GlobalWriteable();
	static Config Default();
	
	// TODO: Store/load to/from file
	
private:
	Config() {};
	static std::unique_ptr<Config> global;
};

} // namespace AlgAudio

#endif // CONFIG_HPP
