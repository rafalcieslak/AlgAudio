#ifndef LIBLOADER_HPP
#define LIBLOADER_HPP

#include <string>
#include <windows.h>

#include "Module.hpp"
#include "Utilities.hpp"

namespace AlgAudio{

typedef void* (create_instance_func_t)(const char *);

struct LibLoadingException : public Exception{
  LibLoadingException(std::string p, std::string t) : Exception(t), path(p) {};
  virtual std::string what() override {return "When loading file '" + path + "': " + text;}
  std::string path;
};

class LibLoader{
public:
    LibLoader(std::string);
    const std::string path;
    Module* AskForInstance(std::string);
    ~LibLoader();
private:
    HINSTANCE hLib;
    create_instance_func_t* create_instance_func;
};

} // namespace AlgAudio
#endif // LIBLOADER_HPP
