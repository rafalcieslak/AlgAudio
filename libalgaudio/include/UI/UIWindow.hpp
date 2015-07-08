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
  static std::shared_ptr<UIWindow> Create(std::string title = "AlgAudio", int w = 350, int h = 300);
  ~UIWindow();

  // explicitly forbid copying windows
  UIWindow(const UIWindow&) = delete;
  UIWindow& operator=(const UIWindow&) = delete;

  void Render();

  void Insert(std::shared_ptr<UIWidget> child);

  void SetNeedsRedrawing();

  void ProcessCloseEvent();
  void ProcessMouseButtonEvent(bool down, short button, int x, int y);
  void ProcessMotionEvent(int x, int y);
  void ProcessEnterEvent();
  void ProcessLeaveEvent();

  unsigned int GetID() const {return id;}
private:
  UIWindow(std::string title, int w, int h);
  std::string title;
  int width;
  int height;
  unsigned int id;
  SDL_Window* window;
  SDL_Renderer* renderer;

  int prev_motion_x = -1, prev_motion_y = -1;

  bool needs_redrawing = true;

  std::shared_ptr<UIWidget> child;
};

} //namespace AlgAudio
#endif // UIWINDOW_HPP
