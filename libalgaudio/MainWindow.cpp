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

namespace AlgAudio{

MainWindow::MainWindow() : Window("AlgAudio",900,600){
}

void MainWindow::init(){
  mainvbox = UIVBox::Create(shared_from_this());
  addbutton = UIButton::Create(shared_from_this()," Add new ");
  removebutton = UIButton::Create(shared_from_this(),"Remove selected");
  quitbutton = UIButton::Create(shared_from_this()," Quit ");
  toolbarbox = UIHBox::Create(shared_from_this());
  toolbar_separator = UISeparator::Create(shared_from_this());
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

  toolbarbox->Insert(addbutton, UIBox::PackMode::TIGHT);
  toolbarbox->Insert(removebutton, UIBox::PackMode::TIGHT);
  toolbarbox->Insert(toolbar_separator, UIBox::PackMode::WIDE);
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
