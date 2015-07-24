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
#include "UI/UIBox.hpp"
#include "UI/UIButton.hpp"
#include "UI/UILayered.hpp"
#include "UI/UILabel.hpp"
#include "UI/UIPosition.hpp"
#include "UI/UIAnimDrawer.hpp"
#include "UI/UIList.hpp"

namespace AlgAudio{

MainWindow::MainWindow() : Window("AlgAudio",900,600){
}

void MainWindow::init(){
  auto mainvbox = UIVBox::Create(shared_from_this());
  auto addbutton = UIButton::Create(shared_from_this()," Add new ");
  auto toolbarbox = UIHBox::Create(shared_from_this());
  auto selectorbox = UIHBox::Create(shared_from_this());
  auto layered = UILayered::Create(shared_from_this());
  auto label1 = UILabel::Create(shared_from_this(), "This is one label\nIdeally, it would be multiline!", 16);
  auto drawer = UIAnimDrawer::Create(shared_from_this(), Direction_LEFT);
  auto list = UIList::Create(shared_from_this());

  toolbarbox->SetBackColor(Theme::Get("bg-main-alt"));

  layered->Insert(label1);
  layered->Insert(selectorbox);
  selectorbox->Insert(list,UIBox::PackMode::TIGHT);
  //drawer->Insert(position);

  list->SetBackColor(Color(0,0,0,90));
  list->AddItem("id1", "First position");
  list->AddItem("id2", "Another position");
  list->AddItem("id3", "One more position");
  list->AddItem("id4", "Last list position");

  subscriptions += addbutton->on_clicked.Subscribe([=](){
    if(flag){
      //drawer->StartHide(1.0);
      flag = false;
    }else{
      //drawer->StartShow(1.0);
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
