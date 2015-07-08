#ifndef UIWINDOW_HPP
#define UIWINDOW_HPP
#include <memory>
#include "SDLHandle.hpp"
#include "UIWidget.hpp"

struct SDL_Window;
struct SDL_Renderer;

namespace AlgAudio{

class UIWindow : public std::enable_shared_from_this<UIWindow>{
private:
  SDLHandle h;
public:
  UIWindow(std::string title = "AlgAudio", int w = 350, int h = 300);
  ~UIWindow();

  // explicitly forbid copying windows
  UIWindow(const UIWindow&) = delete;
  UIWindow& operator=(const UIWindow&) = delete;

  void Render();

  void Insert(std::shared_ptr<UIWidget> child);

  void ProcessCloseEvent();
  void ProcessMouseButtonEvent(bool down, short button, int x, int y);

  unsigned int GetID() const {return id;}
private:
  std::string title;
  int width;
  int height;
  unsigned int id;
  SDL_Window* window;
  SDL_Renderer* renderer;

  std::shared_ptr<UIWidget> child;
};

} //namespace AlgAudio
#endif // UIWINDOW_HPP
