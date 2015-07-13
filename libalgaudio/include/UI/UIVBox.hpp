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
  void SetPadding(unsigned int padding);
  virtual void OnMouseButton(bool down, short b,int x,int y) override;
  virtual void OnMotion(int x1, int y1, int x2, int y2) override;
  virtual void OnMotionEnter(int x, int y) override;
  virtual void OnMotionLeave(int x, int y) override;
private:
  UIVBox(std::weak_ptr<Window> parent_window);
  struct PackData{
    std::shared_ptr<UIWidget> child;
    PackMode mode;
    int size;
  };
  std::vector<PackData> children;
  unsigned int padding = 0;
  Size2D GetChildSize(unsigned int n);
  void TriggerChildResizes();
  void RecalculateChildSizes(unsigned int available_space);
  unsigned int GetChildLocation(unsigned int n);
  unsigned int GetTotalSize();
  unsigned int GetChildMaxWidth();
  // For given coordinates, this function returns the child number inside which
  // the point is, or -1 if none.
  int InWhich(int x, int y);
};

} // namespace AlgAudio

#endif // UIVBOX_HPP
