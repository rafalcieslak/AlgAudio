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
#include <algorithm>
#include <queue>
#include <sstream>
#include <unordered_set>
#include "rapidxml/rapidxml_print.hpp"
#include "ModuleFactory.hpp"
#include "SCLang.hpp"
#include "ModuleUI/ModuleGUI.hpp"

namespace AlgAudio{

Canvas::Canvas(){

}

std::shared_ptr<Canvas> Canvas::CreateEmpty(){
  return std::shared_ptr<Canvas>( new Canvas() );
}

LateReturn<std::shared_ptr<Module>> Canvas::CreateModule(std::string id){
  Relay<std::shared_ptr<Module>> r;
  ModuleFactory::CreateNewInstance(id).Then([this,r](std::shared_ptr<Module> m){
    modules.emplace(m);
    m->canvas = shared_from_this();
    r.Return(m);
  });
  return r;
}

void Canvas::RemoveModule(std::shared_ptr<Module> m){
  if(!m) std::cout << "WARNING: Canvas asked to remove module (nullptr) " << std::endl;
  // First, remove all audio connections that start at this module.
  for(auto iolettempl : m->templ->outlets){
    std::string outid = iolettempl.id;
    auto it = audio_connections.find(IOID{m, outid});
    if(it != audio_connections.end()){
      GetOutletByIOID(IOID{m, outid})->DetachFromAll();
      audio_connections.erase(it);
    }
  }
  // Next, erase all audio connections that end at this module.
  for(auto iolettempl : m->templ->inlets){
    std::string inid = iolettempl.id;
    for(auto it = audio_connections.begin(); it != audio_connections.end(); /*--*/){
      IOID from = it->first;
      auto from_outlet = GetOutletByIOID(from);
      std::list<IOID>& tolist = it->second;
      for(IOID &to : tolist)
        if(to.module == m && to.iolet == inid)
          from_outlet->DetachFromInlet(GetInletByIOID(to));
      tolist.remove(IOID{m,inid});
      if(tolist.empty()){
        // That was the last connection from that outlet.
        audio_connections.erase(it++);
      }else{
        it++;
      }
    }
  }

  // Then, remove all data connections that start at this module.
  for(auto param : m->templ->params){
    std::string paramid = param->id;
    auto it = data_connections.find(IOID{m, paramid});
    if(it != data_connections.end())
      data_connections.erase(it);
  }
  // Fianlly, remove all data connections that end at this module.
  for(auto param : m->templ->params){
    std::string paramid = param->id;
    for(auto it = data_connections.begin(); it != data_connections.end(); /*--*/){
      std::list<IOIDWithMode>& tolist = it->second;
      tolist.remove(IOIDWithMode{{m,paramid}, DataConnectionMode::Absolute}); // connection mode is ignored in comparison
      if(tolist.empty()){
        // That was the last connection from that outlet.
        // Remove the subscription
        auto it2 = data_connections_subscriptions.find(it->first);
        if(it2 == data_connections_subscriptions.end()){
          std::cout << "WARNING: Unable to remove data_connections subscriptions, as none was found!" << std::endl;
        }else data_connections_subscriptions.erase(it2);
        // Erase the connection
        data_connections.erase(it++);
      }else{
        it++;
      }
    }
  }


  // Then, erase the module and destroy it via ModuleFactory.
  modules.erase(m);
  ModuleFactory::DestroyInstance(m);
}

std::shared_ptr<Module::Inlet>  Canvas::GetInletByIOID(IOID i) const{
  return i.module->GetInletByID(i.iolet);
}
std::shared_ptr<Module::Outlet>  Canvas::GetOutletByIOID(IOID i) const{
  return i.module->GetOutletByID(i.iolet);
}

void Canvas::Connect(IOID from, IOID to){
  std::shared_ptr<Module::Outlet> outlet = GetOutletByIOID(from);
  std::shared_ptr<Module::Inlet >  inlet = GetInletByIOID (to);
  if(!(inlet) || !(outlet)){
    std::cout << "WARNING: Invalid connection between unexisting inlet/outlet." << std::endl;
    return;
  }

  if(TestNewConnectionForLoop(from, to))
    throw ConnectionLoopException("Cannot add the selected connection, adding it would close a loop.");

  auto it = audio_connections.find(from);
  if(it == audio_connections.end()){
    // First connection from this inlet
    std::list<IOID> tmp;
    tmp.push_back(to);
    audio_connections[from] = tmp;
  }else{
    // Not the first connectin from this inlet.
    auto it2 = std::find(it->second.begin(), it->second.end(), to);
    if(it2 != it->second.end()) // if found
      throw DoubleConnectionException("Cannot add the connection, it already exists!");
    else{
      if(audio_connections[from].size() >= 20) throw MultipleConnectionsException("Cannot add another connection to the same outlet, maximum (20) reached.");
      audio_connections[from].push_back(to);
    }
  }

  std::cout << "Connecting" << std::endl;
  outlet->ConnectToInlet(inlet);

  // Correct SC synth order.
  RecalculateOrder();
}

void Canvas::Disconnect(IOID from, IOID to){
  std::shared_ptr<Module::Outlet> outlet = GetOutletByIOID(from);
  std::shared_ptr<Module::Inlet >  inlet = GetInletByIOID (to);
  if(!(inlet) || !(outlet)){
    std::cout << "WARNING: cannot remove between unexisting inlet/outlet." << std::endl;
    return;
  }

  std::cout << "Disonnecting" << std::endl;
  outlet->DetachFromInlet(inlet);

  auto it = audio_connections.find(from);
  if(it == audio_connections.end()) return; // no such connection
  it->second.remove(to);
  if(it->second.size() == 0)
    audio_connections.erase(it);
}


void Canvas::ConnectData(IOID from, IOID to, DataConnectionMode m){
  auto it = data_connections.find(from);
  if(it == data_connections.end()){
    // First connection from this inlet
    std::list<IOIDWithMode> tmp;
    tmp.push_back({to,m});
    data_connections[from] = tmp;
    auto paramctrl = from.module->GetParamControllerByID(from.iolet);
    data_connections_subscriptions[from] = paramctrl->after_set.Subscribe([this, source=from](float val, float val2){
      PassData(source, val, val2);
    });
  }else{
    // Not the first connection from this inlet.
    auto it2 = std::find(it->second.begin(), it->second.end(), IOIDWithMode{to, DataConnectionMode::Relative}); // Note that connection mode is ignored in comparison.
    if(it2 != it->second.end()) // if found
      throw DoubleConnectionException("This connection already exists!");

    it->second.push_back({to,m});
  }
}
void Canvas::DisconnectData(IOID from, IOID to){
  auto it = data_connections.find(from);
  if(it == data_connections.end()) return; // no such connection
  it->second.remove({to, DataConnectionMode::Relative});
  it->second.remove({to, DataConnectionMode::Absolute});
  if(it->second.size() == 0){
    // The last connection from that source was removed. Erase the entry,
    // and cleanup the subscription
    data_connections.erase(it);
    auto it2 = data_connections_subscriptions.find(from);
    if(it2 == data_connections_subscriptions.end()){
      std::cout << "WARNING: Unable to remove data_connections subscriptions, as none was found!" << std::endl;
    }else{
      data_connections_subscriptions.erase(it2);
    }
  }
}

void Canvas::PassData(IOID source, float value, float relative){
  auto it = data_connections.find(source);
  if(it == data_connections.end()){
    std::cout << "WARNING: Pasing data from source, which has no connections anymore..." << std::endl;
    return;
  }
  auto list = it->second;
  for(const IOIDWithMode& iwm : list){
    auto ctrl = iwm.ioid.module->GetParamControllerByID(iwm.ioid.iolet);
    if(iwm.mode == DataConnectionMode::Absolute){
      ctrl->Set(value);
    }else{
      ctrl->SetRelative(relative);
    }
  }
}

bool Canvas::GetDirectAudioConnectionExists(IOID from, IOID to){
  auto it = audio_connections.find(from);
  if(it != audio_connections.end())
    if(std::find(it->second.begin(), it->second.end(), to) != it->second.end())
      return true;
  return false;
}

std::pair<bool, Canvas::DataConnectionMode> Canvas::GetDirectDataConnectionExists(IOID from, IOID to){
  auto it = data_connections.find(from);
  if(it != data_connections.end()){
    auto it2 = std::find(it->second.begin(), it->second.end(), IOIDWithMode{to, DataConnectionMode::Relative}); /* Note that connection mode is ignored in comparison. */
    if(it2 != it->second.end())
      return {true, it2->mode};
  }
  return {false, DataConnectionMode()};
}

void Canvas::RecalculateOrder(){
  // To calculate a correct ordering determining how the modules SC synths
  // should be executed, we do a simple topological sort on the graph of
  // connections. It will be calculated whenever a new connection is added.
  // Note that neither of:
  //    1) creating a new module
  //    2) removing any module
  //    3) removing any connection
  // may change the topological order, so there is no need to recalculate it
  // in such cases.

  // We will need to remember the current (in the sense of the steps of the
  // toposort algorithm) indegree of each verticle (module). We will use a map
  // for this. It may look as a waste of efficiency, because this information
  // might be stored withing a module, but please note this costs us only
  // O(V+E), since map operations are O(1).

  std::map<std::shared_ptr<Module>, int> indegrees;

  // Once we fill the map with initial data for all modules, we will always
  // assume that all .find()s return a valid iterator, i.e. each module always
  // has an entry in the map.

  for(const std::shared_ptr<Module>& m : modules)
    if(m->templ->has_sc_code)
      indegrees[m] = 0;

  // Calculate initial indegrees.
  for(auto &it : audio_connections) // For each connection
    for(const IOID& i : it.second) // For each ending
      indegrees[i.module]++; // Increase the indeg for that module

  std::queue<std::shared_ptr<Module>> frontier;

  // Initialize the frontier
  for(auto &it : indegrees) // Look at all modules...
    if(it.second == 0)        // ... that have 0 indeg, ...
      frontier.push(it.first);  // ... add them.

  std::list<std::shared_ptr<Module>> ordering;

  while(!frontier.empty()){
    // Get a new module from frontier.
    std::shared_ptr<Module> current = frontier.front(); frontier.pop();
    // Add the current module to the resulting ordering.
    ordering.push_back(current);
    std::list<std::shared_ptr<Module>> next_list = GetConnectedModules(current);
    // For each module that is directly connected after the current one:
    for(const std::shared_ptr<Module>& m : next_list)
      if( --indegrees[m]  == 0) // Decrease the indeg, also it it's zero, then add the module to frontier.
        frontier.push(m);
  }

  if(ordering.size() != indegrees.size()){
    // Failure. We did not traverse all modules. This can only happen
    // if the connection graph has a cycle.
    // The ordering is inconclusive.
    throw ConnectionLoopException("");
  }

/*
  // For debugging purposes, demonstrate when the computed odrer is.
  std::cout << "New synth ordering:" << std::endl;
  for(const std::shared_ptr<Module> &m : ordering)
    std::cout << "   \"" << m->templ->name << "\"" << std::endl;
*/

  // Send the ordering to SC.
  lo::Message msg;
  for(const std::shared_ptr<Module> &m : ordering)
    msg.add_int32(m->sc_id);
  SCLang::SendOSCCustom("/algaudioSC/ordering", msg);

}

bool Canvas::TestNewConnectionForLoop(IOID from, IOID to){
  // DFS search starting in TO. If we find FROM, then the new connection would
  // create a loop. If we don't, then the new connection is okay.

  // Usually the connection graph is VERY sparse, multiple connections to
  // an inlet are usually rare. Therefore, when traversing the graph, we should
  // end up visiting only a very small number of vertices (modules). Thus it
  // might be inefficient to prepare a large bitmask and use it to mark whether
  // a module was already visited or not, instead we'll use an unordered set
  // which will gather visited vertices. Access to the set should be O(1) on
  // average.

  if(to.module == from.module) // Loop of size 0
    return true;

  std::unordered_set<std::shared_ptr<Module>> visited;
  std::queue<std::shared_ptr<Module>> frontier;

  // Initialize the frontier with the starting vertex.
  frontier.push(to.module);

  while(!frontier.empty()){
    std::shared_ptr<Module> current = frontier.front(); frontier.pop();
    // Mark as visited.
    visited.insert(current);
    std::list<std::shared_ptr<Module>> next_list = GetConnectedModules(current);
    // For each module that is connected to current:
    for(const std::shared_ptr<Module>& next_module : next_list){
      // If that's the one we are looking for, end the search.
      if(next_module == from.module) return true;
      // If already visited, ignore this vertex.
      if(visited.find(next_module) != visited.end()) continue;
      // Finally, add the new module to frontier.
      frontier.push(next_module);
    }
  }
  return false;
}

std::list<std::shared_ptr<Module>> Canvas::GetConnectedModules(std::shared_ptr<Module> m){
  std::list<std::shared_ptr<Module>> result;
  for(const IOLetTemplate& iolettempl : m->templ->outlets){
    std::string outletid = iolettempl.id;
    // For each outlet, get the list of connections from it
    auto it = audio_connections.find(IOID{m,outletid});
    if(it == audio_connections.end()) continue; // No connections from this outlet.
    for(const IOID& inlet : it->second){
      result.push_back(inlet.module);
    }
  }
  return result;
}


Canvas::~Canvas(){
  std::cout << "NOTE: A canvas instance is destroyed." << std::endl;
  for(auto& m : modules) ModuleFactory::DestroyInstance(m);
}


// ===== Saving to XML file =====

// Helper structure for tracking temporary savefile module identifiers.
struct saveid_state{
  int counter = 0;
  std::map<std::shared_ptr<Module>, int> ids;
};

void XML_AppendModule(rapidxml::xml_node<>* parent, std::shared_ptr<Module> m, saveid_state& s);
void XML_AppendAudioConnection(rapidxml::xml_node<>* parent, Canvas::IOID from, Canvas::IOID to, saveid_state& s);
void XML_AppendDataConnection(rapidxml::xml_node<>* parent, Canvas::IOID from, Canvas::IOIDWithMode to, saveid_state& s);

std::string Canvas::XML_SaveAll() const{
  rapidxml::xml_document<> doc;
  // Prepare temporary save ids map
  saveid_state s;

  // Preparet the root node
  rapidxml::xml_node<>* root = doc.allocate_node(rapidxml::node_type::node_element, "algaudio");
  root->append_attribute( doc.allocate_attribute("version","1") );
  doc.append_node( root );
  // Save all modules
  for(const auto &m : modules) XML_AppendModule(root, m ,s);
  // Save all audio connections
  for(const auto &p : audio_connections){
    IOID from = p.first;
    for(const IOID& to : p.second)
      XML_AppendAudioConnection(root, from, to, s);
  }
  // Save all data connections
  for(const auto &p : data_connections){
    IOID from = p.first;
    for(const IOIDWithMode& to : p.second)
      XML_AppendDataConnection(root, from, to, s);
  }
  std::stringstream ss;
  ss << doc;
  return ss.str();
}

#define allocs(x) d->allocate_string((x).c_str())
#define alloc2s(x) d->allocate_string(std::to_string(x).c_str())

void XML_AppendModule(rapidxml::xml_node<>* parent, std::shared_ptr<Module> m, saveid_state& s){
  auto d = parent->document();
  rapidxml::xml_node<>* modulenode = d->allocate_node(rapidxml::node_type::node_element, "module");
  int id = ++s.counter;
  s.ids[m] = id;
  modulenode->append_attribute( d->allocate_attribute("saveid",alloc2s(id)) );
  modulenode->append_attribute( d->allocate_attribute("template",allocs(m->templ->GetFullID())) );
  // Save paramcontrollers state
  for(std::shared_ptr<ParamController> pc : m->param_controllers){
    rapidxml::xml_node<>* pcnode = d->allocate_node(rapidxml::node_type::node_element, "param");
    pcnode->append_attribute( d->allocate_attribute("id", allocs(pc->id) ));
    pcnode->append_attribute( d->allocate_attribute("value", alloc2s( pc->Get() )));
    modulenode->append_node(pcnode);
  }
  auto gui = m->GetGUI();
  if(gui){
    rapidxml::xml_node<>* guinode = d->allocate_node(rapidxml::node_type::node_element, "gui");
    guinode->append_attribute( d->allocate_attribute("x", alloc2s( gui->position.x )));
    guinode->append_attribute( d->allocate_attribute("y", alloc2s( gui->position.y )));
    modulenode->append_node(guinode);
  }
  parent->append_node(modulenode);
}
void XML_AppendAudioConnection(rapidxml::xml_node<>* parent, Canvas::IOID from, Canvas::IOID to, saveid_state& s){
  auto d = parent->document();
  rapidxml::xml_node<>* connnode = d->allocate_node(rapidxml::node_type::node_element, "audioconn");
  auto it1 = s.ids.find(from.module); auto it2 = s.ids.find(to.module);
  if(it1 == s.ids.end() || it2 == s.ids.end()){
    std::cout << "WARNING: Cannot save an invalid audio connection from/to unknown module, skipping." << std::endl;
    return;
  }
  connnode->append_attribute( d->allocate_attribute("fromsaveid",alloc2s(it1->second)) );
  connnode->append_attribute( d->allocate_attribute(  "tosaveid",alloc2s(it2->second)) );
  connnode->append_attribute( d->allocate_attribute("fromioletid",allocs(from.iolet)) );
  connnode->append_attribute( d->allocate_attribute(  "toioletid",allocs(  to.iolet)) );
  parent->append_node(connnode);
}
void XML_AppendDataConnection(rapidxml::xml_node<>* parent, Canvas::IOID from, Canvas::IOIDWithMode to, saveid_state& s){
  auto d = parent->document();
  rapidxml::xml_node<>* connnode = d->allocate_node(rapidxml::node_type::node_element, "dataconn");
  auto it1 = s.ids.find(from.module); auto it2 = s.ids.find(to.ioid.module);
  if(it1 == s.ids.end() || it2 == s.ids.end()){
    std::cout << "WARNING: Cannot save an invalid audio connection from/to unknown module, skipping." << std::endl;
    return;
  }
  connnode->append_attribute( d->allocate_attribute("fromsaveid",alloc2s(it1->second)) );
  connnode->append_attribute( d->allocate_attribute(  "tosaveid",alloc2s(it2->second)) );
  connnode->append_attribute( d->allocate_attribute("fromparamid",allocs(from.iolet)) );
  connnode->append_attribute( d->allocate_attribute(  "toparamid",allocs(to.ioid.iolet)) );
  if(to.mode == Canvas::DataConnectionMode::Absolute)
    connnode->append_attribute( d->allocate_attribute(  "mode","absolute") );
  else
    connnode->append_attribute( d->allocate_attribute(  "mode","relative") );

  parent->append_node(connnode);
}

// ====  Reading from XML file ==== 

typedef std::map<int, std::shared_ptr<Module>> saveid_map;

LateReturn<> XML_AddModuleFromNode(std::shared_ptr<Canvas>, xml_node<> module_node, std::shared_ptr<saveid_map> map);

LateReturn<std::shared_ptr<Canvas>> Canvas::CreateFromFile(std::string path){
  auto canvas = CreateEmpty();
  
  ifstream f(path);
  if(!f) throw SaveFileException("Unable to read save file.\n" + path);
  
  std::shared_ptr<saveid_map> map;
  
  try{
    std::unique_ptr<xml_document<>> doc = std::make_unique<xml_document<>>();
    rapidxml::file<> file_buffer(file);
    if(file_buffer.size() < 5) throw SaveFileException("The save file is clearly too short");

    doc->parse<0>(file_buffer.data());

    xml_node<>* root = doc->first_node("algaudio");
    if(!root) throw SaveFileException("Missing root node");
    xml_attribute<>* version_attr = root->first_attribute("version");
    if(!version_attr) throw SaveFileException("Missing version information");
    std::string version(version_attr->value());
    // Version check!
    if(version != "1") throw SaveFileException("Invalid file version (" + version + ")");
    
    // Assuming version 1
    int module_count = 0;
    for(xml_node<>* module_node = root->first_node("module"); module_node; module_node = module_node->next_sibling("module"))
      counter++;
      
    Sync s(counter);
    for(xml_node<>* module_node = root->first_node("module"); module_node; module_node = module_node->next_sibling("module"))
        XML_AddModuleFromNode(canvas, module_node, map).ThenSync(s);
    s.WhenAll([this,root](){
      
    });
    
  
  }catch(rapidxml::parse_error ex){
    throw SaveFileException(std::string("XML parse error: ") + ex.what());
  }catch(std::runtime_error ex){
    throw SaveFileException(std::string("XML file error: ") + ex.what());
  }
}

LateReturn<> XML_AddModuleFromNode(std::shared_ptr<Canvas> c, xml_node<> module_node, std::shared_ptr<saveid_map> map){
  Relay<> r;
  xml_attribute<>* saveid_attr = module_node->first_attribute("saveid");
  if(!saveid_attr) throw SaveFileException("A module has missing fileid.");
  int saveid = std::stoi(saveid_attr->value());
  if(saveid <= 0) thdow SaveFileException("A module has invalid fileid.");
  xml_attribute<>* template_attr = module_node->first_attribute("template");
  if(!template_attr) throw SaveFileException("A module has missing template.");
  std::string template_id = template_attr->value();
  
  auto templptr = ModuleCollectionBase::GetTemplateByID(template_id);
  if(!templptr) throw MissingTemplateException(template_id);
  
  ModuleFactory::CreateNewInstance(templptr).Then([this,r,map,saveid](std::shared_ptr<Module> m){
    modules.emplace(m);
    map[m] = saveid;
    m->canvas = shared_from_this();
    r.Return();
  });
  return r;
}

} // namespace AlgAudio
