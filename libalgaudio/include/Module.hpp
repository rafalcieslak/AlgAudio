#ifndef MODULE_HPP
#define MODULE_HPP

#include <memory>
#include "DynamicallyLoadableClass.hpp"
#include "Signal.hpp"

namespace AlgAudio{

class ModuleTemplate;

class Module : public DynamicallyLoadableClass, public SubscriptionsManager{
public:
  Module(){};
  Module(void (*deleter)(void*)) : DynamicallyLoadableClass(deleter) {};
  Module(std::shared_ptr<ModuleTemplate> t) : templ(t) {};
  virtual ~Module() {};
  virtual void on_init() {};
  std::shared_ptr<ModuleTemplate> templ;
};

} // namespace AlgAudio

#endif //MODULE_HPP
