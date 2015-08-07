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
#include "DrawContext.hpp"
#include <SDL2/SDL.h>
#include <iostream>
#include "SDLTexture.hpp"
#include "Color.hpp"
#include "SDLFix/SDLFix.hpp"

#undef DrawText

namespace AlgAudio{

DrawContext::DrawContext(SDL_Window* win, SDL_Renderer* r, SDL_GLContext c, int x_, int y_, int w, int h) :
  width(w), height(h), x(x_), y(y_), window(win), renderer(r), context(c)
{

  UpdateClipRect();
}
void DrawContext::DrawLine(Point2D from, Point2D to, bool smooth){
  //std::cout << "Making current " << context << std::endl;
  if(smooth){
    SDLFix::RenderSetLineSmoothing(renderer, true);
  }else{
    SDLFix::RenderSetLineSmoothing(renderer, false);
  }
  SDL_RenderDrawLine(renderer, x+from.x, y+from.y, x+to.x, y+to.y);
}

SDL_FPoint PointToSDL(Point2D_<float> p){
  return {p.x, p.y};
}

void DrawContext::DrawCubicBezier(Point2D p1, Point2D p2, Point2D p3, Point2D p4, unsigned int lines){
  // TODO :: De casteljeu

  SDLFix::RenderSetLineSmoothing(renderer, true);

  unsigned int count = lines + 1;
  Point2D_<float> a1 = p1, a2 = p2, a3 = p3, a4 = p4;
  SDL_FPoint points[count];

  for(unsigned int i = 0; i < lines; i++){
    float t = (1.0f / (lines)) * i;
    float q = (1.0f - t);
    Point2D_<float> x0 =   q*q*q* a1;
    Point2D_<float> x1 = 3*q*q*t* a2;
    Point2D_<float> x2 = 3*q*t*t* a3;
    Point2D_<float> x3 =   t*t*t* a4;
    Point2D_<float> p = x0 + x1 + x2 + x3;
    points[i] = PointToSDL(p);
  }

  points[count-1] = PointToSDL(a4);

  SDLFix::RenderDrawLines(renderer, points, count);
}


void DrawContext::DrawLineEx(float x1, float y1, float x2, float y2, float width){
  SDL_FPoint points[2];
  points[0].x = x1; points[0].y = y1;
  points[1].x = x2; points[1].y = y2;
  SDLFix::RenderSetLineWidth(renderer, width);
  SDLFix::RenderDrawLines(renderer, points, 2);
  SDLFix::RenderSetLineWidth(renderer, 1.0);
}

void DrawContext::DrawTexture(std::shared_ptr<SDLTexture> texture, int x_, int y_){
  //std::cout << "Drawing texture " << texture << "(" << texture->texture << ") at " << x+x_ << " " << y+y_ << std::endl;
  if(!texture->valid) return; // Silently skip null textures.
  const Size2D texture_size = texture->GetSize();
  SDL_Rect source{0, 0, texture_size.width, texture_size.height};
  SDL_Rect dest{x + x_, y + y_, texture_size.width, texture_size.height};
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  SDLFix::CorrectBlendMode(renderer);
  SDL_RenderCopy(renderer, texture->texture, &source, &dest);
}
void DrawContext::DrawText(std::shared_ptr<SDLTextTexture> texture, Color c, int x_, int y_){
  //std::cout << "Drawing texture " << texture << "(" << texture->texture << ") at " << x+x_ << " " << y+y_ << std::endl;
  if(!texture->valid) return; // Silently skip null textures.
  const Size2D texture_size = texture->GetSize();
  SDL_Rect source{0, 0, texture_size.width, texture_size.height};
  SDL_Rect dest{x + x_, y + y_, texture_size.width, texture_size.height};
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  SDLFix::CorrectBlendMode(renderer);
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
  SDLFix::CorrectBlendMode(renderer);
}
void DrawContext::Clear(Color c){
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
  SetColor(c);
  SDL_RenderClear(renderer);
  //SDL_RenderFillRect(renderer,NULL);
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  SDLFix::CorrectBlendMode(renderer);
}
void DrawContext::Push(Point2D p, Size2D s){
  // Remember the previous state
  context_stack.push(DCLevel(current_target, x, y, width, height));
  // Set new state
  x = p.x; y = p.y;
  width = s.width; height = s.height;
  UpdateClipRect();
}
void DrawContext::Restore(){
  SwitchToTarget(current_target);
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
  SDL_RenderSetClipRect(renderer, &clip);
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
