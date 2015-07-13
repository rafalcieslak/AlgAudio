#include "UI/DrawContext.hpp"
#include <SDL2/SDL.h>
#include <iostream>
#include "SDLTexture.hpp"

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
  const Size2D texture_size = texture->GetSize();
  SDL_Rect source{0, 0, texture_size.width, texture_size.height};
  SDL_Rect dest{x + x_, y + y_, texture_size.width, texture_size.height};
  SDL_RenderCopy(renderer, texture->texture, &source, &dest);
}

void DrawContext::DrawRect(int x, int y, int w, int h){
  SDL_Rect rect{x,y,w,h};
  SDL_RenderFillRect(renderer,&rect);
}

void DrawContext::Clear(){
	SDL_RenderFillRect(renderer,NULL);
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
  if(t)
    SDL_SetRenderTarget(renderer, t->texture);
  else
    SDL_SetRenderTarget(renderer, NULL);
  current_target = t;
}

void DrawContext::UpdateClipRect(){
  SDL_Rect clip{x,y,width,height};
  SDL_RenderSetClipRect(renderer, &clip);
}

void DrawContext::SetColor(short r, short g, short b, short a){
  SDL_SetRenderDrawColor(renderer, r, g, b, a);
}
void DrawContext::SetColor(const SDL_Color& c){
  SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
}
bool DrawContext::HasZeroArea(){
  if(width <= 0 || height <= 0) return true;
  return false;
}

} // namespace AlgAudio
