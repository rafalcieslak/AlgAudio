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
#include "Exception.hpp"

struct SDL_Color;
struct SDL_KeyboardEvent;

namespace AlgAudio{

// Forward declarations
template <typename T=int>
struct Point2D_;
struct Rect;

typedef Point2D_<int> Point2D;

/** A structure representing object dimentions. */
struct Size2D{
  Size2D(int w = 0, int h = 0) : width(w), height(h) {}
  /** Returns true iff this size has both dimentions smaller than the other one. */
  bool Fits(const Size2D& other) const{ return (width >= other.width && height >= other.height); }
  /** Returns true iff this size has at least one dimention equal to zero. */
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

/** A structure for representing points on a two-dimentional plane. */
template <typename T>
struct Point2D_{
  Point2D_(T x_ = 0, T y_ = 0) : x(x_), y(y_) {}
  template <typename Q>
  Point2D_(const Point2D_<Q> &other) : x(other.x), y(other.y) {}
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
  /** Returns true iff the point is within the rectangle spanned by the given point and size. */
  bool IsInside(Point2D_<T> r, Size2D s) const { return (x >= r.x) && (x < r.x + s.width) && (y >= r.y) && (y < r.y + s.height);}
  /** Returns true iff the point is within the rectangle. */
  bool IsInside(const Rect& r) const;
  /** Calculates the Pythagorean distance between two points. */
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

/** A strcuture for representing rectangles. */
struct Rect{
  Point2D a;
  Point2D b;
  Rect() {}
  /** When constructing a rectangle spanned between two points, the order
   *  of the points is not significant. \see Point2D*/
  Rect(Point2D a_, Point2D b_){
         if(a_.x <= b_.x && a_.y <= b_.y) {a = a_; b = b_;}
    else if(a_.x >= b_.x && a_.y >= b_.y) {a = b_; b = a_;}
    else if(a_.x <= b_.x && a_.y >= b_.y) {a = {a_.x,b_.y}; b = {b_.x,a_.y};}
    else if(a_.x >= b_.x && a_.y <= b_.y) {a = {b_.x,a_.y}; b = {a_.x,b_.y};}
  }
  Rect(Point2D a_, Size2D s) : a(a_), b(a_+s) {}
  /** Returns the two-dimentional Size2D of this rectangle. \see Size2D */
  inline Size2D Size() const {
    auto q = b-a;
    return {q.x,q.y};
  }
  /** Returns a new rectangle which after a translation by the vector given as a point. */
  Rect MoveOffset(Point2D p) const {return Rect(a+p, b+p);}
  /** Returns the center point of this rectangle. \see Point2D */
  inline Point2D Center() const {return a + Size()/2;}
  /** Returns true iff this rectangle is fully contained within the other one. */
  bool IsFullyInside(const Rect& other){
    return a.x >= other.a.x && a.y >= other.a.y && b.x <= other.b.x && b.y <= other.b.y;
  }
};

template <typename T>
bool Point2D_<T>::IsInside(const Rect& r) const{
  return IsInside(r.a, r.Size());
}


typedef enum Direction{
  Direction_TOP,
  Direction_RIGHT,
  Direction_BOTTOM,
  Direction_LEFT,
} Direction;
typedef enum HorizAlignment{
  HorizAlignment_LEFT,
  HorizAlignment_CENTERED,
  HorizAlignment_RIGHT
} HorizAlignment;
typedef enum VertAlignment{
  VertAlignment_TOP,
  VertAlignment_CENTERED,
  VertAlignment_BOTTOM
} VertAlignment;

/** A custom structure representing keypress info. All functions that serve
 *  as reactions for keypresses work using KeyData. */
struct KeyData{
  /** Called by SDLMain::Init(). Builds a keycode lookup map so that
   *  SDL_KEYCODEs can be quickly translated to KeyData. */
  static void InitKeymap();
  
