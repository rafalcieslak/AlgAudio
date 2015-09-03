#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP
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
#include "ModuleSelector.hpp"
#include "UI/UIBox.hpp"
#include "UI/UIButton.hpp"
#include "UI/UILayered.hpp"
#include "UI/UILabel.hpp"
#include "UI/UISeparator.hpp"
#include "UI/UIAlert.hpp"
#include "UI/UISeparator.hpp"
#include "UI/UICentered.hpp"
#include "UI/UILabel.hpp"
#include "CanvasView.hpp"
#include "Alertable.hpp"

namespace AlgAudio{

/* MainWindow is an implementation of a Window specialised to display
 * the main window interface, embeding, among others, a CanvasView, a
 * ModuleSelector etc.
 * This class implements the Alertable interface.
 */
class MainWindow : public Window{
public:
  static std::shared_ptr<MainWindow> Create();
  // Returns 0 if the first button was clicked, 1, if the second.
  LateReturn<int> ShowSimpleAlert(
    std::string message,
    std::string button1_text,
    std::string button2_text,
    AlertType type = AlertType::NONE,
    Color button1_color = Theme::Get("bg-button-neutral"),
    Color button2_color = Theme::Get("bg-button-neutral")
  ) override;
  LateReturn<> ShowErrorAlert(std::string message, std::string button_text) override;
  enum class SaveAlertReply{ Cancel, Discard, Save};
  LateReturn<SaveAlertReply> ShowDoYouWantToSaveAlert();
  
  void ProcessCloseEvent() override;
  void ProcessKeyboardEvent(KeyData data) override;
  
  // Returns true on successful save 
  bool SaveAs();
  bool Save();
  bool Save(std::string path);
  
  // This helper method asks the user to save the file, potentially saves it,
  // and afterwards invokes a function.
  void AskToSaveBeforeCalling(std::function<void()> f);
  
  void Open();
  void New();
private:
  MainWindow();
  void init();
  
  void UpdatePathLabel();

std::shared_ptr<UIVBox> mainvbox;
   std::shared_ptr<UIHBox> toolbarbox;
      std::shared_ptr<UIButton> addbutton;
      std::shared_ptr<UIButton> removebutton;
      std::shared_ptr<UISeparator> toolbar_separator1;
      std::shared_ptr<UIButton> newbutton;
      std::shared_ptr<UIButton> openbutton;
      std::shared_ptr<UIButton> savebutton;
      std::shared_ptr<UIButton> saveasbutton;
      std::shared_ptr<UISeparator> toolbar_separator2;
      std::shared_ptr<UIButton> quitbutton;
   std::shared_ptr<UILayered> layered;
      std::shared_ptr<UIVBox> canvasbox;
         std::shared_ptr<UIHBox> canvaspathbox;
            std::shared_ptr<UIButton> canvaspathback;
            std::shared_ptr<UILabel> canvaspathlabel;
         std::shared_ptr<CanvasView> canvasview;
      std::shared_ptr<ModuleSelector> selector;
      std::shared_ptr<UILayered> layered_alert;
         std::shared_ptr<UICentered> centered_alert;
            std::shared_ptr<UIAlert> alert;    
  
  
  Subscription sub_alert_reply;
  
  // Stored for SaveAs.
  std::string current_file_path = "";
  std::string file_name = "Unsaved file";
};

} //namespace AlgAudio
#endif // MAINWINDOW_HPP
