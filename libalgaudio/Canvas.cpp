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
#include <unordered_set>
#include "ModuleFactory.hpp"
#include "ModuleCollection.hpp"
#include "SCLang.hpp"

namespace AlgAudio{

Canvas::Canvas(){

}

std::shared_ptr<Canvas> Canvas::CreateEmpty(){
  return std::shared_ptr<Canvas>( new Canvas() );
}

LateReturn<std::shared_ptr<Module>> Canvas::CreateModule(std::string id){
  Relay<std::shared_ptr<Module>> r;
  ModuleFactory::CreateNewInstance(id).Then([this,r](std::shared_ptr<Module> m){
    if(m){
      modules.emplace(m);
      m->canvas = shared_from_this();
    }
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
    if(!paramctrl) std::cout << "WARNING: pointless connection form unexisting paramcontroller" << std::endl;
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
/*
// ====  Reading from XML file ====

typedef std::map<int, std::shared_ptr<Module>> saveid_map;

/// ###### TODO: Reorganize file loding into a loader class, which will correctly maintain
// all nodes lifetime, as well as document's memory allocation.

LateReturn<std::string> XML_AddModuleFromNode(std::shared_ptr<rapidxml::xml_document<>> doc, std::shared_ptr<Canvas>, rapidxml::xml_node<>* module_node, std::shared_ptr<saveid_map> map);

LateReturn<std::pair<std::shared_ptr<Canvas>,std::string>> Canvas::CreateFromFile(std::string path) noexcept{
  Relay<std::pair<std::shared_ptr<Canvas>,std::string>> r;

  auto canvas = CreateEmpty();

  std::cout << "Inside create" << std::endl;

  std::ifstream f(path);
  if(!f) return r.Return({nullptr,"Unable to read save file.\n" + path});

  std::shared_ptr<saveid_map> map = std::make_shared<saveid_map>();

  std::cout << "Before try" << std::endl;

  try{
    std::shared_ptr<rapidxml::xml_document<>> doc = std::make_shared<rapidxml::xml_document<>>();
    //std::unique_ptr<rapidxml::xml_document<>> doc = std::make_unique<rapidxml::xml_document<>>();
    
    rapidxml::file<> file_buffer(f);
    if(file_buffer.size() < 5) return r.Return({nullptr,"The save file is clearly too short"});

    doc->parse<0>(file_buffer.data());

    rapidxml::xml_node<>* root = doc->first_node("algaudio");
    std::cout << *root << std::endl;
    if(!root) return r.Return({nullptr,"Missing root node"});
    rapidxml::xml_attribute<>* version_attr = root->first_attribute("version");
    if(!version_attr) return r.Return({nullptr,"Missing version information"});
    std::string version(version_attr->value());
    // Version check!
    if(version != "1") return r.Return({nullptr,"Invalid file version (" + version + ")"});

    // Assuming version 1
    int module_count = 0;
    for(rapidxml::xml_node<>* module_node = root->first_node("module"); module_node; module_node = module_node->next_sibling("module"))
      module_count++;

    Sync s(module_count);
    std::shared_ptr<std::string> msg = std::make_shared<std::string>("");
    for(rapidxml::xml_node<>* module_node = root->first_node("module"); module_node; module_node = module_node->next_sibling("module"))
        XML_AddModuleFromNode(doc,canvas, module_node, map).Then([s,msg](std::string errormsg){
          if(errormsg != "" && *msg == "") *msg = errormsg;
          s.Trigger();
        });
    s.WhenAll([r,root,canvas,msg,map](){
      //std::cout << "use count " << doc.use_count() << std::endl;
      //std::cout << *root << std::endl;
      
      if(*msg != ""){ // An error occured with at least one of the modules
        return r.Return({nullptr,*msg});
      }
      
      std::cout << "Modules parsed, now connections." << std::endl;

      // Next, parse audio connections.
      for(rapidxml::xml_node<>* audioconn_node = root->first_node("audioconn"); audioconn_node; audioconn_node = audioconn_node->next_sibling("audioconn")){
        std::cout << "Parsing audio connection." << std::endl;
        
        rapidxml::xml_attribute<>*  fromsaveid_attr = audioconn_node->first_attribute( "fromsaveid");
        rapidxml::xml_attribute<>*    tosaveid_attr = audioconn_node->first_attribute(   "tosaveid");
        rapidxml::xml_attribute<>* fromioletid_attr = audioconn_node->first_attribute("fromioletid");
        rapidxml::xml_attribute<>*   toioletid_attr = audioconn_node->first_attribute(  "toioletid");
        if(!fromioletid_attr || !toioletid_attr || !fromsaveid_attr || !tosaveid_attr)
          return r.Return({nullptr,"Audioconn node is missing one of its attributes"});
        int fromsaveid = std::stoi(fromsaveid_attr->value());
        int tosaveid = std::stoi(tosaveid_attr->value());
        std::string fromioletid = fromioletid_attr->value();
        std::string toioletid = toioletid_attr->value();
        auto from_it = map->find(fromsaveid);
        auto   to_it = map->find(  tosaveid);
        if(from_it == map->end() || to_it == map->end())
          return r.Return({nullptr,"Audioconn has invalid from/to save id"});
        std::shared_ptr<Module> from = from_it->second;
        std::shared_ptr<Module>   to =   to_it->second;

        canvas->Connect({from, fromioletid},{to, toioletid});
      }

      // Then, parse data connections.
      for(rapidxml::xml_node<>* dataconn_node = root->first_node("dataconn"); dataconn_node; dataconn_node = dataconn_node->next_sibling("dataconn")){
        std::cout << "Parsing data connection." << std::endl;
        
        rapidxml::xml_attribute<>*  fromsaveid_attr = dataconn_node->first_attribute( "fromsaveid");
        rapidxml::xml_attribute<>*    tosaveid_attr = dataconn_node->first_attribute(   "tosaveid");
        rapidxml::xml_attribute<>* fromparamid_attr = dataconn_node->first_attribute("fromparamid");
        rapidxml::xml_attribute<>*   toparamid_attr = dataconn_node->first_attribute(  "toparamid");
        rapidxml::xml_attribute<>*        mode_attr = dataconn_node->first_attribute(       "mode");
        if(!fromparamid_attr || !toparamid_attr || !fromsaveid_attr || !tosaveid_attr || !mode_attr)
          return r.Return({nullptr,"Dataconn node is missing one of its attributes"});
        int fromsaveid = std::stoi(fromsaveid_attr->value());
        int tosaveid = std::stoi(tosaveid_attr->value());
        std::string fromparamid = fromparamid_attr->value();
        std::string toparamid = toparamid_attr->value();
        auto from_it = map->find(fromsaveid);
        auto   to_it = map->find(  tosaveid);
        if(from_it == map->end() || to_it == map->end())
          return r.Return({nullptr,"Dataconn has invalid from/to save id"});
        std::shared_ptr<Module> from = from_it->second;
        std::shared_ptr<Module>   to =   to_it->second;
        std::string mode = mode_attr->value();
        Canvas::DataConnectionMode connmode;
        if(mode == "absolute"){
          connmode = Canvas::DataConnectionMode::Absolute;
        }else if(mode == "relative"){
          connmode = Canvas::DataConnectionMode::Relative;
        }else{
          return r.Return({nullptr,"Dataconn has invalid mode value"});
        }

        canvas->ConnectData({from, fromparamid},{to, toparamid},connmode);
      }

      //doc->clear();
      return r.Return({canvas,""});
    });


  }catch(rapidxml::parse_error ex){
    return r.Return({nullptr,std::string("XML parse error: ") + ex.what()});
  }catch(std::runtime_error ex){
    return r.Return({nullptr,std::string("XML file error: ") + ex.what()});
  }
  return r;
}

LateReturn<std::string> XML_AddModuleFromNode(std::shared_ptr<rapidxml::xml_document<>> doc, std::shared_ptr<Canvas> c, rapidxml::xml_node<>* module_node, std::shared_ptr<saveid_map> map){
  Relay<std::string> r;
  rapidxml::xml_attribute<>* saveid_attr = module_node->first_attribute("saveid");
  if(!saveid_attr) return r.Return("A module has missing fileid.");
  int saveid = std::stoi(saveid_attr->value());
  if(saveid <= 0) return r.Return("A module has invalid fileid.");
  rapidxml::xml_attribute<>* template_attr = module_node->first_attribute("template");
  if(!template_attr) return r.Return("A module has missing template.");
  std::string template_id = template_attr->value();

  auto templptr = ModuleCollectionBase::GetTemplateByID(template_id);
  if(!templptr) return r.Return("Missing template: " + template_id + ". This may happen if you lack\none of module collections that were used to create the save file.");

  ModuleFactory::CreateNewInstance(templptr).Then([c,r,map,saveid,module_node,doc](std::shared_ptr<Module> m){
    c->modules.emplace(m);
    map->insert(std::make_pair(saveid,m));
    m->canvas = c;

    // Parse param data.
    for(rapidxml::xml_node<>* param_node = module_node->first_node("param"); param_node; param_node = param_node->next_sibling("param") ){
      rapidxml::xml_attribute<>* id_attr  = param_node->first_attribute("id");
      rapidxml::xml_attribute<>* val_attr = param_node->first_attribute("value");
      if(!id_attr)  return r.Return("A param node is missing id attribute. saveid = " + std::to_string(saveid));
      if(!val_attr) return r.Return("A param node is missing value attribute. saveid = " + std::to_string(saveid));
      std::string paramid = id_attr->value();
      float value = std::stof(val_attr->value());

      auto pc = m->GetParamControllerByID(paramid);
      if(!pc) return r.Return("A param node has invalid id attribute: " + paramid);

      pc->Set(value);
    }

    // Store GUI data.
    rapidxml::xml_node<>* guinode = module_node->first_node("gui");
    if(guinode) rapidxml::print(std::back_inserter(m->guidata), *guinode, rapidxml::print_no_indenting);

    return r.Return("");
  });
  return r;
}
*/

} // namespace AlgAudio
