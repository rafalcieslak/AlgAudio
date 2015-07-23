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

  typedef struct
  {
    void* d3dDLL;
    IDirect3D9 *d3d;
    IDirect3DDevice9 *device;
    // There are more fields, but we'll just ignore them.
  } D3D_RenderData;
#endif // ENABLE DX3D

#ifdef ENABLE_OPENGL

  #include <GL/gl.h>

  typedef enum {
      SHADER_NONE,
      SHADER_SOLID,
      SHADER_RGB,
      SHADER_YV12,
      NUM_SHADERS
  } GL_Shader;

  typedef struct GL_FBOList GL_FBOList;
  struct GL_FBOList
  {
    Uint32 w, h;
    GLuint FBO;
    GL_FBOList *next;
  };

  typedef struct
  {
    SDL_GLContext context;

    SDL_bool debug_enabled;
    SDL_bool GL_ARB_debug_output_supported;
    int errors;
    char **error_messages;
    GLDEBUGPROCARB next_error_callback;
    GLvoid *next_error_userparam;

    SDL_bool GL_ARB_texture_rectangle_supported;
    struct {
        GL_Shader shader;
        Uint32 color;
        int blendMode;
    } current;

    SDL_bool GL_EXT_framebuffer_object_supported;
    GL_FBOList *framebuffers;

    /* OpenGL functions */
  #define SDL_PROC(ret,func,params) ret (APIENTRY *func) params;

  SDL_PROC(void, glBegin, (GLenum))
  SDL_PROC(void, glBindTexture, (GLenum, GLuint))
  SDL_PROC(void, glBlendFunc, (GLenum, GLenum))
  SDL_PROC(void, glBlendFuncSeparate, (GLenum, GLenum, GLenum, GLenum))
  SDL_PROC(void, glClear, (GLbitfield))
  SDL_PROC(void, glClearColor, (GLclampf, GLclampf, GLclampf, GLclampf))
  SDL_PROC(void, glColor3fv, (const GLfloat *))
  SDL_PROC(void, glColor4f, (GLfloat, GLfloat, GLfloat, GLfloat))
  SDL_PROC(void, glDeleteTextures, (GLsizei n, const GLuint * textures))
  SDL_PROC(void, glDepthFunc, (GLenum func))
  SDL_PROC(void, glDisable, (GLenum cap))
  SDL_PROC(void, glDrawPixels,
           (GLsizei width, GLsizei height, GLenum format, GLenum type,
            const GLvoid * pixels))
  SDL_PROC(void, glEnable, (GLenum cap))
  SDL_PROC(void, glEnd, (void))
  SDL_PROC(void, glGenTextures, (GLsizei n, GLuint * textures))
  SDL_PROC(GLenum, glGetError, (void))
  SDL_PROC(void, glGetIntegerv, (GLenum pname, GLint * params))
  SDL_PROC(void, glGetPointerv, (GLenum pname, GLvoid * *params))
  SDL_PROC(const GLubyte *, glGetString, (GLenum name))
  SDL_PROC(void, glLineWidth, (GLfloat width))
  SDL_PROC(void, glLoadIdentity, (void))
  SDL_PROC(void, glMatrixMode, (GLenum mode))
  SDL_PROC(void, glOrtho,
           (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top,
            GLdouble zNear, GLdouble zFar))
  SDL_PROC(void, glPixelStorei, (GLenum pname, GLint param))
  SDL_PROC(void, glPointSize, (GLfloat size))
  SDL_PROC(void, glPopMatrix, (void))
  SDL_PROC(void, glPushMatrix, (void))
  SDL_PROC(void, glRasterPos2i, (GLint x, GLint y))
  SDL_PROC(void, glReadBuffer, (GLenum mode))
  SDL_PROC(void, glReadPixels,
           (GLint x, GLint y, GLsizei width, GLsizei height,
            GLenum format, GLenum type, GLvoid * pixels))
  SDL_PROC(void, glRectf,
                  (GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2))
  SDL_PROC(void, glRotated,
                  (GLdouble angle, GLdouble x, GLdouble y, GLdouble z))
  SDL_PROC(void, glRotatef,
                  (GLfloat angle, GLfloat x, GLfloat y, GLfloat z))
  SDL_PROC(void, glScissor, (GLint x, GLint y, GLsizei width, GLsizei height))
  SDL_PROC(void, glShadeModel, (GLenum mode))
  SDL_PROC(void, glTexCoord2f, (GLfloat s, GLfloat t))
  SDL_PROC(void, glTexEnvf, (GLenum target, GLenum pname, GLfloat param))
  SDL_PROC(void, glTexImage2D,
           (GLenum target, GLint level, GLint internalformat, GLsizei width,
            GLsizei height, GLint border, GLenum format, GLenum type,
            const GLvoid * pixels))
  SDL_PROC(void, glTexParameteri, (GLenum target, GLenum pname, GLint param))
  SDL_PROC(void, glTexSubImage2D,
           (GLenum target, GLint level, GLint xoffset, GLint yoffset,
            GLsizei width, GLsizei height, GLenum format, GLenum type,
            const GLvoid * pixels))
  SDL_PROC(void, glTranslatef, (GLfloat x, GLfloat y, GLfloat z))
  SDL_PROC(void, glVertex2f, (GLfloat x, GLfloat y))
  SDL_PROC(void, glVertex3fv, (const GLfloat * v))
  SDL_PROC(void, glViewport, (GLint x, GLint y, GLsizei width, GLsizei height))
  #undef SDL_PROC

    // This struct has more fields, but we'll ignore them.

  } GL_RenderData;
