#ifndef UIWINDOW_HPP
#define UIWINDOW_HPP
#include "SDLHandle.hpp"

struct SDL_Window;
struct SDL_Renderer;

namespace AlgAudio{

class UIWindow{
private:
  SDLHandle h;
public:
  UIWindow(std::string title = "AlgAudio", int w = 100, int h = 100);
  ~UIWindow();
  void ProcessCloseEvent();

private:
  std::string title;
  int width;
  int height;
  SDL_Window* window;
  SDL_Renderer* renderer;

};

} //namespace AlgAudio
#endif // UIWINDOW_HPP
