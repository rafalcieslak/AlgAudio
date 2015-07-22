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
#include "UI/UILayered.hpp"

namespace AlgAudio{

UILayered::UILayered(std::weak_ptr<Window> w) : Widget(w){

}

std::shared_ptr<UILayered> UILayered::Create(std::weak_ptr<Window> w){
  return std::shared_ptr<UILayered>(new UILayered(w));
}

void UILayered::Insert(std::shared_ptr<UIWidget> child, bool visible){

}
void UILayered::SetChildVisible(std::shared_ptr<UIWidget> child, bool visible){

}
virtual void CustomDraw(DrawContext& c){

}
virtual void CustomResize(Size2D size){
  for(auto& chdata : children){
    // All children get our full size
    if(chdata.visible)
      chdata.child->Resize(size);
  }
}
virtual void OnChildRequestedSizeChanged(){

}

} // namespace AlgAudio