#endif // ENABLE_OPENGL

namespace AlgAudio{

void DrawContext::EnablePremiumBlending(){
  // TODO: Check SDL version

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

  // `offset` is a precalculated value of: offsetof(SDL_Renderer,driverdata);
  //// offset values for:
  // SDL 2.0.3
#if _WIN64
  size_t offset = 432;
#elif _WIN32
  #error SDLHack for win32 not implemented
#elif __GNUC__
  #if __x86_64__ || __ppc64__
    size_t offset = 432;
  #else
    #error SDLHack for gnu32 not implemented
  #endif
#else
  #error Unknown platform, SDLHack not implemented
#endif

  // Get SDL renderer info
  SDL_RendererInfo info;
  SDL_GetRendererInfo(renderer, &info);
  std::string renderer_name(info.name);

  // Calclulate where the driver data pointer inside this SDL_Renderer is.
  void** driverdata_in_sdl_renderer = (void**) ( (char*)renderer + offset );

#ifdef ENABLE_DX3D
  if(renderer_name == "direct3d"){
    D3D_RenderData *data = (D3D_RenderData*) ( *driverdata_in_sdl_renderer );

    HRESULT x = IDirect3DDevice9_SetRenderState(data->device, D3DRS_ALPHABLENDENABLE, true);
    if(x != D3D_OK) throw SDLExceptionNoDesc("Failed to manually configure dx3d renderer");
    IDirect3DDevice9_SetRenderState(data->device, D3DRS_SRCBLEND,       D3DBLEND_ONE);
    IDirect3DDevice9_SetRenderState(data->device, D3DRS_DESTBLEND,      D3DBLEND_INVSRCALPHA);
    IDirect3DDevice9_SetRenderState(data->device, D3DRS_SRCBLENDALPHA,  D3DBLEND_ONE);
    IDirect3DDevice9_SetRenderState(data->device, D3DRS_DESTBLENDALPHA, D3DBLEND_INVSRCALPHA);
    return;
  }
  if(renderer_name == "direct3d11"){
    throw UnimplementedException("Direct3D 11 is currently not supported by AlgAudio's SDLHack, please contact the developer it you ever see this message.");
  }
#endif
#ifdef ENABLE_OPENGL
  if(renderer_name == "opengl"){
    GL_RenderData *data = (GL_RenderData*) ( *driverdata_in_sdl_renderer );
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
  throw UnimplementedException("Renderer '" + renderer_name + "' not recognized by SDLHack");
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
