#ifndef DRAWCONTEXT_HPP
#define DRAWCONTEXT_HPP

struct SDL_Renderer;
struct SDL_Texture;
struct SDL_Color;
struct SDL_Rect;

namespace AlgAudio{

class DrawContext{
public:
  DrawContext() {};
  DrawContext(SDL_Renderer* renderer, int x, int y, int width, int height);
  int width, height;
  void SetColor(short r, short g, short b, short a = 255);
  void SetColor(const SDL_Color&);
  void DrawLine(int x1, int y1, int x2, int y2);
  void DrawTexture(SDL_Texture* texture, const SDL_Rect* srcrect, const SDL_Rect* dstrect);
  DrawContext SubContext(int x1, int y1, int width, int height);
private:
  int x, y;
  SDL_Renderer* renderer;
};

} // namespace AlgAudio

#endif // DRAWCONTEXT_HPP
