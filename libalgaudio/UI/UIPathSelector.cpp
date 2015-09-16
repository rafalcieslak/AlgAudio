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
#include "nfd.h"
#include "UI/UIPathSelector.hpp"
#include "UI/UIBox.hpp"
#include "UI/UITextEntry.hpp"
#include "UI/UIButton.hpp"

namespace AlgAudio{
  
UIPathSelector::UIPathSelector(std::weak_ptr<Window> parent_window, std::string p)
  : UIHBox(parent_window), path(p)
{
  
}

std::shared_ptr<UIPathSelector> UIPathSelector::Create(std::weak_ptr<Window> w, std::string text)
{
  std::shared_ptr<UIPathSelector> res(new UIPathSelector(w,text));
  res->Init();
  return res;
}

void UIPathSelector::Init()
{
  child_button = UIButton::Create(window,"...");
  child_entry = UITextEntry::Create(window,path);
  Insert(child_entry, UIBox::PackMode::WIDE);
  Insert(child_button, UIBox::PackMode::TIGHT);
  
  child_button->SetBorder(false);
  child_button->SetCustomSize(Size2D(16,16));
  child_button->SetInnerMargin(0);
  
  subscriptions += child_button->on_clicked.Subscribe([this](){
    OpenDialog();
  });
  subscriptions += child_entry->on_edited.Subscribe([this](){
    path = child_entry->GetText();
  });
}

void UIPathSelector::OpenDialog(){
  nfdchar_t *outPath = nullptr;
  nfdresult_t result = NFD_OpenDialog( NULL, (path=="") ? NULL : path.c_str() , &outPath );
  if(result == NFD_OKAY){
    path = outPath;
    on_selected.Happen(path);
    child_entry->SetText(path);
    free(outPath);
  }
}
  
} // namespace AlgAudio
