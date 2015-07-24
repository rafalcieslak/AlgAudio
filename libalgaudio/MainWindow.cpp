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

namespace AlgAudio{

MainWindow::MainWindow() : Window("AlgAudio",900,600){
}

void MainWindow::init(){
  auto mainvbox = UIVBox::Create(shared_from_this());
  auto addbutton = UIButton::Create(shared_from_this()," Add new ");
  auto toolbarbox = UIHBox::Create(shared_from_this());
  auto selector = ModuleSelector::Create(shared_from_this());
  auto layered = UILayered::Create(shared_from_this());
  auto label1 = UILabel::Create(shared_from_this(), "This is one label\nIdeally, it would be multiline!", 16);

  toolbarbox->SetBackColor(Theme::Get("bg-main-alt"));

  layered->Insert(label1);
  layered->Insert(selector);

  selector->Populate();

  subscriptions += addbutton->on_clicked.Subscribe([=](){
    if(flag){
      selector->Hide();
      flag = false;
    }else{
      selector->Expose();
      flag = true;
    }
  });

  toolbarbox->Insert(addbutton, UIBox::PackMode::TIGHT);
  mainvbox->Insert(toolbarbox, UIBox::PackMode::TIGHT);
  mainvbox->Insert(layered, UIBox::PackMode::WIDE);
  Insert(mainvbox);
}

std::shared_ptr<MainWindow> MainWindow::Create(){
  auto res = std::shared_ptr<MainWindow>( new MainWindow());
  res->init();
  return res;
}

} // namespace AlgAudio
