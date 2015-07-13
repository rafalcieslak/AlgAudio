#ifndef UIVBOX_HPP
#define UIVBOX_HPP
#include <vector>
#include <functional>
#include "UI/UIWidget.hpp"

namespace AlgAudio{

class UIBox : public UIWidget{
public:
  enum PackMode{
    TIGHT,
    WIDE
  };
  virtual void CustomDraw(DrawContext& c) override;
  virtual void CustomResize(Size2D) override;
  virtual void OnChildRequestedSizeChanged() override;
  void Insert(std::shared_ptr<UIWidget> w, PackMode m);
  void SetPadding(unsigned int padding);
  virtual void OnMouseButton(bool down, short b,Point2D) override;
  virtual void OnMotion(Point2D p1, Point2D p2) override;
  virtual void OnMotionEnter(Point2D) override;
  virtual void OnMotionLeave(Point2D) override;
private:
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
  Point2D GetChildLocation(unsigned int n);
  unsigned int GetTotalSize();
  unsigned int GetChildMaxContra();
  // For given coordinates, this function returns the child number inside which
  // the point is, or -1 if none.
  int InWhich(Point2D);
protected:
  UIBox(std::weak_ptr<Window> parent_window);
  // These functions provide access to width/height from Size2D in an abstract
  // manner. In a vertical box, the DirectionalDimension will refer to height,
  // and in a horizontal box it will refer to width. The
  // ContradirectionalDimension must refer to the other dimension than
  // directional_dimension.
  virtual int DirectionalDimension(Size2D s) const = 0;
  virtual int ContradirectionalDimension(Size2D s) const = 0;
  virtual int DirectionalDimension(Point2D s) const = 0;
  virtual int ContradirectionalDimension(Point2D s) const = 0;
  virtual Size2D DirectionalSize2D(int direction, int contra) const = 0;
  virtual Point2D DirectionalPoint2D(int direction, int contra) const = 0;
};

class UIVBox : public UIBox{
private:
  UIVBox(std::weak_ptr<Window> parent_window);
public:
  static std::shared_ptr<UIVBox> Create(std::weak_ptr<Window> parent_window);
  // Specify what a vertical direction is
  inline virtual int DirectionalDimension(Size2D s) const override {return s.height;}
  inline virtual int ContradirectionalDimension(Size2D s) const override {return s.width;}
  inline virtual int DirectionalDimension(Point2D s) const override {return s.y;}
  inline virtual int ContradirectionalDimension(Point2D s) const override {return s.x;}
  inline virtual Size2D DirectionalSize2D(int direction, int contra) const override {return Size2D(contra, direction);}
  inline virtual Point2D DirectionalPoint2D(int direction, int contra) const override {return Point2D(contra, direction);}
};
class UIHBox : public UIBox{
private:
  UIHBox(std::weak_ptr<Window> parent_window);
public:
  static std::shared_ptr<UIHBox> Create(std::weak_ptr<Window> parent_window);
  // Specify what a horizontal direction is
  inline virtual int DirectionalDimension(Size2D s) const override {return s.width;}
  inline virtual int ContradirectionalDimension(Size2D s) const override {return s.height;}
  inline virtual int DirectionalDimension(Point2D s) const override {return s.x;}
  inline virtual int ContradirectionalDimension(Point2D s) const override {return s.y;}
  inline virtual Size2D DirectionalSize2D(int direction, int contra) const override {return Size2D(direction,contra);}
  inline virtual Point2D DirectionalPoint2D(int direction, int contra) const override {return Point2D(direction,contra);}
};

} // namespace AlgAudio

#endif // UIVBOX_HPP
