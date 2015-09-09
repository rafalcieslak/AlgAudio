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

/** A DrawContext is a universal drawing and rendering proxy.
 *  It provides a clean interface for drawing some primitives.
 *  It carries a context stack, which may be stacked on with a new clipping
 *  dimentions, or a new target texture. This way when using a context
 *  passed from a parent call, one does not need to bother about binding a
 *  correct texture, or applying a correct offset, as all operations done on
 *  DrawContext ensure all this.
 *
 *  The workflow with DrawContext is usually similar in all cases. Mose of the
 *  time, when performing any drawings, you do not create a new context, but
 *  are given one (e.g. UIWidget::CustomDraw()). The context you were given
 *  is bound to some texture, zoom and offset, but you should not care about it,
 *  whoever gave you that context wants you to draw your drawing there.
 *  You can check the size you have available for drawing with
 *  DrawContext::Size(). Then use operations such as SetColor, DrawLine and
 *  DrawTexture to perform your drawing. If you want someone else to fill a
 *  part of your drawing, use Push(Point2D, Size2D) to specify the new draw
 *  area, and then pass your current context to be drawn at. All subsequent
 *  draw operations will automatically have an offset applied, according to
 *  what you passed as an argument to Push. When done, call Pop to have your
 *  context return to the previous state (all drawings are kept), so you can
 *  continue drawing on the area you were originally given.
 *
 *  For examples on how to use a DrawContext, see the implementation of
 *  CustomDraw method in any widget.
 */
class DrawContext{
public:
  DrawContext() {};
  /** Creates a new DrawContext bound to a particuar SDL window and renderer as
   *  well as a GLcontext, with some initial bounds size. */
  DrawContext(SDL_Window* w, SDL_Renderer* renderer, SDL_GLContext cont, int x, int y, int width, int height);
  
  ///@{
  /** Changes the current color used for drawing. */
  void SetColor(short r, short g, short b, short a = 255);
  void SetColor(const Color&);
  ///@}
  
  ///@{
  /** Draws a line from a point to another. */
  inline void DrawLine(int x1, int y1, int x2, int y2) {DrawLine(Point2D(x1,y1),Point2D(x2,y2));}
  void DrawLine(Point2D from, Point2D to, bool smooth=false);
  void DrawLineEx(float x1, float y1, float x2, float y2, float width = 1.0);
  ///@}
  
  /** Draws a cubic Bezier curve defined by the given 4 points.
   *  \param p1 Point 1 \param p2 Point 2 \param p3 Point 3 \param p4 Point 4
   *  \param width The line width to use.
   *  \param segments The number of straight lines the curve will be divided into. */
  void DrawCubicBezier(Point2D p1, Point2D p2, Point2D p3, Point2D p4, float width, unsigned int segments = 15);
  
  /** Renders the given texture onto the context at point p. */
  void DrawTexture(std::shared_ptr<SDLTexture> texture, Point2D p = Point2D(0,0));
  /** Renders the given text texture onto the context at point p. */
  void DrawText(std::shared_ptr<SDLTextTexture> text, Color c, Point2D p = Point2D(0,0));
  ///@{
  /** Draws a rectangle onto the context. */
  void DrawRect(int x, int y, int w, int h);
  inline void DrawRect(Rect r){DrawRect(r.a.x, r.a.y, r.Size().width, r.Size().height);}
  void DrawRectBorder(Rect r);
  ///@}
  /** Fills the whole context with current color. */
  void Fill();
  /** Makes the whole context area fully transparent. */
  void Clear();
  /** Crears the whole context area to the given color. */
  void Clear(Color);
  /** Returns the current dimentions of the context. */
  Size2D Size() {return Size2D(width,height);}
  /** Returns if the context is currently empty in size. In such case no draw operation will have any effect. */
  bool HasZeroArea();

  /** Pushes the context stact one level down, changing the relative offset
   * and size. Useful for drawing children widgets. */
  void Push(Point2D p, Size2D s);
  /** Pushes the context stack one level down, changing the target texture.
   *  To change the texture to the window associated with the internal renderer,
   *  pass nullptr as the texture. This method is useful for switching targets
   *  and renderning widgets to cache. */
  void Push(std::shared_ptr<SDLTexture>, int width, int height);
  /** Restores one level ot the context stack upwards, reverting to previous
   *  context state. */
  void Pop();

  /** Sets the offset for all draw operations on this context. */
  void SetOffset(Point2D off){offset = off;}
  /** Clears the offset set by SetOffset(). */
  void ResetOffset(){offset = Point2D(0,0);}
  /** Sets the scaling factor for all draw operations on this context. */
  void SetScale(float s){scale = s;}

  /** Reapplies the current context params. It is useful if your context was
   *  messed up by a different context using the same window.
   *  For example, if you use TextRenderer::Render inside a CustomDraw, then
   *  the remporary drawcontext used by TextRenderer will reset the settings
   *  carried by your Draw context, because they will end up sending the same
   *  SDL calls to the same window. Use Restore to fix the context after text
   *  rendering is completed. */
  void Restore();
  
private:
  // Current DrawContext state.
  int width, height;
  int x,y;
  
  Point2D offset;
  /** Base scale is the cumulative scale from all above levels. scale is the
   *  current scale set by this level. Thanks to this differenciation, setting
   *  a scale on any level does not override what upper levels set, but is
   *  accumulated. */
  float base_scale = 1.0, scale = 1.0;
  inline Point2D_<float> Transform(Point2D_<float> p){return p*TotalScale() + offset;}
  inline float TotalScale(){return base_scale * scale;}

  // Internal variables
  SDL_Window* window;
  SDL_Renderer* renderer;
  SDL_GLContext context;

  /** Current render target. nullptr if rendering onto the window.*/
  std::shared_ptr<SDLTexture> current_target = nullptr;
  /** A helper structure for storing context state onto a stack. */
  struct DCLevel{
    DCLevel(std::shared_ptr<SDLTexture> t, int x, int y, int w, int h, Point2D off, float bs, float s) :
      target(t), xoffset(x), yoffset(y), width(w), height(h), offset(off), base_scale(bs), scale(s) {}
    std::shared_ptr<SDLTexture> target;
    int xoffset, yoffset, width, height;
    Point2D offset;
    float base_scale, scale;
  };
  /** The state stack. */
  std::stack<DCLevel> context_stack;
  /** Changes the current SDL render target. */
  void SwitchToTarget(std::shared_ptr<SDLTexture>);
  /** Sets the clip rect according to current offset and size. */
  void UpdateClipRect();
};

} // namespace AlgAudio

#endif // DRAWCONTEXT_HPP
