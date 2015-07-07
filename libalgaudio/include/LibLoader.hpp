#ifndef LIBLOADER_HPP
#define LIBLOADER_HPP

#include <string>
#include <windows.h>
#include <map>
#include <memory>

#include "Module.hpp"
#include "Utilities.hpp"

namespace AlgAudio{

typedef void* (create_instance_func_t)(const char *);
typedef void (deleter_t)(void*);

struct LibLoadingException : public Exception{
  LibLoadingException(std::string p, std::string t) : Exception(t), path(p) {};
  virtual std::string what() override {return "When loading file '" + path + "': " + text;}
  std::string path;
};

class LibLoader{
public:
  Module* AskForInstance(std::string);
  deleter_t* GetDeleter() {return deleter_func;}
  static std::shared_ptr<LibLoader> GetByPath(std::string path);
  static std::shared_ptr<LibLoader> Preload(std::string path);
  // Do not use the constructor; it is public only because shared_ptr need it to be.
  // Use GetByPath instead.
  LibLoader(std::string);
  ~LibLoader();
private:
  const std::string path;
  HINSTANCE hLib;
  create_instance_func_t* create_instance_func;
  deleter_t* deleter_func;

  static std::map<std::string, std::shared_ptr<LibLoader>> libs_by_path;
};

} // namespace AlgAudio
#endif // LIBLOADER_HPP
