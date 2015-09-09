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
#include "SDLFix.hpp"
#include "SDL_ABI.hpp"

#include <iostream>
#include <string>
#include "Utilities.hpp"


static int
GL_Fixed_UpdateClipRect(SDL_Renderer * renderer)
{
  // SDL_RenderSetClipRect in SDL 2.0.3 is bugged. It forgets to make a gl
  // context current, and therefore it sets the clip rect on a context that
  // is not necessarily the one assiciated with this particular renderer.
  // A workadound is used to force SDL to make the right context current.
  AlgAudio::SDLFix::RendererMakeCurrent(renderer);

  const SDL_Rect *rect = &renderer->clip_rect;
  GL_RenderData *data = (GL_RenderData *) renderer->driverdata;
  if (!SDL_RectEmpty(rect)) {
    data->glEnable(GL_SCISSOR_TEST);
    // ORIG:
    // data->glScissor(rect->x, renderer->viewport.h - rect->y - rect->h, rect->w, rect->h);
    // FIXED:
    data->glScissor(rect->x, rect->y, rect->w, rect->h);
  } else {
    data->glDisable(GL_SCISSOR_TEST);
  }
  return 0;
}

namespace AlgAudio{

void SDLFix::FixRenderer(SDL_Renderer* renderer){
  // Get SDL renderer info
  SDL_RendererInfo info;
  SDL_GetRendererInfo(renderer, &info);
  std::string renderer_name(info.name);

  if(renderer_name != "opengl")
    throw Exceptions::Unimplemented("Renderer '" + renderer_name + "' IS NOT supported! AlgAudio will can work with opengl only.");

  // Fixes clip rect position by setting function pointer to a custom modified
  // function
  renderer->UpdateClipRect = GL_Fixed_UpdateClipRect;
}

void SDLFix::CorrectBlendMode(SDL_Renderer* renderer){
  GL_RenderData *data = (GL_RenderData*) renderer->driverdata;
  data->glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  data->glEnable(GL_BLEND);
  data->glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
}

void SDLFix::RenderDrawLines(SDL_Renderer* renderer, const SDL_FPoint* fpoints, int count){
  // We'll ignore argument checks. See SDL_render.c if you would like to reimplement it.
  renderer->RenderDrawLines(renderer, fpoints, count);
}

void SDLFix::RenderSetLineWidth(SDL_Renderer* renderer, float w){
  GL_RenderData *data = (GL_RenderData*) renderer->driverdata;
  data->glLineWidth(w);
}

void SDLFix::RenderSetLineSmoothing(SDL_Renderer* renderer, bool enabled){
  GL_RenderData *data = (GL_RenderData*) renderer->driverdata;
  if(enabled){
    data->glEnable(GL_LINE_SMOOTH);
    data->glLineWidth(2.0);
  }else{
    data->glDisable(GL_LINE_SMOOTH);
    data->glLineWidth(1.0);
  }
}

void SDLFix::RendererMakeCurrent(SDL_Renderer* renderer){
  SDL_Texture tex;
  tex.driverdata = nullptr;
  // renderer->DestroyTexture points to GL_DestroyTexture (see
  // SDL_render_gl.c), which first calls GL_ActivateRenderer and then checks
  // if texture->driverdata is null, if it is, it returns immediatelly.
  renderer->DestroyTexture(renderer,&tex);
}

void SDLFix::PremultiplySurface32RGBA(SDL_Surface* surf){
  SDL_LockSurface(surf);
  unsigned char* pixels = (unsigned char*)surf->pixels;
  for(int p = 0; p < surf->w * surf->h; p++){
    pixels[4*p+0] = ((unsigned int)pixels[4*p+0])*((unsigned int)pixels[4*p+3])/255;
    pixels[4*p+1] = ((unsigned int)pixels[4*p+1])*((unsigned int)pixels[4*p+3])/255;
    pixels[4*p+2] = ((unsigned int)pixels[4*p+2])*((unsigned int)pixels[4*p+3])/255;
  }
  SDL_UnlockSurface(surf);
}

} // namespace AlgAudio
