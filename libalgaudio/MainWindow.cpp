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
#include "CanvasXML.hpp"
#include "SCLang.hpp"
#include "Config.hpp"

#undef ERROR

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
  allnodes = UIButton::Create(shared_from_this()," QAN ");
  toolbarbox = UIHBox::Create(shared_from_this());
  toolbar_separator1 = UISeparator::Create(shared_from_this());
  toolbar_separator2 = UISeparator::Create(shared_from_this());
  toolbar_separator3 = UISeparator::Create(shared_from_this());
  selector = ModuleSelector::Create(shared_from_this());
  layered = UILayered::Create(shared_from_this());
  canvasview = CanvasView::CreateEmpty(shared_from_this());
  layered_alert = UILayered::Create(shared_from_this());
  centered_alert = UICentered::Create(shared_from_this());
  //alert = UIAlert::Create(shared_from_this(),"");
  canvaspathlabel = UILabel::Create(shared_from_this(),"   Currently editting: Unsaved file", 14);
  canvaspathlabel->SetAlignment(HorizAlignment_LEFT);
  canvaspathlabel->SetTextColor(Theme::Get("text-button"));
  canvaspathback = UIButton::Create(shared_from_this(),"<--");
  canvaspathback->SetColors(Theme::Get("text-button"),Theme::Get("bg-button-negative"));
  canvaspathback->SetBorder(false);
  canvaspathback->SetInnerMargin(1);
  canvaspathback->SetFontSize(12);
  canvaspathback->SetDisplayMode(UIWidget::DisplayMode::EmptySpace);
  canvasbox = UIVBox::Create(shared_from_this());
  canvaspathbox = UIHBox::Create(shared_from_this());

  addbutton ->SetColors(Theme::Get("text-button"),Theme::Get("bg-button-positive"));
  quitbutton->SetColors(Theme::Get("text-button"),Theme::Get("bg-button-negative"));
  toolbarbox->SetBackColor(Theme::Get("bg-main-alt"));
  
  canvaspathbox->Insert(canvaspathback, UIBox::PackMode::TIGHT);
  canvaspathbox->Insert(canvaspathlabel, UIBox::PackMode::WIDE);
  canvaspathbox->SetBackColor(Theme::Get("bg-main-alt2"));
  
  canvasbox->Insert(canvaspathbox, UIBox::PackMode::TIGHT);
  canvasbox->Insert(canvasview, UIBox::PackMode::WIDE);

  layered->Insert(canvasbox);
  layered->Insert(selector);

  selector->Populate();

  subscriptions += canvaspathback->on_clicked.Subscribe([this](){
    canvasview->ExitCanvas();
  });
  subscriptions += canvasview->on_canvas_stack_path_changed.Subscribe([this](){
    UpdatePathLabel();
  });

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
  
  subscriptions += allnodes->on_clicked.Subscribe([this](){
    SCLang::QueryAllNodes();
  });
  
  subscriptions += quitbutton->on_clicked.Subscribe([this](){
    ProcessCloseEvent(); // Pretend the window is getting closed
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
  toolbarbox->Insert(toolbar_separator2, UIBox::PackMode::TIGHT);
  toolbar_separator2->SetCustomSize(Size2D(30,0));
  if(Config::Global().debug)
    toolbarbox->Insert(allnodes, UIBox::PackMode::TIGHT);
  toolbarbox->Insert(toolbar_separator3, UIBox::PackMode::WIDE);
  toolbarbox->Insert(quitbutton, UIBox::PackMode::TIGHT);
  mainvbox->Insert(toolbarbox, UIBox::PackMode::TIGHT);
  mainvbox->Insert(layered, UIBox::PackMode::WIDE);
  layered_alert->Insert(mainvbox);
  layered_alert->Insert(centered_alert);
  //centered_alert->Insert(alert);
  centered_alert->SetBackColor(Color(0,0,0,150));
  centered_alert->SetDisplayMode(UIWidget::DisplayMode::Invisible);
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


bool MainWindow::SaveAs(){
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
    return Save(p);
  }
  return false;
}
bool MainWindow::Save(){
  if(current_file_path == ""){
    return SaveAs();
  }else{
    return Save(current_file_path);
  }
}
bool MainWindow::Save(std::string path){
  auto top_canvas = canvasview->GetTopCanvas();
  if(!top_canvas){
    std::cout << "WARNING: Not saving file, because there is no canvas in CV." << std::endl;
    return false;
  }
  try{
    auto canvasxml = CanvasXML::CreateFromCanvas( top_canvas );
    canvasxml->SaveToFile(path);
    current_file_path = path;
    file_name = Utilities::GetFilename(path);
    UpdatePathLabel();
    return true;
  }catch(Exceptions::XMLFileAccess ex){
    ShowErrorAlert("Failed to access file:\n\n" + ex.what(), "Cancel");
    return false;
  }
}


void MainWindow::Open(){

  AskToSaveBeforeCalling([this](){
    
    std::string def = (current_file_path == "") ? Utilities::GetCurrentDir() : Utilities::GetDir(current_file_path);
    nfdchar_t *outPath = nullptr;
    //nfdresult_t result = NFD_OpenDialog( NULL, def.c_str(), &outPath );
    nfdresult_t result = NFD_OpenDialog( NULL, NULL, &outPath );
    std::cout << "After dialog" << std::endl;
    if(result == NFD_OKAY){
      std::string path = outPath;
      // TODO: Block window (progress bar?) while opening file.
      // TODO: Pass a sharedptr instaed of this, to avoid crashes when the window is closed while opening file.
      try{
        auto canvasxml = CanvasXML::CreateFromFile(path);
        canvasxml->CreateNewCanvas(nullptr).Then( [this,path,canvasxml](std::shared_ptr<Canvas> c){
          std::cout << "File opened sucessfuly." << std::endl;
          this->current_file_path = path;
          this->file_name = Utilities::GetFilename(path);
          canvasview->SwitchTopLevelCanvas(c, file_name);
          UpdatePathLabel();
        }).Catch<Exceptions::XMLParse>([this](auto ex){
          this->ShowErrorAlert("Failed to parse file:\n\n" + ex->what(), "Cancel");
        });
      }catch(Exceptions::XMLFileAccess ex){
        ShowErrorAlert("Failed to access file:\n\n" + ex.what(), "Cancel");
      }catch(Exceptions::XMLParse ex){
        ShowErrorAlert("Failed to parse file:\n\n" + ex.what(), "Cancel");
      }
      
      free(outPath);
    } // If result == OKAY
    
  }); // AskToSaveBeforeCalling
}

void MainWindow::AskToSaveBeforeCalling(std::function<void()> f){
  if(canvasview->GetTopCanvas() == nullptr  || (current_file_path == "" && canvasview->GetTopCanvas()->modules.size() == 0)){
    // Empty canvas w/o file. Continue without saving/asking.
    f();
    return;
  }
  // Ask the user about saving current file
  ShowDoYouWantToSaveAlert().Then([this,f](SaveAlertReply reply){
    if(reply == SaveAlertReply::Discard){
      f();
    }else if(reply == SaveAlertReply::Save){
      if(Save()){
        f();
      }else{
        // If file was not saved, do not create new canvas, giving the user a
        // chance to fix their path etc.
      }
    }
  });
}

void MainWindow::New(){
  AskToSaveBeforeCalling([this](){
    Canvas::CreateEmpty(nullptr).Then([this](std::shared_ptr<Canvas> c){
      canvasview->SwitchTopLevelCanvas( c, "Unsaved file");  
    });
    current_file_path = "";
    file_name = "Unsaved file";
    UpdatePathLabel();
  });
}

void MainWindow::ProcessCloseEvent(){
  AskToSaveBeforeCalling([this](){
    Window::ProcessCloseEvent();
  });
}

void MainWindow::ProcessKeyboardEvent(KeyData data){
  // Pass all key events to the canvasview.
  if(data.ctrl && data.type == KeyData::KeyType::Letter && data.symbol == "s" && data.IsTrig()){
    Save(); return;
  }
  if(data.ctrl && data.type == KeyData::KeyType::Letter && data.symbol == "n" && data.IsTrig()){
    New(); return;
  }
  if(data.ctrl && data.type == KeyData::KeyType::Letter && data.symbol == "o" && data.IsTrig()){
    Open(); return;
  }
  if(data.type == KeyData::KeyType::Space && data.IsTrig()){
    if(selector->IsExposed()) selector->Hide();
    else                      selector->Expose();
    return;
  }
  canvasview->OnKeyboard(data);
}

std::shared_ptr<MainWindow> MainWindow::Create(){
  auto res = std::shared_ptr<MainWindow>( new MainWindow());
  res->init();
  return res;
}

LateReturn<int> MainWindow::ShowSimpleAlert(std::string message, std::string button1_text, std::string button2_text, AlertType type, Color button1_color, Color button2_color){
  Relay<int> r;
  if(alert) std::cout << "WARNING: alert removed before it replied, because a new one is issued." << std::endl;
  alert = UIAlert::Create(shared_from_this(),message);
  alert->SetButtons({UIAlert::ButtonData(button1_text, ButtonID::CUSTOM1, button1_color),
                     UIAlert::ButtonData(button2_text, ButtonID::CUSTOM2, button2_color)});
  alert->SetType(type);
  centered_alert->RemoveChild();
  centered_alert->SetDisplayMode(UIWidget::DisplayMode::Visible);
  centered_alert->Insert(alert);
  sub_alert_reply = alert->on_button_pressed.Subscribe([this,r](ButtonID id){
    centered_alert->SetDisplayMode(UIWidget::DisplayMode::Invisible);
    alert = nullptr; // loose reference
    if(id == ButtonID::CUSTOM1) r.Return(0);
    if(id == ButtonID::CUSTOM2) r.Return(1);
  });
  return r;
}
LateReturn<> MainWindow::ShowErrorAlert(std::string message, std::string button_text){
  Relay<> r;
  if(alert) std::cout << "WARNING: alert removed before it replied, because a new one is issued." << std::endl;
  alert = UIAlert::Create(shared_from_this(),message);
  alert->SetButtons({UIAlert::ButtonData(button_text, ButtonID::OK, Theme::Get("bg-button-neutral"))});
  alert->SetType(AlertType::ERROR);
  centered_alert->RemoveChild();
  centered_alert->SetDisplayMode(UIWidget::DisplayMode::Visible);
  centered_alert->Insert(alert);
  sub_alert_reply = alert->on_button_pressed.Subscribe([this,r](ButtonID id){
    centered_alert->SetDisplayMode(UIWidget::DisplayMode::Invisible);
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

LateReturn<MainWindow::SaveAlertReply> MainWindow::ShowDoYouWantToSaveAlert(){
  Relay<SaveAlertReply> r;
  if(alert) std::cout << "WARNING: alert removed before it replied, because a new one is issued." << std::endl;
  alert = UIAlert::Create(shared_from_this(),"WARNING: If you continue, changes to current file will be lost.\nDo you wish to save the file?");
  alert->SetButtons({UIAlert::ButtonData("Cancel", ButtonID::CANCEL, Theme::Get("bg-button-neutral")),
                     UIAlert::ButtonData("Discard changes", ButtonID::NO, Theme::Get("bg-button-negative")),
                     UIAlert::ButtonData("Save file", ButtonID::YES, Theme::Get("bg-button-positive"))}
                   );
  alert->SetType(AlertType::WARNING);
  
  centered_alert->RemoveChild();
  centered_alert->SetDisplayMode(UIWidget::DisplayMode::Visible);
  centered_alert->Insert(alert);
  sub_alert_reply = alert->on_button_pressed.Subscribe([this,r](ButtonID id){
    centered_alert->SetDisplayMode(UIWidget::DisplayMode::Invisible);
    // centered_alert->RemoveChild(); // See ShowErrorAlert
    alert = nullptr; // loose reference
    if(id == ButtonID::CANCEL) r.Return(SaveAlertReply::Cancel);
    if(id == ButtonID::NO    ) r.Return(SaveAlertReply::Discard);
    if(id == ButtonID::YES   ) r.Return(SaveAlertReply::Save);
  });
  
  return r;
}

void MainWindow::UpdatePathLabel(){
  auto vs = canvasview->GetCanvasStackPath();
  if(vs.size() <= 1) canvaspathback->SetDisplayMode(UIWidget::DisplayMode::EmptySpace);
  else               canvaspathback->SetDisplayMode(UIWidget::DisplayMode::Visible);
  std::string path = Utilities::JoinString(vs," -> ");
  canvaspathlabel->SetText("   Currently editting: " + path);
}

} // namespace AlgAudio
