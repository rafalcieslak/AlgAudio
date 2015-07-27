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
#include "ModuleSelector.hpp"
#include "UI/UIBox.hpp"
#include "UI/UIButton.hpp"
#include "UI/UILayered.hpp"
#include "UI/UILabel.hpp"
#include "UI/UISeparator.hpp"
#include "CanvasView.hpp"

namespace AlgAudio{

MainWindow::MainWindow() : Window("AlgAudio",900,600){
}

void MainWindow::init(){
  auto mainvbox = UIVBox::Create(shared_from_this());
  auto addbutton = UIButton::Create(shared_from_this()," Add new ");
  auto quitbutton = UIButton::Create(shared_from_this()," Quit ");
  auto toolbarbox = UIHBox::Create(shared_from_this());
  auto toolbar_separator = UISeparator::Create(shared_from_this());
  auto selector = ModuleSelector::Create(shared_from_this());
  auto layered = UILayered::Create(shared_from_this());
  //auto label1 = UILabel::Create(shared_from_this(), "This is one label\nIdeally, it would be multiline!", 16);
  auto canvasview = CanvasView::CreateEmpty(shared_from_this());

  addbutton ->SetColors(Theme::Get("text-button"),Theme::Get("bg-button-positive"));
  quitbutton->SetColors(Theme::Get("text-button"),Theme::Get("bg-button-negative"));
  toolbarbox->SetBackColor(Theme::Get("bg-main-alt"));

  layered->Insert(canvasview);
  layered->Insert(selector);

  selector->Populate();

  subscriptions += addbutton->on_clicked.Subscribe([=](){
    if(selector->IsExposed()){
      selector->Hide();
    }else{
      selector->Expose();
    }
  });
  subscriptions += quitbutton->on_clicked.Subscribe([=](){
    on_close.Happen();
  });

  toolbarbox->Insert(addbutton, UIBox::PackMode::TIGHT);
  toolbarbox->Insert(toolbar_separator, UIBox::PackMode::WIDE);
  toolbarbox->Insert(quitbutton, UIBox::PackMode::TIGHT);
  mainvbox->Insert(toolbarbox, UIBox::PackMode::TIGHT);
  mainvbox->Insert(layered, UIBox::PackMode::WIDE);
  Insert(mainvbox);

  subscriptions += selector->on_complete.Subscribe([=](std::string id){
    selector->Hide();
    if(id == "") return;
    std::cout << "Selected " << id << std::endl;
    canvasview->AddModule(id,Point2D(50,50));
  });
}

std::shared_ptr<MainWindow> MainWindow::Create(){
  auto res = std::shared_ptr<MainWindow>( new MainWindow());
  res->init();
  return res;
}

} // namespace AlgAudio
