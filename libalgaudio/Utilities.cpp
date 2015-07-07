#include "Utilities.hpp"
#include <fstream>

namespace AlgAudio {

bool Utilities::GetFileExists(std::string name)
{
  // For compilers that support C++14 experimental TS:
  // std::experimental::filesystem::exists(name);
  // For C++17:
  // std::filesystem::exists(name);
  return (bool)std::ifstream(name);
}

} // namespace AlgAudio
