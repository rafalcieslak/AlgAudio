#ifndef UTILITIES_HPP
#define UTILITIES_HPP
#include <string>
#include <vector>

namespace AlgAudio{

struct Size2D{
  Size2D(int w = 0, int h = 0) : width(w), height(h) {}
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
  static bool GetFileExists(std::string name);
  static std::string GetDir(std::string);
  static const char OSDirSeparator;
  static const std::string OSLibSuffix;
  static std::string ConvertUnipathToOSPath(const std::string& unipath);
  static std::vector<std::string> SplitString(std::string str, std::string delimiter);
  static std::string JoinString(std::vector<std::string> str, std::string c);
};

} // namespace AlgAudio

#endif //UTILITIES_HPP
