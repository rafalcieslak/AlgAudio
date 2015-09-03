#ifndef RAPIDXML_ALGAUDIO_HPP
#define RAPIDXML_ALGAUDIO_HPP
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
#include "rapidxml/rapidxml.hpp"
#include <iostream>

namespace rapidxml{

// Similar to rapidxml::memory_pool, but copies names and values, and therefore
// the result of copying is usable even when the original document no longer
// exists.
template<typename Ch>
void clone_node_copying(const xml_node<Ch> *source, xml_node<Ch> *dest, xml_document<>* target_doc)
{
  // Target document
  auto doc = target_doc;
  if(!doc) std::cout << "WARNING: cloning an XML subtree to a destination with no parent document, crash likely!" << std::endl;
  // Prepare result node
  dest->remove_all_attributes();
  dest->remove_all_nodes();
  dest->type(source->type());

  // Clone name and value, copyin values.
  dest->name(doc->allocate_string(source->name()), source->name_size());
  dest->value(doc->allocate_string(source->value()), source->value_size());

  // Clone child nodes and attributes
  for (xml_node<Ch> *child = source->first_node(); child; child = child->next_sibling()){
    auto newnode = doc->allocate_node(child->type());
    clone_node_copying(child, newnode, doc);
    dest->append_node(newnode);
  }
  for (xml_attribute<Ch> *attr = source->first_attribute(); attr; attr = attr->next_attribute())
    dest->append_attribute(doc->allocate_attribute(doc->allocate_string(attr->name()), doc->allocate_string(attr->value()), attr->name_size(), attr->value_size()));
}
  
} // namespace rapidxml

#endif // RAPIDXML_ALGAUDIO_HPP
