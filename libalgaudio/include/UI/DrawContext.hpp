#ifndef DRAWCONTEXT_HPP
#define DRAWCONTEXT_HPP

#include <stack>
#include <memory>
#include "Utilities.hpp"

struct SDL_Renderer;
struct SDL_Texture;
struct SDL_Color;
struct SDL_Rect;

namespace AlgAudio{

class SDLTexture;

// A DrawContext is defined by it's bounds (x/y/width/height). It serves as
// a handy offset/clip proxy to an SDL2 renderer.
class DrawContext{
public:
  DrawContext() {};
  DrawContext(SDL_Renderer* renderer, int x, int y, int width, int height);
  int width, height;
  void SetColor(short r, short g, short b, short a = 255);
  void SetColor(const SDL_Color&);
  void DrawLine(int x1, int y1, int x2, int y2);
  void DrawTexture(std::shared_ptr<SDLTexture> texture);
  void Clear();
  Size2D Size() {return Size2D(width,height);}
  bool HasZeroArea();

  void Push(int x1, int y1, int width, int height);
  void Push(std::shared_ptr<SDLTexture>, int width, int height);
  void Pop();
private:
  int x,y;
  SDL_Renderer* renderer;

  std::shared_ptr<SDLTexture> current_target = nullptr;
  struct DCLevel{
    DCLevel(std::shared_ptr<SDLTexture> t, int x, int y, int w, int h) :
      target(t), xoffset(x), yoffset(y), width(w), height(h) {}
    std::shared_ptr<SDLTexture> target;
    int xoffset, yoffset, width, height;
  };
  std::stack<DCLevel> context_stack;
  void SwitchToTarget(std::shared_ptr<SDLTexture>);
  void UpdateClipRect();
};

} // namespace AlgAudio

#endif // DRAWCONTEXT_HPP
