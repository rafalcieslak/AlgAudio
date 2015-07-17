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
  create_instance_func_t* create_instance_func = nullptr;
  deleter_t* deleter_func = nullptr;

#ifdef __unix__
  void* handle;
#else
  HINSTANCE hLib;
#endif

  static std::map<std::string, std::shared_ptr<LibLoader>> libs_by_path;
};

} // namespace AlgAudio
#endif // LIBLOADER_HPP
