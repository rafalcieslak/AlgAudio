#ifndef MODULE_HPP
#define MODULE_HPP

namespace AlgAudio{

class Module{
public:
  Module() {};
  virtual ~Module() {};
  virtual void on_init() {};
};

} // namespace AlgAudio

#endif //MODULE_HPP
