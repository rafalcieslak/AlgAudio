#ifndef CONSOLE_HPP
#define CONSOLE_HPP
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

#include "Window.hpp"
#include "UI/UITextArea.hpp"
#include "UI/UIButton.hpp"

namespace AlgAudio{

/** A window that displays all sclang subprocess output. */
class Console : public Window{
public:
 static std::shared_ptr<Console> Create();
private:
 Console();
 void init();
 std::shared_ptr<UIButton> clipboard_button;
 std::shared_ptr<UITextArea> textarea;
};

}// namespace AlgAudio

#endif // CONSOLE_HPP
