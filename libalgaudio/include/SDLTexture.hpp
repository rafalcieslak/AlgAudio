#ifndef SDLTEXTURE_HPP
#define SDLTEXTURE_HPP
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
#include "Color.hpp"
#include "SDLHandle.hpp"
#include <memory>

struct SDL_Texture;
struct SDL_Surface;

namespace AlgAudio{

class Window;

/** A wrapper class for operations on an SDL_Texture */
class SDLTexture{
public:
  SDLTexture(std::weak_ptr<Window> parent_window, Size2D size);
  SDLTexture(std::weak_ptr<Window> parent_window, SDL_Surface*);
  ~SDLTexture();
  //SDLTexture(SDLTexture&& other);
  //SDLTexture& operator=(SDLTexture&& other);
  SDLTexture(const SDLTexture& other) = delete; // No copyconstructing
  SDLTexture& operator=(const SDLTexture& other) = delete; // No copying
  void Resize(Size2D size);
  Size2D GetSize() {return size;}
  friend class DrawContext;
private:
  SDLHandle handle;
  SDL_Texture* texture;
  // Creating a textire of 0 size will mark it as invalid.
  bool valid = true;
  std::weak_ptr<Window> parent;
  Size2D size;
};

/** A speciallized texture type. Text textures are cached in 100% white, and
 *  modulated to desired color when rendering onto another texture. This is
 *  because alpha blend cache misbehaves when rendering onto an empty cache
 *  textures, leading to artefacts on letter edges. */
class SDLTextTexture : public SDLTexture{
public:
  SDLTextTexture(std::weak_ptr<Window> parent_window, SDL_Surface* s) :
    SDLTexture(parent_window,s) {}
  SDLTextTexture(std::weak_ptr<Window> parent_window, Size2D s) :
    SDLTexture(parent_window,s) {}
};

} // namespace AlgAudio

#endif // SDLTEXTURE_HPP
