#ifndef UITEXTAREA_HPP
#define UITEXTAREA_HPP
#include <vector>
#include "UIWidget.hpp"

namespace AlgAudio{

class UITextArea : public UIWidget{
public:
  static std::shared_ptr<UITextArea> Create(std::weak_ptr<Window> parent_window, Color c_fg, Color c_bg = Color(0,0,0));
  void Clear();
  void Push(std::string);
  void PushLine(std::string);
  void SetBottomAligned(bool b){
    bottom_alligned = b;
  }
  virtual void CustomDraw(DrawContext& c) override;
private:
  UITextArea(std::weak_ptr<Window> parent_window, Color c_fg, Color c_bg = Color(0,0,0));
  std::vector<std::string> text;
  std::vector<std::shared_ptr<SDLTexture>> textures;
  bool bottom_alligned = false;
  Color c_fg, c_bg;
};

} // namespace AlgAudio

#endif // UITEXTAREA_HPP
