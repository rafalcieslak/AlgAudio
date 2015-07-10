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
  int width, height;
  bool operator==(const Size2D& other) const{ return (width == other.width && height == other.height); }
  bool operator!=(const Size2D& other) const{ return !(*this == other); }
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
