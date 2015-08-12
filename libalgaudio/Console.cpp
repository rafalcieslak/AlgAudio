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

#include "Console.hpp"
#include "SCLang.hpp"
#include "UI/UIBox.hpp"

namespace AlgAudio{

Console::Console() : Window("SCLang console",500,400,false){

}

std::shared_ptr<Console> Console::Create(){
  auto res = std::shared_ptr<Console>( new Console() );
  res->init();
  return res;
}

void Console::init(){
  auto mainvbox = UIVBox::Create(shared_from_this());
  textarea = UITextArea::Create(shared_from_this(), Color(255,255,255), Color(0,0,0));
  clipboard_button = UIButton::Create(shared_from_this(), "Copy console output to clipboard");
  textarea->SetBottomAligned(true);
  clipboard_button->SetFontSize(13);
  clipboard_button->SetColors(Theme::Get("text-generic"),Theme::Get("bg-button-neutral"));

  mainvbox->Insert(textarea, UIBox::PackMode::WIDE);
  mainvbox->Insert(clipboard_button, UIBox::PackMode::TIGHT);
  Insert(mainvbox);

  subscriptions += SCLang::on_line_received.Subscribe([=](std::string line){
    textarea->PushLine(line);
  });
  subscriptions += clipboard_button->on_clicked.Subscribe([=](){
    Utilities::CopyToClipboard(textarea->GetAllText());
  });
}

} // namespace AlgAudio
