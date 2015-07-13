#ifndef UTILITIES_HPP
#define UTILITIES_HPP
#include <string>
#include <vector>

struct SDL_Color;

namespace AlgAudio{

struct Color{
  Color(unsigned char R, unsigned char G, unsigned char B, unsigned char A=255) : r(R), g(G), b(B), alpha(A) {}
  unsigned char r,g,b,alpha;
  SDL_Color SDL();
  operator SDL_Color();
};

struct Size2D{
  Size2D(int w = 0, int h = 0) : width(w), height(h) {}
  bool Fits(const Size2D& other) const{ return (width >= other.width && height >= other.height); }
  bool IsEmpty() const {return width < 0 || height < 0; }
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
  static void Wait(int ms);
  static bool GetFileExists(std::string name);
  static std::string GetDir(std::string);
  static const char OSDirSeparator;
  static const std::string OSLibSuffix;
  static std::string ConvertUnipathToOSPath(const std::string& unipath);
  static std::vector<std::string> SplitString(std::string str, std::string delimiter);
  static std::string JoinString(std::vector<std::string> str, std::string c);
  static void Replace(std::string& str, const std::string& from, const std::string& to);
};

} // namespace AlgAudio

#endif //UTILITIES_HPP
