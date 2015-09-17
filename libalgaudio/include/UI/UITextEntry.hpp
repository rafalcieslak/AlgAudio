#ifndef UITEXTENTRY_HPP
#define UITEXTENTRY_HPP
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

#include "UIWidget.hpp"

namespace AlgAudio{

class UITextEntry : public UIWidget{
public:
  static std::shared_ptr<UITextEntry> Create(std::weak_ptr<Window> parent_window, std::string text = "");
  virtual void CustomDraw(DrawContext& c) override;
  Signal<> on_complete; // Triggered when the Return key is pressed while entering text.
  Signal<> on_edited; // Triggered when user does any change to the text.
  void SetText(std::string text);
  void SetFontSize(int size);
  std::string GetText() const {return text;}
  /** The default text is displayed when the typed in text is empty. Useful
   *  for setting up a placeholder message, or example value. */
  void SetDefaultText(std::string d) {
    default_text = d;
    if(text == "") UpdateText();
  }
  virtual void OnFocusChanged() override;
  virtual void OnKeyboard(KeyData) override;
private:
  UITextEntry(std::weak_ptr<Window>, std::string t = "");
  void Init();
  void UpdateText();
  std::string text;
  std::string default_text = "";
  std::shared_ptr<SDLTextTexture> text_texture;
  int fontsize = 12;
};

} // namespace AlgAudio

#endif // UITEXTENTRY_HPP
