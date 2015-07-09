#ifndef TEXTRENDERER_HPP
#define TEXTRENDERER_HPP
#include <memory>
#include <string>
#include <map>
#include "SDLHandle.hpp"

struct _TTF_Font;
typedef _TTF_Font TTF_Font;
struct SDL_Color;

namespace AlgAudio{

class SDLTexture;
class Window;

struct FontParrams{
  FontParrams(std::string n, int s) : name(n), size(s) {}
  std::string name;
  int size;
  bool operator<(const FontParrams& other) const{ return name<other.name || (name==other.name && size < other.size);}
};

// TODO: Instantiable class, SDLHandle tracking, font unloading on destruction
class TextRenderer{
  TextRenderer() = delete; // static class
public:
  static std::shared_ptr<SDLTexture> Render(std::weak_ptr<Window>, FontParrams, std::string,  const SDL_Color&);
  static TTF_Font* GetFont(FontParrams);
  static TTF_Font* Preload(FontParrams);
private:
  // Temporarily it is assumed that any rendering will be performed only if
  // at least one window exists, thus it should be safe to assume that SDL
  // is always initialised.
  // SDLHandle handle;
  static std::map<FontParrams, TTF_Font*> fontbank;
};


} // namespace AlgAudio

#endif // TEXTRENDERER_HPP
