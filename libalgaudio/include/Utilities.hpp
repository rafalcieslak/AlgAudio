#ifndef UTILITIES_HPP
#define UTILITIES_HPP
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
#include <string>
#include <vector>
#include <cmath>
#include "Signal.hpp"

struct SDL_Color;

namespace AlgAudio{

template <typename T=int>
struct Point2D_;

typedef Point2D_<int> Point2D;

struct Size2D{
  Size2D(int w = 0, int h = 0) : width(w), height(h) {}
  bool Fits(const Size2D& other) const{ return (width >= other.width && height >= other.height); }
  bool IsEmpty() const {return width <= 0 || height <= 0; }
  int width, height;
  bool operator==(const Size2D& other) const{ return (width == other.width && height == other.height); }
  bool operator!=(const Size2D& other) const{ return !(*this == other); }
  std::string ToString() const {return "{" + std::to_string(width) + ", " + std::to_string(height) + " }"; }
  Size2D operator+(const Size2D& other) const { return Size2D(width + other.width, height + other.height);}
  Size2D operator-(const Size2D& other) const { return Size2D(width - other.width, height - other.height);}
  Size2D operator/(const int& i) const {return Size2D(width/i, height/i);}
  Point2D ToPoint() const;
};

// Same as size2d, but never interchangable. TODO: Turn both into a common base class.
template <typename T>
struct Point2D_{
  Point2D_(T x_ = 0, T y_ = 0) : x(x_), y(y_) {}
  template <typename Q>
  Point2D_(const Point2D_<Q> &other) : x(other.x), y(other.y) {}
  bool Fits(const Point2D_<T>& other) const{ return (x >= other.x && y >= other.y); }
  T x, y;
  bool operator==(const Point2D_<T>& other) const{ return (x == other.x && y == other.y); }
  bool operator!=(const Point2D_<T>& other) const{ return !(*this == other); }
  std::string ToString() const {return "{" + std::to_string(x) + ", " + std::to_string(y) + " }"; }
  Point2D_<T> operator+(const Point2D_<T>& other) const { return Point2D_<T>(x + other.x, y + other.y);}
  Point2D_<T> operator-(const Point2D_<T>& other) const { return Point2D_<T>(x - other.x, y - other.y);}
  Point2D_<T> operator/(const T& t) const {return Point2D_<T>(x/t, y/t);}
  Point2D_<T> operator*(const T& t) const {return Point2D_<T>(x*t, y*t);}
  Point2D_<T> operator+(const Size2D& other) const { return Point2D_<T>(x + other.width, y + other.height);}
  Point2D_<T> operator-(const Size2D& other) const { return Point2D_<T>(x - other.width, y - other.height);}
  bool IsInside(Point2D_<T> r, Size2D s){ return (x >= r.x) && (x <= r.x + s.width) && (y >= r.y) && (y <= r.y + s.height);}
  static float Distance(Point2D_<T> a, Point2D_<T> b){return sqrt(float((a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y)));}
};
inline Point2D Size2D::ToPoint() const{ return Point2D(width,height);}

template <typename T>
Point2D_<T> operator*(const T& t, const Point2D_<T>& a) {
  return Point2D_<T>(a.x*t, a.y*t);
}

// For text alignment, drawer orientation etc.
typedef enum {
  Direction_TOP,
  Direction_RIGHT,
  Direction_BOTTOM,
  Direction_LEFT,
} Direction;
typedef enum {
  HorizAlignment_LEFT,
  HorizAlignment_CENTERED,
  HorizAlignment_RIGHT
} HorizAlignment;
typedef enum {
  VertAlignment_TOP,
  VertAlignment_CENTERED,
  VertAlignment_BOTTOM
} VertAlignment;

class Exception{
public:
  Exception(std::string t);
  virtual std::string what() {return text;}
  virtual ~Exception() {}
protected:
  std::string text;
};
class UnimplementedException : public Exception{
public:
  UnimplementedException(std::string t) : Exception(t){}
};

class Utilities{
private:
  Utilities() = delete; // static
public:
  static void Wait(int ms);
  static void WaitOS(int ms);
  static bool GetFileExists(std::string name);
  static std::string GetDir(std::string);
  static std::string GetCurrentDir();
  static const char OSDirSeparator;
  static const std::string OSLibSuffix;
  static std::string ConvertUnipathToOSPath(const std::string& unipath);
  static std::string ConvertOSpathToUniPath(const std::string& ospath);
  static void CopyToClipboard(std::string);

  // Point/size operations
  static Point2D Align(HorizAlignment, VertAlignment, Size2D inner, Size2D outer);

  // String operations
  static std::vector<std::string> SplitString(std::string str, std::string delimiter);
  static std::string JoinString(std::vector<std::string> str, std::string c);
  static void Replace(std::string& str, const std::string& from, const std::string& to);
  static std::string TrimAllLines(std::string);
};

} // namespace AlgAudio

#endif //UTILITIES_HPP
