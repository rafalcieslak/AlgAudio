#ifndef UTILITIES_HPP
#define UTILITIES_HPP
#include <string>

namespace AlgAudio{

class Exception{
public:
  Exception(std::string t) : text(t) {};
  virtual std::string what() {return text;}
  virtual ~Exception() {}
protected:
  std::string text;
};

class Utilities{
public:

};

} // namespace AlgAudio

#endif //UTILITIES_HPP
