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
#include "SDLMain.hpp"
#include "SDLTexture.hpp"
#include <SDL2/SDL.h>
#include <iostream>
#include <cstddef> // for offsetof

// TODO: CMake should control this flag
#define ENABLE_DX3D

#ifdef ENABLE_DX3D
  #include <windows.h>
  #include <d3dx9.h>
#endif // ENABLE DX3D

typedef struct
{
    void* d3dDLL;
    IDirect3D9 *d3d;
    IDirect3DDevice9 *device;
    // There are more fields, but we'll just ignore them.
} D3D_RenderData;

namespace AlgAudio{

void DrawContext::EnablePremiumBlending(){
  // TODO: Check SDL version
  // TODO: Check if renderer is opengl

  /*
  struct SDL_Renderer looks like this:
  {
    // A lot of fields
    void* data;
  }
  where data is a pointer to driver-specific data. By accessing that pointer
  we can use, for example, the actual D3D_RenderData structure. From it, we can
  access the native d3d handle and use it as we desire.
  */

  // The dist1 is a precalculated value of: offsetof(SDL_Renderer,driverdata);
  // SDL 2.0.3, Win64
  size_t dist1 = 432;

  // Calclulate where the driver data pointer inside this SDL_Renderer is.
  void** driverdata_in_sdl_renderer = (void**) ( (char*)renderer + dist1 );

#ifdef ENABLE_DX3D
  D3D_RenderData *data = (D3D_RenderData*) ( *driverdata_in_sdl_renderer );

  HRESULT x = IDirect3DDevice9_SetRenderState(data->device, D3DRS_ALPHABLENDENABLE, true);
  if(x != D3D_OK) throw SDLExceptionNoDesc("Failed to manually configure dx3d renderer");
  IDirect3DDevice9_SetRenderState(data->device, D3DRS_SRCBLEND,       D3DBLEND_ONE);
  IDirect3DDevice9_SetRenderState(data->device, D3DRS_DESTBLEND,      D3DBLEND_INVSRCALPHA);
  IDirect3DDevice9_SetRenderState(data->device, D3DRS_SRCBLENDALPHA,  D3DBLEND_ONE);
  IDirect3DDevice9_SetRenderState(data->device, D3DRS_DESTBLENDALPHA, D3DBLEND_INVSRCALPHA);

#endif
}

void SDLTexture::PremultiplySurface32RGBA(SDL_Surface* surf){
  std::cout << "Examining surface " << surf->w << " " << surf->h << std::endl;
  SDL_LockSurface(surf);
  unsigned char* pixels = (unsigned char*)surf->pixels;
  for(int p = 0; p < surf->w * surf->h; p++){
    pixels[4*p+0] = ((unsigned int)pixels[4*p+0])*((unsigned int)pixels[4*p+3])/255;
    pixels[4*p+1] = ((unsigned int)pixels[4*p+1])*((unsigned int)pixels[4*p+3])/255;
    pixels[4*p+2] = ((unsigned int)pixels[4*p+2])*((unsigned int)pixels[4*p+3])/255;
  }

  SDL_UnlockSurface(surf);
}

}
