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
#include "Signal.hpp"

struct SDL_Color;

namespace AlgAudio{

struct Size2D{
  Size2D(int w = 0, int h = 0) : width(w), height(h) {}
  bool Fits(const Size2D& other) const{ return (width >= other.width && height >= other.height); }
  bool IsEmpty() const {return width <= 0 || height <= 0; }
  int width, height;
  bool operator==(const Size2D& other) const{ return (width == other.width && height == other.height); }
  bool operator!=(const Size2D& other) const{ return !(*this == other); }
  std::string ToString() {return "{" + std::to_string(width) + ", " + std::to_string(height) + " }"; }
  Size2D operator+(const Size2D& other) const { return Size2D(width + other.width, height + other.height);}
  Size2D operator-(const Size2D& other) const { return Size2D(width - other.width, height - other.height);}
};

// Same as size2d, but never interchangable. TODO: Turn both into a common base class.
struct Point2D{
  Point2D(int x_ = 0, int y_ = 0) : x(x_), y(y_) {}
  bool Fits(const Point2D& other) const{ return (x >= other.x && y >= other.y); }
  int x, y;
  bool operator==(const Point2D& other) const{ return (x == other.x && y == other.y); }
  bool operator!=(const Point2D& other) const{ return !(*this == other); }
  std::string ToString() {return "{" + std::to_string(x) + ", " + std::to_string(y) + " }"; }
  Point2D operator+(const Point2D& other) const { return Point2D(x + other.x, y + other.y);}
  Point2D operator-(const Point2D& other) const { return Point2D(x - other.x, y - other.y);}
};

class Exception{
public:
  Exception(std::string t) : text(t) {};
  virtual std::string what() {return text;}
  virtual ~Exception() {}
protected:
  std::string text;
};

class Utilities{
private:
  Utilities() = delete; // static
public:
  // This signal is called by the main thread whenever it has some spare time.
  // This is roughly once per frame (possibly a skipped frame). When subscribing
  // to this signal, make sure your callee does not take much time to return,
  // or whole app's performance may decrease. By no means subscribe with a
  // blocking callee!
  static Signal<> global_idle;
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

  // String operations
  static std::vector<std::string> SplitString(std::string str, std::string delimiter);
  static std::string JoinString(std::vector<std::string> str, std::string c);
  static void Replace(std::string& str, const std::string& from, const std::string& to);
};

} // namespace AlgAudio

#endif //UTILITIES_HPP
