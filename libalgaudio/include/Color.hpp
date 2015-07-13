#ifndef COLOR_HPP
#define COLOR_HPP

struct SDL_Color;

namespace AlgAudio{

struct Color{
  constexpr Color(unsigned char R, unsigned char G, unsigned char B, unsigned char A=255) : r(R), g(G), b(B), alpha(A) {}
  constexpr Color(unsigned int hex) :
       r((hex&0xff000000)>>24),
       g((hex&0x00ff0000)>>16),
       b((hex&0x0000ff00)>>8),
   alpha((hex&0x000000ff)>>0) {}
  Color() {}
  unsigned char r,g,b,alpha;
  SDL_Color SDL() const;
  operator SDL_Color() const;
  Color ZeroAlpha() const { return Color(r,g,b,0); }
};

} // namespace AlgAudio

#endif // COLOR_HPP
