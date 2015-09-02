#ifndef LIBLOADER_HPP
#define LIBLOADER_HPP
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
#include <map>
#include <memory>

#ifndef __unix__
  #include <windows.h>
#endif

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

/* This class is a wrapper for dynamically loadable libraries.
 * The static part of this class opens up and loads dynamic libraries, while
 * the instantiable part requests the Module instances from an AA file.
 */
class LibLoader{
public:
  // Calls create_instance asking for a new module instance of a given class name.
  std::shared_ptr<Module> AskForInstance(std::string);
  // Returns the deleter function from the shared library.
  deleter_t* GetDeleter() {return deleter_func;}
  // Loads (or returns an existing instance, if cached) of a library from the
  // provided path.
  static std::shared_ptr<LibLoader> GetByPath(std::string path);

  ~LibLoader();
private:
  LibLoader(std::string);
  // This method loads a new library into cache.
  static std::shared_ptr<LibLoader> Preload(std::string path);
  // The path to maintained library.
  const std::string path;

  // Pointers to the create and delete functions as loaded from the dynamic library.
  create_instance_func_t* create_instance_func = nullptr;
  deleter_t* deleter_func = nullptr;

#ifdef __unix__
  void* handle;
#else
  HINSTANCE hLib;
#endif

  // Library cache.
  static std::map<std::string, std::shared_ptr<LibLoader>> libs_by_path;
};

} // namespace AlgAudio
#endif // LIBLOADER_HPP
