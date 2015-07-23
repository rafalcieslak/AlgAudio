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

#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include "Utilities.hpp"

// TODO: CMake should control these flag
#ifdef __unix__
  #define ENABLE_OPENGL
#else
  #define ENABLE_DX3D
  #define ENABLE_OPENGL
#endif

#ifdef ENABLE_DX3D
  #include <windows.h>
  #include <d3dx9.h>
#endif
#ifdef ENABLE_OPENGL
  #include <GL/gl.h>
  #include <GL/glext.h>
#endif

// SDL_ABI must be included after D3D/GL headers.
#include "SDL_ABI.hpp"

static int
GL_Fixed_UpdateClipRect(SDL_Renderer * renderer)
{
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

#ifdef ENABLE_DX3D
  if(renderer_name == "direct3d"){
    // Currently nothing to fix
    return;
  }
  if(renderer_name == "direct3d11"){
    // Currently nothing to fix
    return;
  }
#endif
#ifdef ENABLE_OPENGL
  if(renderer_name == "opengl"){
    // Fixes clip rect position
    renderer->UpdateClipRect = GL_Fixed_UpdateClipRect;
    return;
  }
#endif
  if(renderer_name == "software"){
    // This should never happen, we explicitly ask SDL to create a hadrware accelerated renderer.
    throw UnimplementedException("Sorry, the software renderer is not capable of required blendmodes");
  }
  throw UnimplementedException("Renderer '" + renderer_name + "' not recognized by SDLFix");
}

void SDLFix::CorrectBlendMode(SDL_Renderer* renderer){

  // Get SDL renderer info
  SDL_RendererInfo info;
  SDL_GetRendererInfo(renderer, &info);
  std::string renderer_name(info.name);

#ifdef ENABLE_DX3D
  if(renderer_name == "direct3d"){
    D3D_RenderData *data = (D3D_RenderData*) renderer->driverdata;

    IDirect3DDevice9_SetRenderState(data->device, D3DRS_ALPHABLENDENABLE, true);
    IDirect3DDevice9_SetRenderState(data->device, D3DRS_SRCBLEND,       D3DBLEND_ONE);
    IDirect3DDevice9_SetRenderState(data->device, D3DRS_DESTBLEND,      D3DBLEND_INVSRCALPHA);
    IDirect3DDevice9_SetRenderState(data->device, D3DRS_SRCBLENDALPHA,  D3DBLEND_ONE);
    IDirect3DDevice9_SetRenderState(data->device, D3DRS_DESTBLENDALPHA, D3DBLEND_INVSRCALPHA);
    return;
  }
  if(renderer_name == "direct3d11"){
    throw UnimplementedException("Direct3D 11 is currently not supported by AlgAudio's SDLFix, please contact the developer it you ever see this message.");
  }
#endif
#ifdef ENABLE_OPENGL
  if(renderer_name == "opengl"){
    GL_RenderData *data = (GL_RenderData*) renderer->driverdata;
    data->glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    data->glEnable(GL_BLEND);
    data->glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    return;
  }
#endif
  if(renderer_name == "software"){
    // This should never happen, we explicitly ask SDL to create a hadrware accelerated renderer.
    throw UnimplementedException("Sorry, the software renderer is not capable of required blendmodes");
  }
  throw UnimplementedException("Renderer '" + renderer_name + "' not recognized by SDLFix");
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
