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
#include "Color.hpp"
#include <SDL2/SDL.h>
#include <algorithm>
#include <iostream>

namespace AlgAudio{

SDL_Color Color::SDL() const{
  std::cout << "Used." << std::endl;
  return SDL_Color{r*(alpha/255.0),g*(alpha/255.0),b*(alpha/255.0),alpha};
}

Color::operator SDL_Color() const{
  return SDL();
}

Color Color::Lighter(double amount) const{
  HSL b = (HSL)(*this);
  b.l += amount;
  if(b.l > 1.0) b.l = 1.0;
  if(b.l < 0.0) b.l = 0.0;
  return (Color)b;
}
Color Color::Darker(double amount) const{
  return Lighter(-amount);
}

// Conversion logic explained here: http://en.wikipedia.org/wiki/HSL_color_space
Color::operator HSL() const{
    double rd = (double) r/255;
    double gd = (double) g/255;
    double bd = (double) b/255;
    double max = std::max(rd, std::max(gd, bd));
    double min = std::min(rd, std::min(gd, bd));
    double h, s, l = (max + min) / 2;
    if (max == min) {
        h = s = 0; // achromatic
    } else {
        double d = max - min;
        s = (l > 0.5) ? d/(2 - max - min) : d/(max + min);
        if (max == rd) {
            h = (gd - bd) / d + ((gd < bd) ? 6 : 0);
        } else if (max == gd) {
            h = (bd - rd) / d + 2;
        } else if (max == bd) {
            h = (rd - gd) / d + 4;
        }
        h /= 6;
    }
    return HSL(h,s,l,alpha);
}

inline double hueextract(double p, double q, double t) {
    // Wrap around t
    if(t < 0) t += 1;
    if(t > 1) t -= 1;
    // Return color from hue
    if(t < 1.0/6) return p + (q - p) * 6 * t;
    if(t < 1.0/2) return q;
    if(t < 2.0/3) return p + (q - p) * (2.0/3 - t) * 6;
    return p;
}

// HSL to RGB
Color::HSL::operator Color() const{
    double r, g, b;
    if (s == 0) {
        r = g = b = l; // achromatic
    } else {
        double q = (l < 0.5) ? l*(1+s) : l+s-(l*s);
        double p = 2*l - q;
        r = hueextract(p, q, h + 1.0/3);
        g = hueextract(p, q, h);
        b = hueextract(p, q, h - 1.0/3);
    }
    return Color(r*255, g*255, b*255, alpha);
}


} // namespace AlgAudio
