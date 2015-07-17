#ifndef SDLMAIN_HPP
#define SDLMAIN_HPP
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
#include <map>
#include <memory>
#include <atomic>

#include "Window.hpp"

namespace AlgAudio{

class SDLMain{
private:
  SDLMain() = delete; // static class
public:
  static void Quit();
  // Loop is not re-entrant!
  // Also, it will block for at least until Quit is called.
  static void Loop();
  static void Step();
  // A registered window is managed and rendered as SDLMain sees fit.
  // This is handy it you wish to create a window and forget about it,
  // as SDLMain will then sustain the ownership of the window, and it will keep
  // redrawing it when needed.
  // However, if you wish to redraw a window on your own, you should not
  // register it.
  static void RegisterWindow(std::shared_ptr<Window>);
  static void UnregisterWindow(std::shared_ptr<Window>);
  static void UnregisterAll();
  static unsigned int GetWindowNum() {return registered_windows.size();}

  static std::atomic_bool running;
private:
  static std::map<unsigned int, std::shared_ptr<Window>> registered_windows;
  static void ProcessEvents();
};

} // namespace AlgAudio

#endif // SDLMAIN_HPP
