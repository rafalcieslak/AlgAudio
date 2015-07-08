#ifndef DRAWCONTEXT_HPP
#define DRAWCONTEXT_HPP

struct SDL_Renderer;
struct SDL_Texture;
struct SDL_Color;
struct SDL_Rect;

namespace AlgAudio{

// A DrawContext is defined by it's bounds (x/y/width/height). It serves as
// a handy offset/clip proxy to an SDL2 renderer.
class DrawContext{
public:
  DrawContext() {};
  DrawContext(SDL_Renderer* renderer, int x, int y, int width, int height);
  int width, height;
  void SetColor(short r, short g, short b, short a = 255) const;
  void SetColor(const SDL_Color&) const;
  void DrawLine(int x1, int y1, int x2, int y2) const;
  void DrawTexture(SDL_Texture* texture, const SDL_Rect* srcrect, const SDL_Rect* dstrect) const;
  DrawContext SubContext(int x1, int y1, int width, int height) const;
  bool HasZeroArea() const;
private:
  int x, y;
  // The rendered is marked as mutable, this way a widget can be given a const
  // DC and it is unable to modify its offset of bounds, but can perform
  // (drawing) actions on the renderer.
  mutable SDL_Renderer* renderer;
};

} // namespace AlgAudio

#endif // DRAWCONTEXT_HPP
