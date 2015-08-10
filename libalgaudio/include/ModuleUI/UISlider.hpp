#ifndef UISLIDER_HPP
#define UISLIDER_HPP
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

namespace AlgAudio{

struct ParramController;

class UISlider : public UIWidget{
public:
  static std::shared_ptr<UISlider> Create(std::weak_ptr<Window> parent_window, std::shared_ptr<ParramController> controller);
  void CustomDraw(DrawContext& c) override;
  virtual bool CustomMousePress(bool down, short b,Point2D pos) override;
  virtual void CustomMouseMotion(Point2D pos1,Point2D pos2) override;
  virtual void CustomMouseEnter(Point2D pos) override;
  virtual void CustomMouseLeave(Point2D pos) override;
  Rect GetInputRect() const;
  Rect GetOutputRect() const;
  Rect GetBodyRect() const;

  void DragStart(Point2D pos);
  void DragStep(Point2D pos);
  void DragEnd(Point2D pos);

  void SetName(std::string name);
  void SetRangeMin(float x){ range_min = x; }
  void SetRangeMax(float x){ range_max = x; }

  std::string id;
protected:
  UISlider(std::weak_ptr<Window> parent_window, std::shared_ptr<ParramController> controller);
private:
  void Init(std::shared_ptr<ParramController> controller);
  std::weak_ptr<ParramController> controller;
  std::shared_ptr<SDLTextTexture> name_texture;

  float current_value;
  std::shared_ptr<SDLTextTexture> value_texture, value_texture_big;

  float range_min, range_max;
  std::shared_ptr<SDLTextTexture> range_min_texture, range_max_texture;

  bool dragged = false;
  Point2D drag_start;
  float drag_start_q;

  enum class PointMode{ None, Left, Center, Right };
  PointMode point_mode = PointMode::None;
};


} // namespace AlgAudio

#endif // UISLIDER_HPP
