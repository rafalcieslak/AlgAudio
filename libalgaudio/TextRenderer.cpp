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
#include "TextRenderer.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "SDLTexture.hpp"
#include "Theme.hpp"
#include <iostream>

namespace AlgAudio{

std::map<FontParrams, TTF_Font*> TextRenderer::fontbank;

TTF_Font* TextRenderer::GetFont(FontParrams fp){
  auto it = fontbank.find(fp);
  if(it != fontbank.end()) return it->second;
  return Preload(fp);
}

TTF_Font* TextRenderer::Preload(FontParrams fp){
  std::string path = "fonts/" + fp.name + ".ttf";
  TTF_Font* f = TTF_OpenFont(path.c_str(), fp.size);
  if(!f) throw SDLException("TTF_OpenFont failed");
  fontbank[fp] = f;
  return f;
}

std::shared_ptr<SDLTextTexture> TextRenderer::RenderShaded(std::weak_ptr<Window> w,FontParrams fp, std::string text){
  // Woarkaround for rendering empty strings
  if(text == "") text = " ";

  SDL_Surface* surf = TTF_RenderUTF8_Shaded(GetFont(fp), text.c_str(), SDL_Color{255,255,255,255}, SDL_Color{0,0,0,0});
  if(!surf)
    std::cout << "Warning: TTF_RenderUTF8_Shaded failed " << TTF_GetError() << std::endl;
  auto res = std::make_shared<SDLTextTexture>(w, surf);
  SDL_FreeSurface(surf);
  return res;
}
std::shared_ptr<SDLTextTexture> TextRenderer::Render(std::weak_ptr<Window> w,FontParrams fp, std::string text){
  // Woarkaround for rendering empty strings
  if(text == "") text = " ";

  SDL_Surface* surf = TTF_RenderUTF8_Blended(GetFont(fp), text.c_str(), SDL_Color{255,255,255,255});
  if(!surf)
    std::cout << "Warning: TTF_RenderUTF8_Blended failed " << TTF_GetError() << std::endl;
  SDLTexture::PremultiplySurface32RGBA(surf);
  auto res = std::make_shared<SDLTextTexture>(w, surf);
  SDL_FreeSurface(surf);
  return res;
}
} // namespace AlgAudio
