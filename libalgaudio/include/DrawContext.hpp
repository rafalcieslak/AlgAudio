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

/* A DrawContext is a universal drawing and rendering proxy.
 * It provides a clean interface for drawing some primitives.
 * It carries a context stack, which may be pushed down with a new clipping
 * dimentions, or a new target texture. This way whoever is using a context
 * passed from a parent call, does not need to bother about binding a correct
 * texture, or applying a correct offset, as all operations done on DrawContext
 * ensure all this.
 */
class DrawContext{
public:
  DrawContext() {};
  // Creates a new DrawContext bound to a particuar SDL window and renderer as
  // well as a GLcontext, with some initial bounds size.
  DrawContext(SDL_Window* w, SDL_Renderer* renderer, SDL_GLContext cont, int x, int y, int width, int height);
  // Changes the current color used for drawing.
  void SetColor(short r, short g, short b, short a = 255);
  void SetColor(const Color&);
  // Line drawing methods.
  inline void DrawLine(int x1, int y1, int x2, int y2) {DrawLine(Point2D(x1,y1),Point2D(x2,y2));}
  void DrawLine(Point2D from, Point2D to, bool smooth=false);
  void DrawLineEx(float x1, float y1, float x2, float y2, float width = 1.0);
  void DrawCubicBezier(Point2D p1, Point2D p2, Point2D p3, Point2D p4, unsigned int lines = 15, float width = 2.0);
  // Rendering textures onto a context.
  void DrawTexture(std::shared_ptr<SDLTexture> texture, int x = 0, int y = 0);
  void DrawText(std::shared_ptr<SDLTextTexture> text, Color c, int x = 0, int y = 0);
  // Drawing rectangles.
  void DrawRect(int x, int y, int w, int h);
  inline void DrawRect(Rect r){DrawRect(r.a.x, r.a.y, r.Size().width, r.Size().height);}
  void DrawRectBorder(Rect r);
  // Fills the whole context with current color.
  void Fill();
  void Clear();
  void Clear(Color);
  // Returns the current size of the context.
  Size2D Size() {return Size2D(width,height);}
  // Is the context currently empty?
  bool HasZeroArea();

  // Pushes the context stact one level down, changing the relative offset
  // and size. Useful for drawing children widgets.
  void Push(Point2D p, Size2D s);
  // Pushes the context stack one level down, changing the target texture.
  // To change the texture to the window associated with the internal renderer,
  // pass nullptr as the texture. This method is useful for switching targets
  // and renderning widgets to cache.
  void Push(std::shared_ptr<SDLTexture>, int width, int height);
  // Restores one level ot the context stack upwards, reverting to previous
  // context state.
  void Pop();

  // Reapplies the current context params. It is useful if your context was
  // messed up by a different context using the same window.
  // For example, if you use TextRenderer::Render inside a CustomDraw, then
  // the remporary drawcontext used by TextRenderer will reset the settings
  // carried by your Draw context. Use Restore to fix the context after text
  // rendering is completed.
  void Restore();
private:
  // Current DrawContext state.
  int width, height;
  int x,y;

  // Internal variables
  SDL_Window* window;
  SDL_Renderer* renderer;
  SDL_GLContext context;

  // Current render target.
  std::shared_ptr<SDLTexture> current_target = nullptr;
  // A helper structure for storing context state onto a stack.
  struct DCLevel{
    DCLevel(std::shared_ptr<SDLTexture> t, int x, int y, int w, int h) :
      target(t), xoffset(x), yoffset(y), width(w), height(h) {}
    std::shared_ptr<SDLTexture> target;
    int xoffset, yoffset, width, height;
  };
  // The state stack.
  std::stack<DCLevel> context_stack;
  // Changes the current render target.
  void SwitchToTarget(std::shared_ptr<SDLTexture>);
  // Sets the clip rect according to current offset and size.
  void UpdateClipRect();
};

} // namespace AlgAudio

#endif // DRAWCONTEXT_HPP
