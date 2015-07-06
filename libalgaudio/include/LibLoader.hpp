#ifndef LIBLOADER_HPP
#define LIBLOADER_HPP

#include "Module.hpp"
#include <string>
#include <windows.h>

typedef void* (create_instance_func_t)(const char *);

struct LibLoadingException{
  LibLoadingException(std::string p, std::string t) : path(p), text(t) {};
  std::string path;
  std::string text;
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

#endif // LIBLOADER_HPP
