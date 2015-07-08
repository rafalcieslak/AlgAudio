#ifndef SDLMAIN_HPP
#define SDLMAIN_HPP
#include <list>
#include <memory>
#include <atomic>

#include "UI/UIWindow.hpp"

namespace AlgAudio{

class SDLMain{
private:
  SDLMain() = delete; // static class
public:
  static void Quit();
  // Run is not re-entrant!
  // Also, it will block for at least until Quit is called.
  static void Run();
  // A registered window is managed and rendered as SDLMain sees fit.
  // This is handy it you wish to create a window and forget about it,
  // as SDLMain will then sustain the ownership of the window, and it will keep
  // redrawing it when needed.
  // However, if you wish to redraw a window on your own, you should not
  // register it.
  static void RegisterWindow(std::shared_ptr<UIWindow>);
  static void UnregisterWindow(std::shared_ptr<UIWindow>);
private:
  static std::atomic_bool keep_running;
  static std::list<std::shared_ptr<UIWindow>> registered_windows;
  static ProcessEvents();
};

} // namespace AlgAudio

#endif // SDLMAIN_HPP
