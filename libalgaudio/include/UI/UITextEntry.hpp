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
#include "Window.hpp"

namespace AlgAudio{

class UITextEntry : public UIWidget{
public:
  static std::shared_ptr<UITextEntry> Create(std::shared_ptr<Window> parent_window);
  virtual void CustomDraw(DrawContext& c) override;
  Signal<> on_complete; // Triggered when the Return key is pressed while entering text.
  void SetText(std::string text);
  std::string GetText() const;
  virtual void OnFocusChanged() override;
  virtual void OnKeyboard(KeyData) override;
private:
  UITextEntry(std::shared_ptr<Window>);
  std::string text;
  std::shared_ptr<SDLTextTexture> text_texture;
};

} // namespace AlgAudio

#endif // UITEXTENTRY_HPP
