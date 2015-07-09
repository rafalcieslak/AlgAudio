#include "TextRenderer.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "SDLTexture.hpp"

namespace AlgAudio{

std::map<FontParrams, TTF_Font*> TextRenderer::fontbank;

TTF_Font* TextRenderer::GetFont(FontParrams fp){
  auto it = fontbank.find(fp);
  if(it != fontbank.end()) return it->second;
  return Preload(fp);
}

TTF_Font* TextRenderer::Preload(FontParrams fp){
  std::string path = "fonts/" + fp.name + ".ttf";
  TTF_Font* f = TTF_OpenFont(path.c_str(), fp.size);
  if(!f) throw SDLException("TTF_OpenFont failed");
  fontbank[fp] = f;
  return f;
}

std::shared_ptr<SDLTexture> TextRenderer::Render(std::weak_ptr<Window> w,FontParrams fp, std::string text, const SDL_Color& cl){
  SDL_Surface* surf = TTF_RenderUTF8_Blended(GetFont(fp), text.c_str(), cl);
  auto res = std::make_shared<SDLTexture>(w, surf);
  SDL_FreeSurface(surf);
  return res;
}

} // namespace AlgAudio
