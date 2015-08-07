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
#include "UI/UISlider.hpp"
#include "Module.hpp"
#include "ModuleTemplate.hpp"
#include "Theme.hpp"
#include "TextRenderer.hpp"

// Just because we need SDL_BUTTON_LEFT. TODO: Create a custom button number enum.
#include <SDL2/SDL.h>

namespace AlgAudio{

UISlider::UISlider(std::weak_ptr<Window> parent_window, std::shared_ptr<ParramController> c) : UIWidget(parent_window), controller(c) {
  SetMinimalSize(Size2D(130,20));
}

std::shared_ptr<UISlider> UISlider::Create(std::weak_ptr<Window> parent_window, std::shared_ptr<ParramController> controller){
  auto res = std::shared_ptr<UISlider>(new UISlider(parent_window, controller));
  res->Init(controller);
  return res;
}

void UISlider::Init(std::shared_ptr<ParramController> controller){
  // Temporary. Tests link to controller.
  if(controller->templ->name == "Frequency"){
    controller->Set(60.0);
  }

  id = controller->templ->id;
  name_texture = TextRenderer::Render(window,FontParrams("FiraMono-Regular",10), controller->templ->name);

  range_min = controller->templ->default_min;
  range_max = controller->templ->default_max;

  range_max_texture = TextRenderer::Render(window,FontParrams("FiraMono-Regular",10), Utilities::PrettyFloat(range_max));
  range_min_texture = TextRenderer::Render(window,FontParrams("FiraMono-Regular",10), Utilities::PrettyFloat(range_min));

  current_value = controller->Get();
  value_texture     = TextRenderer::Render(window,FontParrams("FiraMono-Regular",10), Utilities::PrettyFloat(current_value));
  value_texture_big = TextRenderer::Render(window,FontParrams("FiraMono-Regular",16), Utilities::PrettyFloat(current_value));

  subscriptions += controller->on_set.Subscribe([this](float v){
    current_value = v;
    value_texture     = TextRenderer::Render(window,FontParrams("FiraMono-Regular",10), Utilities::PrettyFloat(current_value));
    value_texture_big = TextRenderer::Render(window,FontParrams("FiraMono-Regular",16), Utilities::PrettyFloat(current_value));
    SetNeedsRedrawing();
  });
}

Rect UISlider::GetInputRect() const{
  return Rect(Point2D(0,0), Size2D(12, current_size.height));
}
Rect UISlider::GetOutputRect() const{
  return Rect(Point2D(current_size.width - 12, 0), Size2D(12, current_size.height));
}
Rect UISlider::GetBodyRect() const{
  return Rect(Point2D(12, 0), Size2D(current_size.width - 24, current_size.height));
}

void UISlider::CustomDraw(DrawContext& c){
  int w = c.Size().width;
  int h = c.Size().height;

  Color bg_color = Theme::Get("slider-bg");
  if(point_mode == PointMode::Center) bg_color = bg_color.Lighter(0.07);
  c.SetColor(bg_color);
  c.DrawRect(0,0,w,h);

  // Left connector body
  if(point_mode == PointMode::Left) c.SetColor(Theme::Get("slider-connector").Lighter(0.1));
  else c.SetColor(Theme::Get("slider-connector"));
  c.DrawRect(0,0,12,h);

  // Right connector body
  if(point_mode == PointMode::Right) c.SetColor(Theme::Get("slider-connector").Lighter(0.1));
  else c.SetColor(Theme::Get("slider-connector"));
  c.DrawRect(w-12,0,12,h);

  if(point_mode != PointMode::Center){
    // NOT pointed on center

    // Slider name
    c.DrawText(name_texture, Color(0,0,0), 13,3);

    auto contr = controller.lock();
    if(contr){
      float v = contr->Get();
      float p = (v - range_min)/(range_max - range_min);
      float pos = std::max(0.0f, std::min(p, 1.0f));
      float x = 12.0 + pos*((float)w - 25.0f);

      c.SetColor(Theme::Get("slider-marker"));
      c.DrawLineEx(x, 0.0f, x, (float)h, 3.0);

      c.Restore(); // Fix the context, because textrendered did tinker with it.
      c.DrawText(value_texture, Color(0,0,0), w - 14 - value_texture->GetSize().width ,3);
    }

  }else{
    // POINTED at center

    auto contr = controller.lock();
    if(contr){
      float v = contr->Get();
      float p = (v - range_min)/(range_max - range_min);
      float pos = std::max(0.0f, std::min(p, 1.0f));
      float x = 12.0 + pos*((float)w - 25.0f);

      c.SetColor(Theme::Get("slider-marker"));
      c.DrawLineEx(x, 0.0f, x, (float)h, 3.0);

      c.Restore(); // Fix the context, because textrendered did tinker with it.
      c.DrawText(value_texture_big, Color(0,0,0), w/2 - value_texture_big->GetSize().width/2, 0);

      c.Restore(); // Fix the context, because textrendered did tinker with it.
      c.DrawText(range_max_texture, Color(0,0,0), w - 14 - range_max_texture->GetSize().width ,3);

      c.Restore(); // Fix the context, because textrendered did tinker with it.
      c.DrawText(range_min_texture, Color(0,0,0), 13 ,3);
    }

  }
  c.SetColor(Theme::Get("slider-border"));
  c.DrawLine(0,0,w,0);
  c.DrawLine(0,0,0,h);
  c.DrawLine(0,h-1,w,h-1);
  c.DrawLine(w-1,0,w-1,h);
  c.DrawLine(12,0,12,h);
  c.DrawLine(w-13,0,w-13,h);

}

bool UISlider::CustomMousePress(bool down, short b,Point2D pos){
  if(pos.IsInside(GetBodyRect()) && down && b == SDL_BUTTON_LEFT){
    float x = pos.x - 12;
    float q = x / (current_size.width - 24);
    float val = range_min + (range_max - range_min) * q;
    controller.lock()->Set(val);
    return true;
  }
  return false;
}

void UISlider::CustomMouseMotion(Point2D, Point2D pos2){
  if(pos2.IsInside(Point2D(0,0), Size2D(12,current_size.height))){
    if(point_mode != PointMode::Left){
      point_mode = PointMode::Left;
      SetNeedsRedrawing();
    }
  }else if(pos2.IsInside(Point2D(current_size.width - 12, 0), Size2D(12, current_size.height))) {
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
  if(pos.IsInside(Point2D(0,0), Size2D(12,current_size.height)))
    point_mode = PointMode::Left;
  else if(pos.IsInside(Point2D(current_size.width - 12, 0), Size2D(12, current_size.height)))
    point_mode = PointMode::Right;
  else
    point_mode = PointMode::Center;
  SetNeedsRedrawing();
}
void UISlider::CustomMouseLeave(Point2D pos){
  point_mode = PointMode::None;
  SetNeedsRedrawing();
}

} // namespace AlgAudio
