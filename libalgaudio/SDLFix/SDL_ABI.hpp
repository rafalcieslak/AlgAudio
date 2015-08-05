#ifndef SDL_ABI
#define SDL_ABI

/*
When accessing the internals of SDL in order to get native handles, it is
necessary to know the offsets of several data fields in structures.
Below are these structures compiled into on file for your convenience.
These are taken from SDL 2.0.3 source files.
SDL is available on http://www.libsdl.com and redistributed under the terms of
the zlib license:

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

*/


/// =======================================
/// ========== SDL structures =============
/// =======================================

struct SDL_SW_YUVTexture;

typedef struct
{
    float x;
    float y;
    float w;
    float h;
} SDL_FRect;

/* Define the SDL texture structure */
struct SDL_Texture
{
    const void *magic;
    Uint32 format;              /**< The pixel format of the texture */
    int access;                 /**< SDL_TextureAccess */
    int w;                      /**< The width of the texture */
    int h;                      /**< The height of the texture */
    int modMode;                /**< The texture modulation mode */
    SDL_BlendMode blendMode;    /**< The texture blend mode */
    Uint8 r, g, b, a;           /**< Texture modulation values */

    SDL_Renderer *renderer;

    /* Support for formats not supported directly by the renderer */
    SDL_Texture *native;
    SDL_SW_YUVTexture *yuv;
    void *pixels;
    int pitch;
    SDL_Rect locked_rect;

    void *driverdata;           /**< Driver specific texture representation */

    SDL_Texture *prev;
    SDL_Texture *next;
};

struct SDL_Renderer
{
    const void *magic;

    void (*WindowEvent) (SDL_Renderer * renderer, const SDL_WindowEvent *event);
    int (*GetOutputSize) (SDL_Renderer * renderer, int *w, int *h);
    int (*CreateTexture) (SDL_Renderer * renderer, SDL_Texture * texture);
    int (*SetTextureColorMod) (SDL_Renderer * renderer,
                               SDL_Texture * texture);
    int (*SetTextureAlphaMod) (SDL_Renderer * renderer,
                               SDL_Texture * texture);
    int (*SetTextureBlendMode) (SDL_Renderer * renderer,
                                SDL_Texture * texture);
    int (*UpdateTexture) (SDL_Renderer * renderer, SDL_Texture * texture,
                          const SDL_Rect * rect, const void *pixels,
                          int pitch);
    int (*UpdateTextureYUV) (SDL_Renderer * renderer, SDL_Texture * texture,
                            const SDL_Rect * rect,
                            const Uint8 *Yplane, int Ypitch,
                            const Uint8 *Uplane, int Upitch,
                            const Uint8 *Vplane, int Vpitch);
    int (*LockTexture) (SDL_Renderer * renderer, SDL_Texture * texture,
                        const SDL_Rect * rect, void **pixels, int *pitch);
    void (*UnlockTexture) (SDL_Renderer * renderer, SDL_Texture * texture);
    int (*SetRenderTarget) (SDL_Renderer * renderer, SDL_Texture * texture);
    int (*UpdateViewport) (SDL_Renderer * renderer);
    int (*UpdateClipRect) (SDL_Renderer * renderer);
    int (*RenderClear) (SDL_Renderer * renderer);
    int (*RenderDrawPoints) (SDL_Renderer * renderer, const SDL_FPoint * points,
                             int count);
    int (*RenderDrawLines) (SDL_Renderer * renderer, const SDL_FPoint * points,
                            int count);
    int (*RenderFillRects) (SDL_Renderer * renderer, const SDL_FRect * rects,
                            int count);
    int (*RenderCopy) (SDL_Renderer * renderer, SDL_Texture * texture,
                       const SDL_Rect * srcrect, const SDL_FRect * dstrect);
    int (*RenderCopyEx) (SDL_Renderer * renderer, SDL_Texture * texture,
                       const SDL_Rect * srcquad, const SDL_FRect * dstrect,
                       const double angle, const SDL_FPoint *center, const SDL_RendererFlip flip);
    int (*RenderReadPixels) (SDL_Renderer * renderer, const SDL_Rect * rect,
                             Uint32 format, void * pixels, int pitch);
    void (*RenderPresent) (SDL_Renderer * renderer);
    void (*DestroyTexture) (SDL_Renderer * renderer, SDL_Texture * texture);

    void (*DestroyRenderer) (SDL_Renderer * renderer);

    int (*GL_BindTexture) (SDL_Renderer * renderer, SDL_Texture *texture, float *texw, float *texh);
    int (*GL_UnbindTexture) (SDL_Renderer * renderer, SDL_Texture *texture);

    /* The current renderer info */
    SDL_RendererInfo info;

    /* The window associated with the renderer */
    SDL_Window *window;
    SDL_bool hidden;

    /* The logical resolution for rendering */
    int logical_w;
    int logical_h;
    int logical_w_backup;
    int logical_h_backup;

    /* The drawable area within the window */
    SDL_Rect viewport;
    SDL_Rect viewport_backup;

    /* The clip rectangle within the window */
    SDL_Rect clip_rect;
    SDL_Rect clip_rect_backup;

    /* The render output coordinate scale */
    SDL_FPoint scale;
    SDL_FPoint scale_backup;

    /* The list of textures */
    SDL_Texture *textures;
    SDL_Texture *target;

    Uint8 r, g, b, a;                   /**< Color for drawing operations values */
    SDL_BlendMode blendMode;            /**< The drawing blend mode */

    void *driverdata;
};


/// =======================================
/// ======== DIRECT3D structures ==========
/// =======================================

#ifdef ENABLE_DX3D

typedef struct
{
  void* d3dDLL;
  IDirect3D9 *d3d;
  IDirect3DDevice9 *device;
  // There are more fields, but we'll just ignore them.
} D3D_RenderData;

#endif //ENABLE_DX3D

/// =======================================
/// ======== OPENGL structures ==========
/// =======================================

#ifdef ENABLE_OPENGL

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

#endif //SDL_ABI
