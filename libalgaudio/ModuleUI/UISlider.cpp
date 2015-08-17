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
#include "ModuleUI/UISlider.hpp"
#include "Module.hpp"
#include "ModuleTemplate.hpp"
#include "Theme.hpp"
#include "TextRenderer.hpp"

namespace AlgAudio{

UISlider::UISlider(std::weak_ptr<Window> parent_window, std::shared_ptr<ParamController> c) : UIWidget(parent_window), controller(c) {
  SetMinimalSize(Size2D(130,20));
}

std::shared_ptr<UISlider> UISlider::Create(std::weak_ptr<Window> parent_window, std::shared_ptr<ParamController> controller){
  auto res = std::shared_ptr<UISlider>(new UISlider(parent_window, controller));
  res->Init(controller);
  return res;
}

void UISlider::Init(std::shared_ptr<ParamController> controller){
  param_id = controller->templ->id;
  name_texture = TextRenderer::Render(window,FontParams("FiraMono-Regular",10), controller->templ->name);

  current_range_min = controller->GetRangeMin();
  current_range_max = controller->GetRangeMax();

  range_min_texture = TextRenderer::Render(window,FontParams("FiraMono-Regular",10), Utilities::PrettyFloat(current_range_min));
  range_max_texture = TextRenderer::Render(window,FontParams("FiraMono-Regular",10), Utilities::PrettyFloat(current_range_max));

  current_value = controller->Get();
  value_texture     = TextRenderer::Render(window,FontParams("FiraMono-Regular",10), Utilities::PrettyFloat(current_value));
  value_texture_big = TextRenderer::Render(window,FontParams("FiraMono-Regular",16), Utilities::PrettyFloat(current_value));

  subscriptions += controller->on_set.Subscribe([this](float v, float){
    current_value = v;
    value_texture     = TextRenderer::Render(window,FontParams("FiraMono-Regular",10), Utilities::PrettyFloat(current_value));
    value_texture_big = TextRenderer::Render(window,FontParams("FiraMono-Regular",16), Utilities::PrettyFloat(current_value));
    SetNeedsRedrawing();
  });
}

void UISlider::SetName(std::string name){
  name_texture = TextRenderer::Render(window,FontParams("FiraMono-Regular",10), name);
  SetNeedsRedrawing();
}

void UISlider::SetRangeMin(float x){
  current_range_min = x;
  range_min_texture = TextRenderer::Render(window,FontParams("FiraMono-Regular",10), Utilities::PrettyFloat(current_range_min));
}
void UISlider::SetRangeMax(float x){
  current_range_max = x;
  range_max_texture = TextRenderer::Render(window,FontParams("FiraMono-Regular",10), Utilities::PrettyFloat(current_range_max));
}

Rect UISlider::GetInputRect() const{
  return (mode == Mode::Display) ?
    Rect(Point2D(0,0), Size2D(0,0)) :
    Rect(Point2D(0,0), Size2D(12, current_size.height));
}
Rect UISlider::GetOutputRect() const{
  return Rect(Point2D(current_size.width - 12, 0), Size2D(12, current_size.height));
}
Rect UISlider::GetBodyRect() const{
  return (mode == Mode::Display) ?
    Rect(Point2D(0 , 0), Size2D(current_size.width - 12, current_size.height)) :
    Rect(Point2D(12, 0), Size2D(current_size.width - 24, current_size.height));
}

void UISlider::CustomDraw(DrawContext& c){
  int w = c.Size().width;
  int h = c.Size().height;

  Color bg_color;
  if(mode == Mode::Slider) bg_color = Theme::Get("slider-bg");
  else if(mode == Mode::Display) bg_color = Theme::Get("slider-bg-display");

  if( mode == Mode::Slider && (point_mode == PointMode::Center || dragged)) bg_color = bg_color.Lighter(0.07);
  c.SetColor(bg_color);
  c.DrawRect(0,0,w,h);

  if(mode == Mode::Slider){
    // Left connector body
    if(point_mode == PointMode::Left) c.SetColor(Theme::Get("slider-connector").Lighter(0.1));
    else c.SetColor(Theme::Get("slider-connector"));
    c.DrawRect(0,0,12,h);
  }

  // Right connector body
  if(point_mode == PointMode::Right) c.SetColor(Theme::Get("slider-connector").Lighter(0.1));
  else c.SetColor(Theme::Get("slider-connector"));
  c.DrawRect(w-12,0,12,h);

  if(point_mode != PointMode::Center && !dragged){
    // NOT pointed on center

    // Slider name
    c.DrawText(name_texture, Color(0,0,0), (mode == Mode::Display)?1:13,3);

    auto contr = controller.lock();
    if(contr){
      float p = contr->GetRelative();
      float pos = std::max(0.0f, std::min(p, 1.0f));
      float x = GetBodyStart() + pos*(GetBodyWidth());

      c.SetColor(Theme::Get("slider-marker"));
      c.DrawLineEx(x, 0.0f, x, (float)h, 3.0);

      c.Restore(); // Fix the context, because textrendered did tinker with it.
      c.DrawText(value_texture, Color(0,0,0), GetBodyEnd() - 1 - value_texture->GetSize().width ,3);
    }

  }else{
    // POINTED at center

    auto contr = controller.lock();
    if(contr){
      float q = (current_value - current_range_min)/(current_range_max - current_range_min);
      float pos = std::max(0.0f, std::min(q, 1.0f));
      float x = GetBodyStart() + pos*(GetBodyWidth());

      c.SetColor(Theme::Get("slider-marker"));
      c.DrawLineEx(x, 0.0f, x, (float)h, 3.0);

      c.Restore(); // Fix the context, because textrendered did tinker with it.
      c.DrawText(value_texture_big, Color(0,0,0), w/2 - value_texture_big->GetSize().width/2, 0);

      c.Restore(); // Fix the context, because textrendered did tinker with it.
      c.DrawText(range_max_texture, Color(0,0,0), GetBodyEnd() - 1 - range_max_texture->GetSize().width ,3);

      c.Restore(); // Fix the context, because textrendered did tinker with it.
      c.DrawText(range_min_texture, Color(0,0,0), GetBodyStart() + 1 ,3);
    }

  }
  c.SetColor(Theme::Get("slider-border"));
  c.DrawLine(0,0,w,0);
  c.DrawLine(0,0,0,h);
  c.DrawLine(0,h-1,w,h-1);
  c.DrawLine(w-1,0,w-1,h);
  if(mode == Mode::Slider) c.DrawLine(12,0,12,h);
  c.DrawLine(w-13,0,w-13,h);

}

bool UISlider::CustomMousePress(bool down, MouseButton b,Point2D pos){
  if(pos.IsInside(GetBodyRect()) && down && b == MouseButton::Left && mode == Mode::Slider){
    float x = pos.x - GetBodyStart();
    float q = x / GetBodyWidth();
    controller.lock()->SetRelative(q);
    return true;
  }
  return false;
}

void UISlider::CustomMouseMotion(Point2D, Point2D pos2){
  if(pos2.IsInside(GetInputRect())){
    if(point_mode != PointMode::Left){
      point_mode = PointMode::Left;
      SetNeedsRedrawing();
    }
  }else if(pos2.IsInside(GetOutputRect())) {
    if(point_mode != PointMode::Right){
      point_mode = PointMode::Right;
      SetNeedsRedrawing();
    }
  }else{
    if(point_mode != PointMode::Center){
      point_mode = PointMode::Center;
      SetNeedsRedrawing();
    }
  }
}
void UISlider::CustomMouseEnter(Point2D pos){
  if(pos.IsInside(GetInputRect()))
    point_mode = PointMode::Left;
  else if(pos.IsInside(GetOutputRect()))
    point_mode = PointMode::Right;
  else
    point_mode = PointMode::Center;
  SetNeedsRedrawing();
}
void UISlider::CustomMouseLeave(Point2D){
  point_mode = PointMode::None;
  SetNeedsRedrawing();
}

void UISlider::DragStart(){
  dragged = true;
  drag_start_q = (current_value - current_range_min)/(current_range_max - current_range_min);
  SetNeedsRedrawing();
}
void UISlider::DragStep(Point2D_<float> offset){
  if(mode == Mode::Display) return;

  float dq = (offset.x)/((float)GetBodyWidth());
  float q = std::max(0.0f, std::min(1.0f, drag_start_q + dq));
  controller.lock()->SetRelative(q);
}
void UISlider::DragEnd(){
  dragged = false;
  SetNeedsRedrawing();
}

} // namespace AlgAudio
