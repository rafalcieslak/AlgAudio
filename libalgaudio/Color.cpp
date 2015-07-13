#include "Color.hpp"
#include <SDL2/SDL.h>

namespace AlgAudio{

SDL_Color Color::SDL() const{
  return SDL_Color{r,g,b,alpha};
}

Color::operator SDL_Color() const{
  return SDL();
}

}
