#ifndef SDLFIX_HPP
#define SDLFIX_HPP
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

struct SDL_Surface;
struct SDL_Renderer;

typedef struct { // This struct is not exposed by SDL API, but we need it.
  float x;
  float y;
} SDL_FPoint;

namespace AlgAudio{

class SDLFix{
public:
  // Mutliplies all pixel colors in a surface by alpha.
  static void PremultiplySurface32RGBA(SDL_Surface* surf);
  // This function substitutes several method pointers inside the renderer,
  // so that fixed (or modified) versions of several rendering functions are
  // used whenever SDL calls any of these methods on the renderer.
  static void FixRenderer(SDL_Renderer* renderer);
  // This function corrects the renderers internal blendmode state. The internal
  // formulas SDL uses are wrong, incorrect, ugly, and do not support
  // premultiplied alpha. Call this function on your renderer after calling
  // SetBlendMode(BLENDMODE_BLEND), and make sure your textures use an invalid
  // blendmode, otherwise SDL will reset
  static void CorrectBlendMode(SDL_Renderer* renderer);
  // Similar to SDL_RenderDrawLines, but acccepts SDL_FPoints instead of SDL_Points.
  static void RenderDrawLines(SDL_Renderer* renderer, const SDL_FPoint* points, int count);
private:
  SDLFix() = delete;
};

} // namespace AlgAudio

#endif // SDLFIX_HPP
