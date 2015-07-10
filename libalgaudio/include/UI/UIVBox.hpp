#ifndef UIVBOX_HPP
#define UIVBOX_HPP
#include <vector>
#include "UI/UIWidget.hpp"

namespace AlgAudio{

class UIVBox : public UIWidget{
public:
  enum PackMode{
    TIGHT,
    WIDE
  };
  static std::shared_ptr<UIVBox> Create(std::weak_ptr<Window> parent_window);
  virtual void CustomDraw(DrawContext& c) override;
  void Insert(std::shared_ptr<UIWidget> w, PackMode m);
private:
  UIVBox(std::weak_ptr<Window> parent_window);
  struct PackData{
    std::shared_ptr<UIWidget> child;
    PackMode mode;
    int size;
  };
  std::vector<PackData> children;
};

} // namespace AlgAudio

#endif // UIVBOX_HPP
