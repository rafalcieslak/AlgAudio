#include "UI/UICheckbox.hpp"
#include "UI/UIBox.hpp"
#include "UI/UILabel.hpp"
#include "UI/UIButton.hpp"
#include "Theme.hpp"
#include <iostream>

namespace AlgAudio{

UICheckbox::UICheckbox(std::weak_ptr<Window> w, std::string t) : UIClickable(w), text(t){
}

std::shared_ptr<UICheckbox> UICheckbox::Create(std::weak_ptr<Window> w, std::string text){
  std::shared_ptr<UICheckbox> res(new UICheckbox(w,text));
  res->Init();
  return res;
}

void UICheckbox::CustomDraw(DrawContext& c){
  child_box->Draw(c);
}

void UICheckbox::Init(){
  child_box = UIHBox::Create(window);
  child_box->parent = shared_from_this();
  child_button = UIButton::Create(window,"   ");
  child_label = UILabel::Create(window,text);
  child_box->Insert(child_button, UIBox::PackMode::TIGHT);
  child_box->Insert(child_label, UIBox::PackMode::WIDE);

  SetClearColor(Theme::Get("bg-main"));
  on_clicked.SubscribeForever([&](){
    if(active){
      child_button->SetText("   ");
      on_toggled.Happen(false);
      active = false;
    }else{
      child_button->SetText(" X ");
      on_toggled.Happen(true);
      active = true;
    };
  });
  on_pointed.SubscribeForever([&](bool p){
    if(p) SetOverlayColor(Color(255,255,255,20));
    else SetOverlayColor(Color(0,0,0,0));
  });
}

void UICheckbox::CustomResize(Size2D s){
  child_box->Resize(s);
}

void UICheckbox::SetText(std::string t){
  child_label->SetText(t);
}
void UICheckbox::OnChildRequestedSizeChanged(){
  SetRequestedSize(child_box->GetRequestedSize());
}


} // namespace AlgAudio
