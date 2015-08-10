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
#include "UI/UIBox.hpp"
#include "UI/UILabel.hpp"
#include "ModuleUI/UISlider.hpp"

namespace AlgAudio{

class StandardModuleGUI : public ModuleGUI{
public:
  static std::shared_ptr<StandardModuleGUI> CreateFromXML(std::shared_ptr<Window> w, std::string xml_data, std::shared_ptr<Module> mod);
  static std::shared_ptr<StandardModuleGUI> CreateFromTemplate(std::shared_ptr<Window> w, std::shared_ptr<Module> mod);
  void CustomDraw(DrawContext& c) override;
  void CustomResize(Size2D s) override;
  void SetHighlight(bool) override;
  void OnChildRequestedSizeChanged() override;
  void OnChildVisibilityChanged() override;
  virtual bool CustomMousePress(bool down, short b,Point2D pos) override {return main_margin->OnMousePress(down,b,pos);}
  virtual void CustomMouseMotion(Point2D pos1,Point2D pos2) override {main_margin->OnMouseMotion(pos1,pos2);}
  virtual void CustomMouseEnter(Point2D pos) override {main_margin->OnMouseEnter(pos);}
  virtual void CustomMouseLeave(Point2D pos) override {main_margin->OnMouseLeave(pos);}
  virtual Point2D WhereIsInletByWidgetID(UIWidget::ID inlet) override;
  virtual Point2D WhereIsOutletByWidgetID(UIWidget::ID outlet) override;
  virtual Point2D WhereIsInletByParramID(std::string inlet) override;
  virtual Point2D WhereIsOutletByParramID(std::string outlet) override;
  virtual WhatIsHere GetWhatIsHere(Point2D) const override;
  virtual void SliderDragStart(UIWidget::ID id, Point2D start_pos) override;
  virtual void SliderDragStep(UIWidget::ID id, Point2D current_pos) override;
  virtual void SliderDragEnd(UIWidget::ID id, Point2D final_pos) override;
  virtual std::string GetIoletParramID(UIWidget::ID) const override;
  virtual std::shared_ptr<UIWidget> CustomFindChild(ID id) const override{ return main_margin->FindChild(id);}
protected:
  StandardModuleGUI(std::shared_ptr<Window> w, std::shared_ptr<Module> mod) : ModuleGUI(w, mod){}
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
  std::shared_ptr<UIVBox> parrams_box;

  std::shared_ptr<SDLTextTexture> id_texture = nullptr;

  class IOConn : public UIWidget{
  public:
    std::string iolet_id;
    VertAlignment align;
    Color main_color;
    Color border_color;
    Signal<bool> on_press;
    // This flag is set iff the mouse pointer is not only pointing at this
    // widget, but also is inside the inlet/outlet rect.
    bool inside = false;
    static std::shared_ptr<IOConn> Create(std::weak_ptr<Window> w, std::string id, VertAlignment align, Color c);
    void CustomDraw(DrawContext& c) override;
    void SetBorderColor(Color c);
    virtual bool CustomMousePress(bool down, short b,Point2D pos) override;
    friend class StandardModuleGUI;
    static const int width, height;
  private:
    void Init();
    IOConn(std::weak_ptr<Window> w, std::string id_, VertAlignment align_, Color c);
    Point2D GetRectPos() const;
    inline Size2D  GetRectSize() const {return Size2D(width,height);}
    Point2D GetCenterPos() const;
  };

  // Here all crucial elements are stored.
  std::map<UIWidget::ID, std::shared_ptr<IOConn>> inlets;
  std::map<UIWidget::ID, std::shared_ptr<IOConn>> outlets;
  std::map<UIWidget::ID, std::shared_ptr<UISlider>> parram_sliders;

  // Rectangle cache for WhatIsHere method
  void UpdateWhatIsHereCache();
  std::list<std::pair<Rect, WhatIsHere>> rect_cache;

};

} // namespace AlgAudio

#endif //STANDARDMODULEGUI_HPP
