#ifndef THEME_HPP
#define THEME_HPP
#include "Color.hpp"
#include <map>

namespace AlgAudio{

class Theme{
public:
  static void Init();
  static const Color& Get(const std::string&);
private:
  static std::map<std::string, Color> thememap;
};

} // namespace AlgAudio

#endif // THEME_HPP
