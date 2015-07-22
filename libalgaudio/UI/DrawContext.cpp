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
#include "UI/DrawContext.hpp"
#include <SDL2/SDL.h>
#include <iostream>
#include "SDLTexture.hpp"
#include "Color.hpp"

namespace AlgAudio{

DrawContext::DrawContext(SDL_Renderer* r, int x_, int y_, int w, int h) :
  width(w), height(h), x(x_), y(y_), renderer(r)
{
    UpdateClipRect();
}

void DrawContext::DrawLine(int x1, int y1, int x2, int y2){
  SDL_RenderDrawLine(renderer, x+x1, y+y1, x+x2, y+y2);
}

void DrawContext::DrawTexture(std::shared_ptr<SDLTexture> texture, int x_, int y_){
  //std::cout << "Drawing texture " << texture << "(" << texture->texture << ") at " << x+x_ << " " << y+y_ << std::endl;
  const Size2D texture_size = texture->GetSize();
  SDL_Rect source{0, 0, texture_size.width, texture_size.height};
  SDL_Rect dest{x + x_, y + y_, texture_size.width, texture_size.height};
  SDL_RenderCopy(renderer, texture->texture, &source, &dest);
}
void DrawContext::DrawText(std::shared_ptr<SDLTextTexture> texture, Color c, int x_, int y_){
  //std::cout << "Drawing texture " << texture << "(" << texture->texture << ") at " << x+x_ << " " << y+y_ << std::endl;
  const Size2D texture_size = texture->GetSize();
  SDL_Rect source{0, 0, texture_size.width, texture_size.height};
  SDL_Rect dest{x + x_, y + y_, texture_size.width, texture_size.height};
  SDL_SetTextureColorMod(texture->texture, c.r, c.g, c.b);
  SDL_RenderCopy(renderer, texture->texture, &source, &dest);
  SDL_SetTextureColorMod(texture->texture, 255,255,255);
}

void DrawContext::DrawRect(int x, int y, int w, int h){
  SDL_Rect rect{x,y,w,h};
  SDL_RenderFillRect(renderer,&rect);
}

void DrawContext::Fill(){
	SDL_RenderFillRect(renderer,NULL);
}
void DrawContext::Clear(){
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
  SetColor(Color(0,0,0,0));
  Fill();
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
}
void DrawContext::Clear(Color c){
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
  SetColor(c);
  //Fill();
  SDL_RenderClear(renderer);
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
}
void DrawContext::Push(int x1, int y1, int width_, int height_){
  // Remember the previous state
  context_stack.push(DCLevel(current_target, x, y, width, height));
  // Set new state
  x = x1; y = y1;
  width = width_; height = height_;
  UpdateClipRect();
}
void DrawContext::Push(Point2D p, Size2D s){
  // Remember the previous state
  context_stack.push(DCLevel(current_target, x, y, width, height));
  // Set new state
  x = p.x; y = p.y;
  width = s.width; height = s.height;
  UpdateClipRect();
}
void DrawContext::Push(std::shared_ptr<SDLTexture> t, int width_, int height_){
  // Remember the previous state
  context_stack.push(DCLevel(current_target, x, y, width, height));
  // Set new state
  SwitchToTarget(t);
  x = 0; y = 0;
  width = width_; height = height_;
  UpdateClipRect();
}
void DrawContext::Pop(){
  if(context_stack.empty()){
    std::cout << "Warning: too many POPS on a DrawContext" << std::endl;
    return;
  }
  DCLevel state = context_stack.top();
  context_stack.pop();

  SwitchToTarget(state.target);
  x = state.xoffset; y = state.yoffset;
  width = state.width; height = state.height;
  UpdateClipRect();
}

void DrawContext::SwitchToTarget(std::shared_ptr<SDLTexture> t){
  //std::cout << "Switching to target " << t << "(" << ((!t)?0:t->texture) << ")" << std::endl;
  if(t)
    SDL_SetRenderTarget(renderer, t->texture);
  else
    SDL_SetRenderTarget(renderer, NULL);
  current_target = t;
}

void DrawContext::UpdateClipRect(){
  //std::cout << "Clip set: " << x << " " << y << " " << width << " " << height << std::endl;
  SDL_Rect clip{x,y,width,height};
  //SDL_RenderSetClipRect(renderer, &clip);
}

void DrawContext::SetColor(short r, short g, short b, short a){
  SDL_SetRenderDrawColor(renderer, r, g, b, a);
}
void DrawContext::SetColor(const Color& c){
  SDL_SetRenderDrawColor(renderer, c.r*(c.alpha/255.0), c.g*(c.alpha/255.0), c.b*(c.alpha/255.0), c.alpha);
}
bool DrawContext::HasZeroArea(){
  if(width <= 0 || height <= 0) return true;
  return false;
}

} // namespace AlgAudio
