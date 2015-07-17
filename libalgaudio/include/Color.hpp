#ifndef COLOR_HPP
#define COLOR_HPP
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

struct SDL_Color;

namespace AlgAudio{

class Color{
public:
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
    // Color operations
    Color ZeroAlpha() const { return Color(r,g,b,0); }
    Color Lighter(double amount) const;
    Color Darker(double amount) const;
private:
// HSL representation wrapper
  class HSL{
  public:
    constexpr HSL(double hue, double saturation, double lightness, unsigned char a = 255) : h(hue), s(saturation), l(lightness), alpha(a) {}
    constexpr HSL(unsigned int hex) :
         h(((double)((hex&0xff000000)>>24)/255)),
         s(((double)((hex&0x00ff0000)>>16)/255)),
         l(((double)((hex&0x0000ff00)>> 8)/255)),
     alpha(((double)((hex&0x000000ff)    )    ))  {}
     HSL() {}
     double h,s,l;
     unsigned char alpha;
     explicit operator Color() const;
  };
  explicit operator HSL() const;
};

} // namespace AlgAudio

#endif // COLOR_HPP
