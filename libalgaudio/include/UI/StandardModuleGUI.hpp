#ifndef STANDARDMODULEGUI_HPP
#define STANDARDMODULEGUI_HPP
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
#include "UI/UIWidget.hpp"
#include "Module.hpp"
#include "ModuleGUI.hpp"
#include "UI/UIMarginBox.hpp"
#include "UIBox.hpp"
#include "UI/UILabel.hpp"

namespace AlgAudio{

class StandardModuleGUI : public ModuleGUI{
public:
  static std::shared_ptr<StandardModuleGUI> CreateEmpty(std::shared_ptr<Window> w);
  static std::shared_ptr<StandardModuleGUI> CreateFromXML(std::shared_ptr<Window> w, std::string xml_data, std::shared_ptr<ModuleTemplate> templ);
  static std::shared_ptr<StandardModuleGUI> CreateFromTemplate(std::shared_ptr<Window> w, std::shared_ptr<ModuleTemplate> templ);
  void CustomDraw(DrawContext& c) override;
  void CustomResize(Size2D s) override;
  void SetHighlight(bool) override;
  void OnChildRequestedSizeChanged() override;
  void OnChildVisibilityChanged() override;

protected:
  StandardModuleGUI(std::shared_ptr<Window> w) : ModuleGUI(w){}
private:
  void LoadFromXML(std::string xml_data, std::shared_ptr<ModuleTemplate> templ);
  void LoadFromTemplate(std::shared_ptr<ModuleTemplate> templ);
  void UpdateMinimalSize();
  void CommonInit();
  bool highlight = false;
  std::shared_ptr<UILabel> caption;

  std::shared_ptr<UIMarginBox> main_margin;
  std::shared_ptr<UIVBox> main_box;
  std::shared_ptr<UIHBox> inlets_box;
  std::shared_ptr<UIHBox> outlets_box;

  class IOConn : public UIWidget{
  public:
    std::string id;
    VertAlignment align;
    Color main_color;
    Color border_color;
    static std::shared_ptr<IOConn> Create(std::weak_ptr<Window> w, std::string id, VertAlignment align, Color c);
    void CustomDraw(DrawContext& c) override;
    void SetBorderColor(Color c);
  private:
    void Init();
    IOConn(std::weak_ptr<Window> w, std::string id_, VertAlignment align_, Color c);
  };

  std::vector<std::shared_ptr<IOConn>> inlets;
  std::vector<std::shared_ptr<IOConn>> outlets;
};

} // namespace AlgAudio

#endif //STANDARDMODULEGUI_HPP
