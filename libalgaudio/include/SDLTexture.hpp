#ifndef SDLTEXTURE_HPP
#define SDLTEXTURE_HPP
#include "Utilities.hpp"
#include "SDLHandle.hpp"
#include <memory>

struct SDL_Texture;

namespace AlgAudio{

class UIWindow;

// A wrapper class for operations on an SDL_Texture
class SDLTexture{
public:
  SDLTexture(std::weak_ptr<UIWindow> parent_window, Size2D size);
  ~SDLTexture();
  SDLTexture(const SDLTexture& other) = delete; // No copyconstructing
  SDLTexture& operator=(const SDLTexture& other) = delete; // No copying
  void Resize(Size2D size);
  Size2D GetSize() {return size;}
  friend class DrawContext;
private:
  SDLHandle handle;
  SDL_Texture* texture;
  std::weak_ptr<UIWindow> parent;
  Size2D size;
};

} // namespace AlgAudio

#endif // SDLTEXTURE_HPP
