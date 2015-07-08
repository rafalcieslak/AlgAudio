#include "UI/DrawContext.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

namespace AlgAudio{

DrawContext::DrawContext(SDL_Renderer* r, int x_, int y_, int w, int h) :
  width(w), height(h), x(x_), y(y_), renderer(r) { }

void DrawContext::DrawLine(int x1, int y1, int x2, int y2) const{
  SDL_RenderDrawLine(renderer, x+x1, y+y1, x+x2, y+y2);
}

DrawContext DrawContext::SubContext(int x1, int y1, int width_, int height_) const{
  return DrawContext(renderer, x + x1, y + y1, width_, height_);
}

void DrawContext::SetColor(short r, short g, short b, short a) const{
  SDL_SetRenderDrawColor(renderer, r, g, b, a);
}
void DrawContext::SetColor(const SDL_Color& c) const{
  SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
}
bool DrawContext::HasZeroArea() const{
  if(width <= 0 || height <= 0) return true;
  return false;
}

} // namespace AlgAudio