  enum KeyType{
    Unknown,
    Text, Letter, Digit, Symbol,
    Backspace, Delete,
    Return,
    Shift, Ctrl, Alt,
    Escape,
    Space
  };
  KeyData(const SDL_KeyboardEvent&);
  /** This constructor creates a Text key data (See SDL's TextInput) */
  KeyData(std::string);
  /** Marks whether this key event is about pressing or releasin a key */
  bool pressed = true;
  /** True, if this event is a system repetition of a held-down key */
  bool repeat = false;
  /** Modifiers state at the time of the event. */
  bool shift = false, ctrl = false, alt = false;
  /** The type of key this KeyData concerns. */
  KeyType type;
  /** The human-readable name or symbol of this key. */
  std::string symbol = "";
  /** Returns true if this key is a printable character or text. */
  bool IsPrintable() const {return type == Letter || type == Digit || type == Symbol || type == Text;}
  /** Returns true if this is a non-repeated key-press. */
  bool IsTrig() const {return pressed && !repeat;}
};

/** Custom enum for identifying mouse buttons. */
enum class MouseButton{
  Left,
  Right,
  Middle,
  WheelUp,
  WheelDown
};

/** A structure for representing MIDI messages. */
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

/** A static class encapsulating a number of useful helper functions that are
 * widely used thorough the rest of the source code. */
class Utilities{
private:
  Utilities() = delete; // static class
public:
  /** Sleeps for a given interval using SDL's functions. */
  static void Wait(int ms);
  /** Sleeps for a given interval using platform native functions. */
  static void WaitOS(int ms);
  /** Returns if a file at the given path exists. */
  static bool GetFileExists(std::string path);
  /** Trims a file path to the directory, stripping the filename. */
  static std::string GetDir(std::string);
  /** Trims a file path to the filename, stripping the directory path. */
  static std::string GetFilename(std::string);
  /** Returns the path to the current working directory. */
  static std::string GetCurrentDir();
  /** The platform-specific file path separator.
   *  This is '\' on Windows, and '/' on *nixes.*/
  static const char OSDirSeparator;
  /** The platform-specific shared library file extension.
   *  This is '.dll' on Windows, and '.so' on Linux. */
  static const std::string OSLibSuffix;
  /** Platform specific sclang binary name (sclang or sclang.exe) */
  static const std::string OSSCLangBinName;
  /** Replaces all '/' path separators to platform native separators. */
  static std::string ConvertUnipathToOSPath(const std::string& unipath);
  /** Replaces all platform native path separators to '/'. */
  static std::string ConvertOSpathToUniPath(const std::string& ospath);
  /** Searches for SuperCollider interpreter executable in the system. That
   *  includes searching in windows registry, $PATH, and a list of common
   *  locations.
   *  \returns The path to sclang.exe binary, or an empty string on failure. */
  static std::string FindSCLang();
  
  /** Pushes the string to sytem clipboard. */
  static void CopyToClipboard(std::string);

  /** For a given horizontal and vertical alignment, this function calculates
   *  as what offset a rectangle (represented by a Size2D) should be placed
   *  to match that alighments in another (usually larger) rectangle, and
   *  returns that offset as a Point2D.
   */
  static Point2D Align(HorizAlignment, VertAlignment, Size2D inner, Size2D outer);

  // String operations
  /** Splits an std::string into a vector of strings, using the provided delimiter.*/
  static std::vector<std::string> SplitString(std::string str, std::string delimiter);
  /** Joins a vector of strings into a single string placing string c between each consequent pair. */
  static std::string JoinString(std::vector<std::string> str, std::string c);
  /** This function replaces all ocurences of string from to string to in string str.*/
  static void Replace(std::string& str, const std::string& from, const std::string& to);
  /** Returns a string trimmed from heading and tailing whitespaces. */
  static std::string Trim(std::string);
  /** Returns a string trimmed from heading and tailing whitespaces on each line. */
  static std::string TrimAllLines(std::string);
  /** Returns a float formatted to a string in a way that uses only a few digits at each magnitude level. */
  static std::string PrettyFloat(float val);
  
  // Other
  /** Converts a midi node to the corresponding frequency. */
  static float mtof(float m);
  
  /** Sets the numeric locale to universal "C" */
  static void NumericLocaleSetUniversal();
  /** Restores locale settings to user defined. */
  static void NumericLocaleRestoreUserCustom();
  /** RAII-style locale decimal point changer. */
  struct LocaleDecPoint{
    LocaleDecPoint() {NumericLocaleSetUniversal();}
    ~LocaleDecPoint() {NumericLocaleRestoreUserCustom();}
  };
};

} // namespace AlgAudio

#endif //UTILITIES_HPP
