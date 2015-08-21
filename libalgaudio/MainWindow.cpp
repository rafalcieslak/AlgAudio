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
#include "MainWindow.hpp"
#include <fstream>
#include "nfd.h"

namespace AlgAudio{

MainWindow::MainWindow() : Window("AlgAudio",1200,800){
}

void MainWindow::init(){
  mainvbox = UIVBox::Create(shared_from_this());
  addbutton = UIButton::Create(shared_from_this()," Add new ");
  removebutton = UIButton::Create(shared_from_this(),"Remove selected");
  newbutton = UIButton::Create(shared_from_this()," New ");
  quitbutton = UIButton::Create(shared_from_this()," Quit ");
  openbutton = UIButton::Create(shared_from_this()," Open ");
  savebutton = UIButton::Create(shared_from_this()," Save ");
  saveasbutton = UIButton::Create(shared_from_this()," Save as... ");
  toolbarbox = UIHBox::Create(shared_from_this());
  toolbar_separator1 = UISeparator::Create(shared_from_this());
  toolbar_separator2 = UISeparator::Create(shared_from_this());
  selector = ModuleSelector::Create(shared_from_this());
  layered = UILayered::Create(shared_from_this());
  canvasview = CanvasView::CreateEmpty(shared_from_this());
  layered_alert = UILayered::Create(shared_from_this());
  centered_alert = UICentered::Create(shared_from_this());
  //alert = UIAlert::Create(shared_from_this(),"");

  addbutton ->SetColors(Theme::Get("text-button"),Theme::Get("bg-button-positive"));
  quitbutton->SetColors(Theme::Get("text-button"),Theme::Get("bg-button-negative"));
  toolbarbox->SetBackColor(Theme::Get("bg-main-alt"));

  layered->Insert(canvasview);
  layered->Insert(selector);

  selector->Populate();

  subscriptions += addbutton->on_clicked.Subscribe([this](){
    if(selector->IsExposed()){
      selector->Hide();
    }else{
      selector->Expose();
    }
  });
  subscriptions += removebutton->on_clicked.Subscribe([this](){
    canvasview->RemoveSelected();
  });
  subscriptions += quitbutton->on_clicked.Subscribe([this](){
    on_close.Happen();
  });
  subscriptions += newbutton->on_clicked.Subscribe([this](){New();});
  subscriptions += saveasbutton->on_clicked.Subscribe([this](){SaveAs();});
  subscriptions += savebutton->on_clicked.Subscribe([this](){Save();});
  subscriptions += openbutton->on_clicked.Subscribe([this](){Open();});

  toolbarbox->Insert(addbutton, UIBox::PackMode::TIGHT);
  toolbarbox->Insert(removebutton, UIBox::PackMode::TIGHT);
  toolbarbox->Insert(toolbar_separator1, UIBox::PackMode::TIGHT);
  toolbar_separator1->SetCustomSize(Size2D(30,0));
  toolbarbox->Insert(newbutton, UIBox::PackMode::TIGHT);
  toolbarbox->Insert(openbutton, UIBox::PackMode::TIGHT);
  toolbarbox->Insert(savebutton, UIBox::PackMode::TIGHT);
  toolbarbox->Insert(saveasbutton, UIBox::PackMode::TIGHT);
  toolbarbox->Insert(toolbar_separator2, UIBox::PackMode::WIDE);
  toolbarbox->Insert(quitbutton, UIBox::PackMode::TIGHT);
  mainvbox->Insert(toolbarbox, UIBox::PackMode::TIGHT);
  mainvbox->Insert(layered, UIBox::PackMode::WIDE);
  layered_alert->Insert(mainvbox);
  layered_alert->Insert(centered_alert);
  //centered_alert->Insert(alert);
  centered_alert->SetBackColor(Color(0,0,0,150));
  centered_alert->SetVisible(false);
  Insert(layered_alert);

  subscriptions += selector->on_complete.Subscribe([this](std::string id){
    selector->Hide();
    if(id == "") return;
    std::cout << "Selected " << id << std::endl;
    canvasview->AddModule(id,layered->last_mouse_pos);
/*
    ShowSimpleAlert("You have added a new module to the canvas!", "Cool!", "Meh.", AlertType::INFO)
      >>=
    [](int i) {
    if(i == 0) std::cout << "Cool indeed!" << std::endl;
    else       std::cout << "Yay." << std::endl;
    };
    */
  });

}


void MainWindow::SaveAs(){
  std::string def = (current_file_path == "") ? Utilities::GetCurrentDir() : current_file_path;
  nfdchar_t* path;
  nfdresult_t result = NFD_SaveDialog(
    NULL, // TODO: Investigate errors that happen when this is not set to null.
    def.c_str(),
    &path
  );
  if(result == NFD_OKAY){
    std::string p(path);
    free(path);
    Save(p);
  }
}
void MainWindow::Save(){
  if(current_file_path == ""){
    SaveAs();
  }else{
    Save(current_file_path);
  }
}
void MainWindow::Save(std::string path){
  std::ofstream file(path);
  if(!file){
    ShowErrorAlert("Save unsuccessful.\nFailed to access file " + path, "Cancel");
    return;
  }
  file << canvasview->GetCanvas()->XML_SaveAll() << std::endl;
  current_file_path = path;
  file.close();
}
void MainWindow::Open(){
  std::string def = (current_file_path == "") ? Utilities::GetCurrentDir() : GetDir(current_file_path);
  nfdchar_t *outPath = nullptr;
  nfdresult_t result = NFD_OpenDialog( NULL, def.c_str(), &outPath );
  if(result == NFD_OKAY){
    try{
      auto newcanvas = Canvas::CreateFromFile(path);
      canvasview->SwitchCanvas(newcanvas, true);
      current_file_path = path;
      free(path);
    }catch(SaveFileException ex){
      ShowErrorAlert("Opening file failed:\n\n" + ex.what());
      return;
    }catch(MissingTemplateException ex){
      // TODO: warning altert
      ShowErrorAlert("Opening file failed:\n\n" + ex.what());
    }
  }
}
void MainWindow::New(){
  canvasview->SwitchCanvas( Canvas::CreateEmpty() );
  current_file_path = "";
}

void MainWindow::ProcessKeyboardEvent(KeyData data){
  // Pass all key events to the canvasview.
  canvasview->OnKeyboard(data);
}

std::shared_ptr<MainWindow> MainWindow::Create(){
  auto res = std::shared_ptr<MainWindow>( new MainWindow());
  res->init();
  return res;
}

LateReturn<int> MainWindow::ShowSimpleAlert(std::string message, std::string button1_text, std::string button2_text, AlertType type, Color button1_color, Color button2_color){
  auto r = Relay<int>::Create();
  if(alert){
    std::cout << "WARNING: alert removed before it replied, because a new one is issued." << std::endl;
  }
  alert = UIAlert::Create(shared_from_this(),message);
  alert->SetButtons({UIAlert::ButtonData(button1_text, ButtonID::CUSTOM1, button1_color),
                     UIAlert::ButtonData(button2_text, ButtonID::CUSTOM2, button2_color)});
  alert->SetType(type);
  centered_alert->RemoveChild();
  centered_alert->SetVisible(true);
  centered_alert->Insert(alert);
  sub_alert_reply = alert->on_button_pressed.Subscribe([this,r](ButtonID id){
    centered_alert->SetVisible(false);
    alert = nullptr; // loose reference
    if(id == ButtonID::CUSTOM1) r.Return(0);
    if(id == ButtonID::CUSTOM2) r.Return(1);
  });
  return r;
}
LateReturn<> MainWindow::ShowErrorAlert(std::string message, std::string button_text){
  auto r = Relay<>::Create();
  if(alert){
    std::cout << "WARNING: alert removed before it replied, because a new one is issued." << std::endl;
  }
  alert = UIAlert::Create(shared_from_this(),message);
  alert->SetButtons({UIAlert::ButtonData(button_text, ButtonID::OK, Theme::Get("bg-button-neutral"))});
  alert->SetType(AlertType::ERROR);
  centered_alert->RemoveChild();
  centered_alert->SetVisible(true);
  centered_alert->Insert(alert);
  sub_alert_reply = alert->on_button_pressed.Subscribe([this,r](ButtonID id){
    centered_alert->SetVisible(false);
    // Cannot remove the child at this point. This is because the button_pressed signal
    // is called by a chain of CustomMousePresses. One of them is the UICentered,
    // and it calls code from it's child. If we remove ALL references to the child,
    // including the one owned by the UICentered, then it'll be destructed while
    // it's code is still being executed!
    // centered_alert->RemoveChild();
    alert = nullptr; // loose reference
    if(id == ButtonID::OK) r.Return();
  });
  return r;
}


} // namespace AlgAudio
