#ifndef DRAWCONTEXT_HPP
#define DRAWCONTEXT_HPP
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

#include <stack>
#include <memory>
#include "Color.hpp"
#include "Utilities.hpp"

struct SDL_Renderer;
struct SDL_Texture;
struct SDL_Color;
struct SDL_Rect;
struct SDL_Window;
typedef void* SDL_GLContext;

namespace AlgAudio{

class SDLTexture;
class SDLTextTexture;

// A DrawContext is defined by it's bounds (x/y/width/height). It serves as
// a handy offset/clip proxy to an SDL2 renderer.
class DrawContext{
public:
  DrawContext() {};
  DrawContext(SDL_Window* w, SDL_Renderer* renderer, SDL_GLContext cont, int x, int y, int width, int height);
  void SetColor(short r, short g, short b, short a = 255);
  void SetColor(const Color&);
  void DrawLine(int x1, int y1, int x2, int y2) {DrawLine(Point2D(x1,y1),Point2D(x2,y2));}
  void DrawLine(Point2D from, Point2D to, bool smooth=false);
  void DrawCubicBezier(Point2D p1, Point2D p2, Point2D p3, Point2D p4, unsigned int lines = 15);
  void DrawTexture(std::shared_ptr<SDLTexture> texture, int x = 0, int y = 0);
  void DrawText(std::shared_ptr<SDLTextTexture> text, Color c, int x = 0, int y = 0);
  // Draws the text texture onto a clear cache. Does not blend anything, this is useful
  void DrawTextClear(std::shared_ptr<SDLTextTexture> text, Color c, int x = 0, int y = 0);
  void DrawRect(int x, int y, int w, int h);
  void Fill();
  void Clear();
  void Clear(Color);
  Size2D Size() {return Size2D(width,height);}
bool HasZeroArea();

  void Push(Point2D p, Size2D s);
  void Push(std::shared_ptr<SDLTexture>, int width, int height);
  void Pop();
private:
  int width, height;
  int x,y;
  SDL_Window* window;
  SDL_Renderer* renderer;
  SDL_GLContext context;

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
