#ifndef CANVAS_HPP
#define CANVAS_HPP
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

#include <memory>
#include <set>
#include "Module.hpp"
#include "Utilities.hpp"

namespace AlgAudio{

struct MultipleConnectionsException : public Exception{
  MultipleConnectionsException(std::string t) : Exception(t) {}
};
struct ConnectionLoopException : public Exception{
  ConnectionLoopException(std::string t) : Exception(t) {}
};
struct DoubleConnectionException : public Exception{
  DoubleConnectionException(std::string t) : Exception(t) {}
};

/* A Canvas represents a set of interconnected modules. The Canvas manages
 * connections between them, calculates topological ordering and detects loops.
 * It also sends control data between modules.
 * The Canvas is strictly an abstract representation.
 * In fututre, it is likely that modules such as subpatch or poly will simply
 * maintain their own instance of a Canvas.
 */
class Canvas : public std::enable_shared_from_this<Canvas>{
public:
  // Creates a new instance of a Canvas with no modules inside.
  static std::shared_ptr<Canvas> CreateEmpty();

  virtual ~Canvas();
  // Creates a new module according to the given template id, and places the
  // new module instance on this Canvas.
  LateReturn<std::shared_ptr<Module>> CreateModule(std::string id);
  // Removes a particular module instance from the Canvas.
  void RemoveModule(std::shared_ptr<Module>);


  // This structure is simply a pair of module pointer and inlet/outlet id.
  // It's simpler to use them then just a reference to the inlet/outlet,
  // because quite often we'll want to search for connections by module.
  struct IOID{
    std::shared_ptr<Module> module;
    std::string iolet;
    bool operator<(const IOID& other) const {return (module==other.module)?(iolet<other.iolet):(module<other.module);}
    bool operator==(const IOID& other) const {return (module==other.module)?(iolet==other.iolet):false;}
  };
  std::shared_ptr<Module::Inlet >  GetInletByIOID(IOID) const;
  std::shared_ptr<Module::Outlet> GetOutletByIOID(IOID) const;
  // These methods are used for creating and removing audio connections between
  // modules.
  void Connect(IOID from, IOID to);
  void Disconnect(IOID from, IOID to);

  enum class DataConnectionMode{
    Relative,
    Absolute
  };
  void ConnectData(IOID from, IOID to, DataConnectionMode m);
  void DisconnectData(IOID from, IOID to);
  // Returns true iff the new connection suggested by method arguments would
  // create a cycle in connections graph.
  bool TestNewConnectionForLoop(IOID from, IOID to);
  // Returns a list of module that have an inlet directly connected to any of
  // the outlets of the module given as argument.
  std::list<std::shared_ptr<Module>> GetConnectedModules(std::shared_ptr<Module> m);
  // Returns true iff the specified connection already exists.
  bool GetDirectAudioConnectionExists(IOID from, IOID to);
  std::pair<bool, DataConnectionMode>  GetDirectDataConnectionExists(IOID from, IOID to);

  // Updates SC synth ordering. Calculates a topological ordering for the graph
  // of interconnections, and sends the result to SC so that it can reorder
  // synths.
  void RecalculateOrder();

  std::string XML_SaveAll() const;

  // The list of all audio connections "from-to", in the format of one-to-many.
  std::map<IOID, std::list<IOID>> audio_connections;

  // The list of all data connections.
  struct IOIDWithMode{
    IOID ioid;
    mutable DataConnectionMode mode;
    bool operator==(const IOIDWithMode& other) const {return ioid == other.ioid; /* Ignore modes for comparison. */}
  };
  std::map<IOID, std::list<IOIDWithMode>> data_connections;
  std::map<IOID, Subscription> data_connections_subscriptions;

  void PassData(IOID source, float value, float relative);
  // The set of all modules that are placed onto (and maintained by) this Canvas.
  std::set<std::shared_ptr<Module>> modules;
private:
  // Private constructor. Use CreateEmpty() instead.
  Canvas();

};

} // namespace AlgAudio

#endif //CANVAS_HPP
