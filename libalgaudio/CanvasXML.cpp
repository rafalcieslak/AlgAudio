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

#include "CanvasXML.hpp"
#include <cstring>
#include <sstream>
#include "ModuleUI/ModuleGUI.hpp"
#include "ModuleTemplate.hpp"
#include "ModuleCollection.hpp"
#include "ModuleFactory.hpp"

namespace AlgAudio{
  
  
#define allocs(x) doc.allocate_string((x).c_str())
#define alloc2s(x) doc.allocate_string(std::to_string(x).c_str())
  
CanvasXML::CanvasXML(){
  
}

CanvasXML::~CanvasXML(){
  std::cout << "Destroing CanvasXML" << std::endl;
  if(input_buffer) delete[] input_buffer;
}


std::shared_ptr<CanvasXML> CanvasXML::CreateFromFile(std::string path){
  std::ifstream file(path);
  if(!file) throw Exceptions::XMLFileAccess("Unable to open file '" + path + "'");
  std::stringstream ss;
  ss << file.rdbuf();
  return CreateFromString(ss.str());
}

std::shared_ptr<CanvasXML> CanvasXML::CreateFromString(std::string string){
  Utilities::LocaleDecPoint ldp;
  
  auto res = std::shared_ptr<CanvasXML>( new CanvasXML() );
  res->doc_text = string;
  res->doc.clear();
  int len = string.length();
  res->input_buffer = new char[len + 1];
  strncpy(res->input_buffer, string.c_str(), len);
  res->input_buffer[len] = '\0';
  try{
    // Parse the XML into doc
    res->doc.parse< rapidxml::parse_validate_closing_tags >( res->input_buffer );
    
    // Get the root node
    res->root = res->doc.first_node("algaudio");
    if(res->root == nullptr) throw Exceptions::XMLParse("Root node is missing");
    
    // Get version attribute
    rapidxml::xml_attribute<>* version_attr = res->root->first_attribute("version");
    if(!version_attr) throw Exceptions::XMLParse("Version information is missing");
    std::string version(version_attr->value());
    // Version check!
    if(version != "1") throw Exceptions::XMLParse("Invalid file version (" + version + ")");
  
  }catch(rapidxml::parse_error ex){
    throw Exceptions::XMLParse("The XML data is not valid.\nChar: " + std::to_string((int)(ex.where<char>() - res->input_buffer)) + "\n" + ex.what());
  }catch(std::runtime_error ex){
    throw Exceptions::XMLParse("Failed to parse XML data.");
  }
  return res;
}

std::shared_ptr<CanvasXML> CanvasXML::CreateFromNode(rapidxml::xml_node<>* node){
  Utilities::LocaleDecPoint ldp;
  
  if(node == nullptr) throw Exceptions::XMLParse("Root node is missing");
  
  auto res = std::shared_ptr<CanvasXML>( new CanvasXML() );
  res->doc.clear();
  
  res->root = res->doc.allocate_node(rapidxml::node_type::node_element);
  rapidxml::clone_node_copying(node, res->root, &res->doc);
  
  // Get version attribute
  rapidxml::xml_attribute<>* version_attr = res->root->first_attribute("version");
  if(!version_attr) throw Exceptions::XMLParse("Version information is missing");
  std::string version(version_attr->value());
  // Version check!
  if(version != "1") throw Exceptions::XMLParse("Invalid file version (" + version + ")");
  
  return res;
}

std::shared_ptr<CanvasXML> CanvasXML::CreateFromCanvas(std::shared_ptr<Canvas> canvas){
  
  auto res = std::shared_ptr<CanvasXML>( new CanvasXML() );

  // Prepare the root node
  res->root = res->doc.allocate_node(rapidxml::node_type::node_element, "algaudio");
  
  res->root->append_attribute( res->doc.allocate_attribute("version","1") );
  res->doc.append_node( res->root );
  
  res->block_string_updates = true;
  
  // Save all modules
  for(const auto &m : canvas->modules){
    res->AppendModule(m);
  }
  // Save all audio connections
  for(const auto &p : canvas->audio_connections){
    Canvas::IOID from = p.first;
    for(const Canvas::IOID& to : p.second)
      res->AppendAudioConnection(from, to);
  }
  // Save all data connections
  for(const auto &p : canvas->data_connections){
    Canvas::IOID from = p.first;
    for(const Canvas::IOIDWithMode& to : p.second)
      res->AppendDataConnection(from, to);
  }
  
  res->block_string_updates = false;
  res->UpdateStringFromDoc();
  
  // Modules saveid map shall no longer be needed.
  res->modules_to_saveids.clear();
  
  return res;
}

void CanvasXML::UpdateStringFromDoc(){
  if(block_string_updates) return;
  std::stringstream ss;
  ss << doc;
  doc_text = ss.str();
}

void CanvasXML::AppendModule(std::shared_ptr<Module> m){
  Utilities::LocaleDecPoint ldp;
  
  rapidxml::xml_node<>* modulenode = doc.allocate_node(rapidxml::node_type::node_element, "module");
  root->append_node(modulenode);
  
  int id = ++saveid_counter;
  modules_to_saveids[m] = id;
  modulenode->append_attribute( doc.allocate_attribute("saveid",alloc2s(id)) );
  modulenode->append_attribute( doc.allocate_attribute("template",allocs(m->templ->GetFullID())) );
  // Save paramcontrollers state
  for(std::shared_ptr<ParamController> pc : m->param_controllers){
    rapidxml::xml_node<>* pcnode = doc.allocate_node(rapidxml::node_type::node_element, "param");
    pcnode->append_attribute( doc.allocate_attribute("id", allocs(pc->id) ));
    pcnode->append_attribute( doc.allocate_attribute("value", alloc2s( pc->Get() )));
    modulenode->append_node(pcnode);
  }
  // Gui data
  auto gui = m->GetGUI();
  if(gui){
    rapidxml::xml_node<>* guinode = doc.allocate_node(rapidxml::node_type::node_element, "gui");
    guinode->append_attribute( doc.allocate_attribute("x", alloc2s( gui->position().x )));
    guinode->append_attribute( doc.allocate_attribute("y", alloc2s( gui->position().y )));
    modulenode->append_node(guinode);
  }
  // Custom string
  std::string customstring = m->state_store_string();
  if(customstring != ""){
    rapidxml::xml_node<>* customnode = doc.allocate_node(rapidxml::node_type::node_element, "customstring");
    customnode->value(allocs(customstring));
    modulenode->append_node(customnode);
  }
  // Custom xml subtree
  rapidxml::xml_node<>* xmlnode = doc.allocate_node(rapidxml::node_type::node_element, "customxml");
  modulenode->append_node(xmlnode); // This sets the parent document.
  m->state_store_xml(xmlnode);
  auto childnode = xmlnode->first_node();
  auto childattr = xmlnode->first_attribute();
  if(!childnode && !childattr){ // Do not save the node if it has no custom data.
    modulenode->remove_node(xmlnode);
  }
  
  UpdateStringFromDoc();
}
void CanvasXML::AppendAudioConnection(Canvas::IOID from, Canvas::IOID to){
  Utilities::LocaleDecPoint ldp;
  
  rapidxml::xml_node<>* connnode = doc.allocate_node(rapidxml::node_type::node_element, "audioconn");
  auto it1 = modules_to_saveids.find(from.module); auto it2 = modules_to_saveids.find(to.module);
  if(it1 == modules_to_saveids.end() || it2 == modules_to_saveids.end()){
    std::cout << "WARNING: Cannot save an invalid audio connection from/to an unknown module, skipping." << std::endl;
    return;
  }
  connnode->append_attribute( doc.allocate_attribute("frommodule",alloc2s(it1->second)) );
  connnode->append_attribute( doc.allocate_attribute(  "tomodule",alloc2s(it2->second)) );
  connnode->append_attribute( doc.allocate_attribute("fromioletid",allocs(from.iolet)) );
  connnode->append_attribute( doc.allocate_attribute(  "toioletid",allocs(  to.iolet)) );
  root->append_node(connnode);
  
  UpdateStringFromDoc();
}
void CanvasXML::AppendDataConnection(Canvas::IOID from, Canvas::IOIDWithMode to){
  Utilities::LocaleDecPoint ldp;
  
  rapidxml::xml_node<>* connnode = doc.allocate_node(rapidxml::node_type::node_element, "dataconn");
  auto it1 = modules_to_saveids.find(from.module); auto it2 = modules_to_saveids.find(to.ioid.module);
  if(it1 == modules_to_saveids.end() || it2 == modules_to_saveids.end()){
    std::cout << "WARNING: Cannot save an invalid audio connection from/to an unknown module, skipping." << std::endl;
    return;
  }
  connnode->append_attribute( doc.allocate_attribute("frommodule",alloc2s(it1->second)) );
  connnode->append_attribute( doc.allocate_attribute(  "tomodule",alloc2s(it2->second)) );
  connnode->append_attribute( doc.allocate_attribute("fromparamid",allocs(from.iolet)) );
  connnode->append_attribute( doc.allocate_attribute(  "toparamid",allocs(to.ioid.iolet)) );
  if(to.mode == Canvas::DataConnectionMode::Absolute)
    connnode->append_attribute( doc.allocate_attribute(  "mode","absolute") );
  else
    connnode->append_attribute( doc.allocate_attribute(  "mode","relative") );

  root->append_node(connnode);
  
  UpdateStringFromDoc();
}

void CanvasXML::SaveToFile(std::string path){
  std::ofstream file(path);
  if(!file) throw Exceptions::XMLFileAccess("Unable to write to file '" + path + "'.");
  file << doc_text << std::endl;
  file.close();
  std::cout << "File saved." << std::endl;
}
std::string CanvasXML::GetXMLAsString(){
  return doc_text;
}

LateReturn<std::shared_ptr<Canvas>> CanvasXML::CreateNewCanvas(std::shared_ptr<Canvas> parent){
  Relay<std::shared_ptr<Canvas>> r;
  // Captturing a shared pointer to self to extend lifetime.
  Canvas::CreateEmpty(parent).Then([me = shared_from_this(),r](std::shared_ptr<Canvas> res){
    me->ApplyToCanvas(res).ThenReturn(r).Catch(r);
  });
  return r;
}


// Helper macro for exitting with an error.  
#define parseerror(x) {r.LateThrow<Exceptions::XMLParse>(x); return r;}
#define parseerrornr(x) {r.LateThrow<Exceptions::XMLParse>(x); return;}

LateReturn<std::shared_ptr<Canvas>> CanvasXML::ApplyToCanvas(std::shared_ptr<Canvas> c){
  Utilities::LocaleDecPoint ldp;
  
  // Traverse all nodes, add their state to canvas.
  Relay<std::shared_ptr<Canvas>> r;

  // Assuming version 1
  
  saveids_to_modules.clear(); // just to make sure.
  
  int module_count = 0;
  for(rapidxml::xml_node<>* module_node = root->first_node("module"); module_node; module_node = module_node->next_sibling("module"))
    module_count++;

  Sync s(module_count);
  for(rapidxml::xml_node<>* module_node = root->first_node("module"); module_node; module_node = module_node->next_sibling("module"))
      AddModuleFromNode(c, module_node).ThenSync(s).Catch(r);
      
  // Capturing me as shared_ptr to extend lifetime
  s.WhenAll([this,me = shared_from_this(),r,c]()->void{
    
    std::cout << "Modules parsed, now connections." << std::endl;

    // Next, parse audio connections.
    c->BlockReordering(true);
    
    for(rapidxml::xml_node<>* audioconn_node = root->first_node("audioconn"); audioconn_node; audioconn_node = audioconn_node->next_sibling("audioconn")){
      std::cout << "Parsing audio connection." << std::endl;
      
      rapidxml::xml_attribute<>*  fromsaveid_attr = audioconn_node->first_attribute( "frommodule");
      rapidxml::xml_attribute<>*    tosaveid_attr = audioconn_node->first_attribute(   "tomodule");
      rapidxml::xml_attribute<>* fromioletid_attr = audioconn_node->first_attribute("fromioletid");
      rapidxml::xml_attribute<>*   toioletid_attr = audioconn_node->first_attribute(  "toioletid");
      if(!fromioletid_attr || !toioletid_attr || !fromsaveid_attr || !tosaveid_attr)
        parseerrornr("Audioconn node is missing one of its attributes");
      int fromsaveid = std::stoi(fromsaveid_attr->value());
      int tosaveid = std::stoi(tosaveid_attr->value());
      std::string fromioletid = fromioletid_attr->value();
      std::string toioletid = toioletid_attr->value();
      auto from_it = saveids_to_modules.find(fromsaveid);
      auto   to_it = saveids_to_modules.find(  tosaveid);
      if(from_it == saveids_to_modules.end() || to_it == saveids_to_modules.end())
        parseerrornr("Audioconn has invalid from/to save id");
      std::shared_ptr<Module> from = from_it->second;
      std::shared_ptr<Module>   to =   to_it->second;

      c->Connect({from, fromioletid},{to, toioletid});
    }
    
    c->BlockReordering(false);
    
    // Then, parse data connections.
    for(rapidxml::xml_node<>* dataconn_node = root->first_node("dataconn"); dataconn_node; dataconn_node = dataconn_node->next_sibling("dataconn")){
      std::cout << "Parsing data connection." << std::endl;
      
      rapidxml::xml_attribute<>*  fromsaveid_attr = dataconn_node->first_attribute( "frommodule");
      rapidxml::xml_attribute<>*    tosaveid_attr = dataconn_node->first_attribute(   "tomodule");
      rapidxml::xml_attribute<>* fromparamid_attr = dataconn_node->first_attribute("fromparamid");
      rapidxml::xml_attribute<>*   toparamid_attr = dataconn_node->first_attribute(  "toparamid");
      rapidxml::xml_attribute<>*        mode_attr = dataconn_node->first_attribute(       "mode");
      if(!fromparamid_attr || !toparamid_attr || !fromsaveid_attr || !tosaveid_attr || !mode_attr)
        parseerrornr("Dataconn node is missing one of its attributes");
      int fromsaveid = std::stoi(fromsaveid_attr->value());
      int tosaveid = std::stoi(tosaveid_attr->value());
      std::string fromparamid = fromparamid_attr->value();
      std::string toparamid = toparamid_attr->value();
      auto from_it = saveids_to_modules.find(fromsaveid);
      auto   to_it = saveids_to_modules.find(  tosaveid);
      if(from_it == saveids_to_modules.end() || to_it == saveids_to_modules.end())
        parseerrornr("Dataconn has invalid from/to save id");
      std::shared_ptr<Module> from = from_it->second;
      std::shared_ptr<Module>   to =   to_it->second;
      std::string mode = mode_attr->value();
      Canvas::DataConnectionMode connmode;
      if(mode == "absolute"){
        connmode = Canvas::DataConnectionMode::Absolute;
      }else if(mode == "relative"){
        connmode = Canvas::DataConnectionMode::Relative;
      }else{
        parseerrornr("Dataconn has invalid mode value");
      }

      c->ConnectData({from, fromparamid},{to, toparamid},connmode);
    }
    
    r.Return(c);
    
  }); // sync.whenall
  
  return r;
}

LateReturn<> CanvasXML::AddModuleFromNode(std::shared_ptr<Canvas> c, rapidxml::xml_node<>* module_node){
  Utilities::LocaleDecPoint ldp;
  
  Relay<> r;
  rapidxml::xml_attribute<>* saveid_attr = module_node->first_attribute("saveid");
  if(!saveid_attr) parseerror("A module has missing fileid.");
  int saveid = std::stoi(saveid_attr->value());
  if(saveid <= 0) parseerror("A module has invalid fileid.");
  rapidxml::xml_attribute<>* template_attr = module_node->first_attribute("template");
  if(!template_attr) parseerror("A module has missing template.");
  std::string template_id = template_attr->value();

  auto templptr = ModuleCollectionBase::GetTemplateByID(template_id);
  if(!templptr) parseerror("Missing template: " + template_id + ". This may happen if you lack\none of module collections that were used to create the save file.");

  ModuleFactory::CreateNewInstance(templptr, c).Then([this,c,r,saveid,module_node](std::shared_ptr<Module> m) -> void{
    c->modules.emplace(m);
    saveids_to_modules.insert(std::make_pair(saveid,m));
    m->canvas = c;

    // Parse param data.
    for(rapidxml::xml_node<>* param_node = module_node->first_node("param"); param_node; param_node = param_node->next_sibling("param") ){
      rapidxml::xml_attribute<>* id_attr  = param_node->first_attribute("id");
      rapidxml::xml_attribute<>* val_attr = param_node->first_attribute("value");
      if(!id_attr) parseerrornr("A param node is missing id attribute. saveid = " + std::to_string(saveid));
      if(!val_attr) parseerrornr("A param node is missing value attribute. saveid = " + std::to_string(saveid));
      std::string paramid = id_attr->value();
      float value = std::stof(val_attr->value());

      auto pc = m->GetParamControllerByID(paramid);
      if(!pc) parseerrornr("A param node has invalid id attribute: " + paramid);

      pc->Set(value);
    }

    // Read GUI data.
    rapidxml::xml_node<>* guinode = module_node->first_node("gui");
    if(guinode){
      rapidxml::xml_attribute<>* x_attr = guinode->first_attribute("x");
      rapidxml::xml_attribute<>* y_attr = guinode->first_attribute("y");
      if(x_attr && y_attr){
        // Set modulegui position in canvas
        m->position_in_canvas = Point2D( std::stoi(x_attr->value()), std::stoi(y_attr->value()));
      }
    }
    
    // Read custom data.
    rapidxml::xml_node<>* customstringnode = module_node->first_node("customstring");
    if(customstringnode){
      // Pass the value to the module.
      m->state_load_string(customstringnode->value());
    }
    rapidxml::xml_node<>* customxmlnode = module_node->first_node("customxml");
    if(customxmlnode){
      // Pass the subtree to the module.
      m->state_load_xml(customxmlnode);
    }
    r.Return();
  }).Catch<Exceptions::ModuleInstanceCreationFailed>([r](auto ex){
    parseerrornr("Failed to create module instance: " + ex->what());
  });
  return r;
}


} // namespace AlgAudio
