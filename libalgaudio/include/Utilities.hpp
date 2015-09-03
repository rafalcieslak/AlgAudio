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
struct SDL_KeyboardEvent;

namespace AlgAudio{

// Forward declarations
template <typename T=int>
struct Point2D_;
struct Rect;

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

// Similar to Size2D, but never interchangable.
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
  Point2D_<T> operator-() const { return Point2D_<T>(-x,-y);}
  Point2D_<T> operator/(const T& t) const {return Point2D_<T>(x/t, y/t);}
  Point2D_<T> operator*(const T& t) const {return Point2D_<T>(x*t, y*t);}
  Point2D_<T> operator+(const Size2D& other) const { return Point2D_<T>(x + other.width, y + other.height);}
  Point2D_<T> operator-(const Size2D& other) const { return Point2D_<T>(x - other.width, y - other.height);}
  Point2D_<T>& operator+=(const Point2D_<T>& other) { x += other.x; y += other.y; return *this;}
  Point2D_<T>& operator-=(const Point2D_<T>& other) { x -= other.x; y -= other.y; return *this;}
  bool IsInside(Point2D_<T> r, Size2D s) const { return (x >= r.x) && (x < r.x + s.width) && (y >= r.y) && (y < r.y + s.height);}
  bool IsInside(const Rect& r) const;
  static float Distance(Point2D_<T> a, Point2D_<T> b){return sqrt(float((a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y)));}
};
inline Point2D Size2D::ToPoint() const{ return Point2D(width,height);}

template <typename T>
Point2D_<T> operator*(const T& t, const Point2D_<T>& a) {
  return Point2D_<T>(a.x*t, a.y*t);
}

// Special case for multiplying and dividing a classic point with a float. To
// prevent accuracy loss specialisations below return a float point. Also, if
// it was not for these specialisations, multiplying a point by 0.5 would 
// implicitly convert 0.5 to an int and thus cause a SIGFPE.
inline Point2D_<float> operator*(const float& t, const Point2D_<int>& a) {
  return Point2D_<float>(a.x*t, a.y*t);
}
inline Point2D_<float> operator*(const Point2D_<int>& a, const float& t) {
  return Point2D_<float>(a.x*t, a.y*t);
}
inline Point2D_<float> operator/(const Point2D_<int>& a, const float& t) {
  return Point2D_<float>(a.x/t, a.y/t);
}

struct Rect{
  Point2D a;
  Point2D b;
  Rect() {}
  Rect(Point2D a_, Point2D b_){
         if(a_.x <= b_.x && a_.y <= b_.y) {a = a_; b = b_;}
    else if(a_.x >= b_.x && a_.y >= b_.y) {a = b_; b = a_;}
    else if(a_.x <= b_.x && a_.y >= b_.y) {a = {a_.x,b_.y}; b = {b_.x,a_.y};}
    else if(a_.x >= b_.x && a_.y <= b_.y) {a = {b_.x,a_.y}; b = {a_.x,b_.y};}
  }
  Rect(Point2D a_, Size2D s) : a(a_), b(a_+s) {}
  inline Size2D Size() const {
    auto q = b-a;
    return {q.x,q.y};
  }
  Rect MoveOffset(Point2D p) {return Rect(a+p, b+p);}
  inline Point2D Center() const {return a + Size()/2;}
  bool IsFullyInside(const Rect& other){
    return a.x >= other.a.x && a.y >= other.a.y && b.x <= other.b.x && b.y <= other.b.y;
  }
};

template <typename T>
bool Point2D_<T>::IsInside(const Rect& r) const{
  return IsInside(r.a, r.Size());
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


// A custom structure representing keypress info.
struct KeyData{
  // Called by SDLMain::Init(). Builds a keycode lookup map.
  static void InitKeymap();
  enum KeyType{
    Unknown,
    Text, Letter, Digit, Symbol,
    Backspace, Delete,
    Return,
    Shift, Ctrl, Alt,
    Escape
  };
  KeyData(const SDL_KeyboardEvent&);
  KeyData(std::string);
  bool pressed = true;
  bool repeat = false;
  bool shift = false, ctrl = false, alt = false;
  KeyType type;
  std::string symbol = "";
  bool IsPrintable() const {return type == Letter || type == Digit || type == Symbol || type == Text;}
  bool IsTrig() const {return pressed && !repeat;}
};

// Custom enum for identifying mouse buttons
enum class MouseButton{
  Left,
  Right,
  Middle,
  WheelUp,
  WheelDown
};

struct MidiMessage{
  enum class Type{
    NoteOn,
    NoteOff,
    Control,
  };
  Type type;
  unsigned char channel;
  unsigned char number;
  unsigned char velocity;
  unsigned char value;
};


class Utilities{
private:
  Utilities() = delete; // static
public:
  static void Wait(int ms);
  static void WaitOS(int ms);
  static bool GetFileExists(std::string name);
  static std::string GetDir(std::string);
  static std::string GetFilename(std::string);
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
  static std::string PrettyFloat(float val);
};

} // namespace AlgAudio

#endif //UTILITIES_HPP
