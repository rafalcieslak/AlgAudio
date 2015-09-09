#ifndef IALERTABLE_HPP
#define IALERTABLE_HPP
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

#include "Utilities.hpp"
#include "Theme.hpp"
#include "LateReturn.hpp"

namespace AlgAudio{

#undef IGNORE

enum class ButtonID : int{
  OK,
  CANCEL,
  YES,
  NO,
  QUIT,
  IGNORE,
  CUSTOM1,
  CUSTOM2,
};

#undef ERROR
enum class AlertType : int{
  NONE,
  INFO,
  WARNING,
  ERROR,
};

namespace Exceptions{
struct WindowNotAlertable : public Exception{
  WindowNotAlertable(std::string t) : Exception(t) {}
};
} //namespace Exceptions

/* This is an interface implemented by all classes which are capable of
 * displaying an alert. For example, the MainWindow is such class.
 */
class IAlertable {
public:
  // Returns 0 if the first button was clicked, 1, if the second.
  virtual LateReturn<int> ShowSimpleAlert(
    std::string message,
    std::string button1_text,
    std::string button2_text,
    AlertType type = AlertType::NONE,
    Color button1_color = Theme::Get("bg-button-neutral"),
    Color button2_color = Theme::Get("bg-button-neutral")
  ) = 0;
  virtual LateReturn<> ShowErrorAlert(std::string message, std::string button_text) = 0;
};

} //namespace AlgAudio

#endif // IALERTABLE_HPP
