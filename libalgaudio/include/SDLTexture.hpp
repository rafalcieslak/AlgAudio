#ifndef SDLTEXTURE_HPP
#define SDLTEXTURE_HPP
#include "Color.hpp"
#include "SDLHandle.hpp"
#include <memory>

struct SDL_Texture;
struct SDL_Surface;

namespace AlgAudio{

class Window;

// A wrapper class for operations on an SDL_Texture
class SDLTexture{
public:
  SDLTexture(std::weak_ptr<Window> parent_window, Size2D size);
  SDLTexture(std::weak_ptr<Window> parent_window, SDL_Surface*);
  ~SDLTexture();
  SDLTexture(const SDLTexture& other) = delete; // No copyconstructing
  SDLTexture& operator=(const SDLTexture& other) = delete; // No copying
  void Resize(Size2D size);
  Size2D GetSize() {return size;}
  friend class DrawContext;
private:
  SDLHandle handle;
  SDL_Texture* texture;
  // Creating a textire of 0 size will mark it as invalid.
  bool valid = true;
  std::weak_ptr<Window> parent;
  Size2D size;
};

} // namespace AlgAudio

#endif // SDLTEXTURE_HPP
