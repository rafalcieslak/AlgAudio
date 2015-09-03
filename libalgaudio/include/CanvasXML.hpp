#ifndef CANVASXML_HPP
#define CANVASXML_HPP
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

#include "Canvas.hpp"
#include "rapidxml/rapidxml_print.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include "rapidxml_algaudio.hpp"

namespace AlgAudio{
  
  
struct XMLFileAccessException : public Exception{
  XMLFileAccessException(std::string t) : Exception(t) {}
};  
struct XMLParseException : public Exception{
  XMLParseException(std::string t) : Exception(t) {}
};  
  
/* An instanceable class that exports canvas state to an XML document, and
 * vice versa.
 */
class CanvasXML{
public:
  static std::shared_ptr<CanvasXML> CreateFromFile(std::string path);
  static std::shared_ptr<CanvasXML> CreateFromString(std::string string);
  static std::shared_ptr<CanvasXML> CreateFromNode(rapidxml::xml_node<>* node);
  static std::shared_ptr<CanvasXML> CreateFromCanvas(std::shared_ptr<Canvas> canvas);
  
  void SaveToFile(std::string path);
  std::string GetXMLAsString();
  
  void CloneToAnotherXMLTree(rapidxml::xml_node<>* node, rapidxml::xml_document<>* doc){
    rapidxml::clone_node_copying(root, node, doc);
  }
  
  // On success, latereturns the same canvas pointer. Otherwise it returns a
  // null pointer, and stores an error message to be read using GetLastError.
  // WARNING: ApplyToCanvas is strictly NOT late-reentrant! (it shall not be
  // invoked again before the previous call latereturns)
  LateReturn<std::shared_ptr<Canvas>> ApplyToCanvas(std::shared_ptr<Canvas> c);
  
  // Creates a new canvas basing on the stored document.
  // WARNING: CreateNewCanvas is strictly NOT late-reentrant! (it shall not be
  // invoked again before the previous call latereturns)
  LateReturn<std::shared_ptr<Canvas>> CreateNewCanvas();
  
  // Since CreateNewCanvas is a lateReturn, it cannot throw exceptions.
  // Thus if it fails, it returns a nullptr, and the error cause can be
  // fetched using GetLastError();
  std::string GetLastError(){ return last_createcanvas_error;}
  
  ~CanvasXML();
private:
  CanvasXML();
  std::string doc_text;
  char* input_buffer = nullptr;
  rapidxml::xml_document<> doc;
  rapidxml::xml_node<>* root;
  
  // Used temporarily when creating a document from canvas.
  std::map<std::shared_ptr<Module>, int> modules_to_saveids;
  int saveid_counter = 0;
  // Used temporarily when creating a canvas from document.
  std::map<int, std::shared_ptr<Module>> saveids_to_modules;
  
  // Adds data to the xml document.
  void AppendModule(std::shared_ptr<Module>);
  void AppendAudioConnection(Canvas::IOID from, Canvas::IOID to);
  void AppendDataConnection(Canvas::IOID from, Canvas::IOIDWithMode to);
  
  // Helper for creating canvas from document. Returns error message, or empty
  // string on success.
  LateReturn<std::string> AddModuleFromNode(std::shared_ptr<Canvas> c, rapidxml::xml_node<>* module_node);
  
  // Exports the contents of the doc to doc_text.
  void UpdateStringFromDoc();
  // Set this flag to true if you are going to call Append* multiple times, to
  // avoid unnvecessary exports to doc_text after each append.
  bool block_string_updates = false;
  
  std::string last_createcanvas_error;
};
  
} // namespace AlgAudio

#endif // CANVASXML_HPP
