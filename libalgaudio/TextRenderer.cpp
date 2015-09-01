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
#include "SDLFix/SDLFix.hpp"
#include "Utilities.hpp"
#include "Window.hpp"
#include "DrawContext.hpp"
#include <iostream>

namespace AlgAudio{

std::map<FontParams, TTF_Font*> TextRenderer::fontbank;

TTF_Font* TextRenderer::GetFont(FontParams fp){
  auto it = fontbank.find(fp);
  if(it != fontbank.end()) return it->second;
  return Preload(fp);
}

TTF_Font* TextRenderer::Preload(FontParams fp){
  std::string path = "fonts/" + fp.name + ".ttf";
  TTF_Font* f = TTF_OpenFont(path.c_str(), fp.size);
  if(!f) throw SDLException("TTF_OpenFont failed");
  fontbank[fp] = f;
  return f;
}
std::shared_ptr<SDLTextTexture> TextRenderer::Render(std::weak_ptr<Window> w,FontParams fp, std::string text){
  auto parent_window = w.lock();
  // Split the text into lines.
  std::vector<std::string> lines = Utilities::SplitString(text,"\n");
  // Buffers.
  std::vector<SDL_Surface*> surfaces;
  std::vector<std::shared_ptr<SDLTexture>> textures;
  // For each line of text
  for(auto& l : lines){
    // Workaround for rendering empty strings
    if(l == "") l = " ";
    // Ask SDL_TTF to render a line
    SDL_Surface* surf = TTF_RenderUTF8_Blended(GetFont(fp), l.c_str(), SDL_Color{255,255,255,255});
    if(!surf){
      std::cout << "Warning: TTF_RenderUTF8_Blended failed " << TTF_GetError() << std::endl;
      continue;
    }
    // Use SDLFix to premultiply alpha
    SDLFix::PremultiplySurface32RGBA(surf);
    // Store the surface in buffer.
    surfaces.push_back(surf);
  }
  int totalh = 0, maxw = 0;
  // When all surfaces are ready, for each surface
  for(auto& surf : surfaces){
    // create corresponding texture.
    textures.push_back(std::make_shared<SDLTextTexture>(w, surf));
    SDL_FreeSurface(surf);
    // Calculate total size
    Size2D size = textures.back()->GetSize();
    totalh += size.height;
    if(size.width >= maxw) maxw = size.width;
  }
  // Prepare the result texture
  auto res = std::make_shared<SDLTextTexture>(w,Size2D(maxw,totalh));
  // Create a helper drawcontext
  DrawContext dc(parent_window->GetWindow(), parent_window ->GetRenderer(), nullptr, 0,0,maxw,totalh);
  // We'll be drawing onto the result texture
  dc.Push(res,maxw,totalh);
  int currenty = 0;
  for(auto& texture : textures){
    dc.DrawTexture(texture,Point2D(0,currenty));
    currenty += texture->GetSize().height;
  }
  dc.Pop();
  return res;
}
} // namespace AlgAudio
