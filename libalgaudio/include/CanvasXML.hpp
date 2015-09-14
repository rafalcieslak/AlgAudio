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
  
namespace Exceptions{
struct XMLFileAccess : public Exception{
  XMLFileAccess(std::string t) : Exception(t) {}
};  
struct XMLParse : public Exception{
  XMLParse(std::string t) : Exception(t) {}
};  
} // namespace Exceptions
  
/** An instanceable class that exports canvas state to an XML document, and
 *  vice versa. A CanvasXML maintains an XML document, which can be loaded from
 *  a file or created according to canvas state, as well as saved to file or
 *  applied to a Canvas.
 *
 *  To open a file to a new Canvas, you would usually cretate a CanvasXML with
 *  CreateFromFile(), and then use CreateNewCanvas() member function.
 *
 *  To save a Canvas to file, you should create a CanvasXML with
 *  CreateFromCanvas, and then use SaveToFile() member function.
 */
class CanvasXML : public std::enable_shared_from_this<CanvasXML>{
public:
  /** Opens the file at the given path, reads the contents, parses the XML
   *  document, and creates a new CanvasXML using that data. \param path The
   *  path to file to load. */
  static std::shared_ptr<CanvasXML> CreateFromFile(std::string path);
  /** Parses the given string as an XML document, and creates a new CanvasXML
   *  which uses that data. */
  static std::shared_ptr<CanvasXML> CreateFromString(std::string string);
  /** Uses the given rapidxml XML node to create a new CanvasXML. The node data
   *  will be fully cloned, so it's safe to delete the node afterwards. */
  static std::shared_ptr<CanvasXML> CreateFromNode(rapidxml::xml_node<>* node);
  /** Creates a new CanvasXML which has data corresponding to the given canvas'
   *  current state. Modifying the Canvas afterwards will not result in any
   *  changes in the CanvasXML. */
  static std::shared_ptr<CanvasXML> CreateFromCanvas(std::shared_ptr<Canvas> canvas);
  
  /** Stores the XML document in a file. \param path The path to file the XML
   *  document shall be saved to. */
  void SaveToFile(std::string path);
  /** Returns the stored XML document as a string. */
  std::string GetXMLAsString();
  
  /** Puts the stored XML document as a subtree of some other rapidxml document.
   *  All data is fully cloned, so it's safe to destruct this CanvasXML instance
   *  and continue using the parent xml document.
   *   \param node The subtree node the stored document shall be placed at.
   *   \param doc  The xml_document the node is in.
   */
  void CloneToAnotherXMLTree(rapidxml::xml_node<>* node, rapidxml::xml_document<>* doc){
    rapidxml::clone_node_copying(root, node, doc);
  }
  
  /** Applies the data in stored document to a given Canvas, creating new
   *  modules, setting params, adding connections etc. On success, latereturns
   *  the same canvas pointer. Never returns a nullptr. May latethrow
   *  Exceptions::XMLParse.
   *  \warning ApplyToCanvas() is strictly NOT late-reentrant! (it shall not be
   *  invoked again before the previous call latereturns). */
  LateReturn<std::shared_ptr<Canvas>> ApplyToCanvas(std::shared_ptr<Canvas> c);
  
  /** Creates a new canvas basing on the stored document. Never returns a
   *  nullptr. May latethrow Exceptions::XMLParse.
   *  \warning CreateNewCanvas() is strictly NOT late-reentrant! (it shall not
   *  be invoked again before the previous call latereturns) */
  LateReturn<std::shared_ptr<Canvas>> CreateNewCanvas(std::shared_ptr<Canvas> parent);
  
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
  
  /** Helper for creating canvas from document. May latethrow Exceptions::XMLParse in case of problems. */
  LateReturn<> AddModuleFromNode(std::shared_ptr<Canvas> c, rapidxml::xml_node<>* module_node);
  
  /** Exports the contents of the doc to doc_text. */
  void UpdateStringFromDoc();
  /** Set this flag to true if you are going to call Append* multiple times, to
   * avoid unnvecessary exports to doc_text after each append. */
  bool block_string_updates = false;
};
  
} // namespace AlgAudio

#endif // CANVASXML_HPP
