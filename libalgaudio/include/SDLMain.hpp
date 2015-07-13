#ifndef SDLMAIN_HPP
#define SDLMAIN_HPP
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
