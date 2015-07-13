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
  virtual void CustomResize(Size2D) override;
  virtual void OnChildRequestedSizeChanged() override;
  void Insert(std::shared_ptr<UIWidget> w, PackMode m);
  int padding = 0;
private:
  UIVBox(std::weak_ptr<Window> parent_window);
  struct PackData{
    std::shared_ptr<UIWidget> child;
    PackMode mode;
    int size;
  };
  std::vector<PackData> children;
  Size2D GetChildSize(unsigned int n);
  void TriggerChildResizes();
  void RecalculateChildSizes(unsigned int available_space);
  unsigned int GetChildLocation(unsigned int n);
  unsigned int GetTotalSize();
  unsigned int GetChildMaxWidth();
};

} // namespace AlgAudio

#endif // UIVBOX_HPP
