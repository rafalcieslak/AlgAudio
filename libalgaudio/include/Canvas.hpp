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

namespace Exceptions{
struct MultipleConnections : public Exception{
  MultipleConnections(std::string t) : Exception(t) {}
};
struct ConnectionLoop : public Exception{
  ConnectionLoop(std::string t) : Exception(t) {}
};
struct DoubleConnection : public Exception{
  DoubleConnection(std::string t) : Exception(t) {}
};
} // namespace Exceptions

/** A Canvas represents a collection of interconnected modules. The Canvas
 *  manages connections between them, calculates topological ordering and
 *  detects loops. It also sends control data between modules.
 *  The Canvas is strictly an abstract representation.
 *  Each file consists of a single top-level Canvas. Subpatches maintain their
 *  own instance of a Canvas.
 *
 *  The widget CanvasView is specialised for displaying canvas contents.
 *  \see CanvasView
 */
class Canvas : public std::enable_shared_from_this<Canvas>{
public:
  /** Creates a new instance of a Canvas with no modules inside.
   *  \param parent The parent canvas, or nullptr if this canvas is top-level.
   *  The parent canvas must be known in order for modules like SubpatchExit to
   *  properly detect parent Subpatch module. */
  static LateReturn<std::shared_ptr<Canvas>> CreateEmpty(std::shared_ptr<Canvas> parent);

  virtual ~Canvas();
  
  /** Returns the SC server group corresponding to this canvas. The returned pointer
   *  will never be null. */
  std::shared_ptr<Group> GetGroup() const {return group;}
  
  /** Creates a new module according to the given template id, and places the
   *  new module instance on this Canvas.
   *
   *  This is the proper way of adding new modules.
   */
  LateReturn<std::shared_ptr<Module>> CreateModule(std::string id);
  /** Removes a particular module instance from the Canvas. */
  void RemoveModule(std::shared_ptr<Module>);

  /** The set of all modules that are placed onto (and maintained by) this Canvas. */
  std::set<std::shared_ptr<Module>> modules;

  /** It this canvas is managed by a module, it should set this pointer to itself,
   *  so that everyone else can easlily can tell who owns this module. */
  std::shared_ptr<Module> owner_hint = nullptr;
  
  /** This structure is simply a pair of module pointer and inlet/outlet id.
   *  It's simpler to use them then just a reference to the inlet/outlet,
   *  because quite often we'll want to search for connections by module.
   */
  struct IOID{
    std::shared_ptr<Module> module;
    std::string iolet;
    bool operator<(const IOID& other) const {return (module==other.module)?(iolet<other.iolet):(module<other.module);}
    bool operator==(const IOID& other) const {return (module==other.module)?(iolet==other.iolet):false;}
  };
  /** Searches for an inlet by its IOID. \returns a shared_ptr to that inlet instance, or nullptr if not found. */
  std::shared_ptr<Module::Inlet >  GetInletByIOID(IOID) const;
  /** Searches for an outlet by its IOID. \returns a shared_ptr to that outlet instance, or nullptr if not found. */
  std::shared_ptr<Module::Outlet> GetOutletByIOID(IOID) const;
  
  // === AUDIO CONNECTIONS ====
  
  /** Creates a new audio connection between an outlet and an inlet. */
  void Connect(IOID from, IOID to);
  /** Returns true iff the specified connection already exists. */
  bool GetDirectAudioConnectionExists(IOID from, IOID to);
  /** Removes an audio connection between an outlet and an inlet, if it exists. */
  void Disconnect(IOID from, IOID to);

  /** Returns true iff a new audio connection suggested by method arguments would
   *  create a cycle in connections graph. */
  bool TestNewConnectionForLoop(IOID from, IOID to);
  /** Returns a list of modules that have an inlet directly connected to any of
   *  the outlets of the module given as argument. */
  std::list<std::shared_ptr<Module>> GetConnectedModules(std::shared_ptr<Module> m);

  /**  Updates SC server synth ordering. Calculates a topological ordering for
   *  the graph of interconnections, and sends the result to SC so that it can
   *  reorder synths. */
  void RecalculateOrder();
  
  /*The list of all audio connections "from-to", in the format of one-to-many. */
  std::map<IOID, std::list<IOID>> audio_connections;
  
  // === DATA CONNECTIONS ====
  
  enum class DataConnectionMode{
    /** A relative link between sliders keeps their fractional position the
     *  same. (Setting one param to half it's range will set the other one to
     *  half the other one's range, regardless of the parram value in such
     *  position.) */
    Relative, 
    /** An absolute link between sliders keeps their values the same. (Setting\
     *  one param to zero will set the other to zero too, regardless of zero's
     *  position in param's range. */
    Absolute
  };
    
  /** Creates a new data connection between an outlet and an inlet. */
  void ConnectData(IOID from, IOID to, DataConnectionMode m);
  /** Returns true iff the specified data connection already exists. The
   *  returned pair also provides DataConnectionMode for the queried connection. */
  std::pair<bool, DataConnectionMode>  GetDirectDataConnectionExists(IOID from, IOID to);
  /** Removes a data connection between an outlet and an inlet, if it exists. */
  void DisconnectData(IOID from, IOID to);
  
  struct IOIDWithMode{
    IOID ioid;
    mutable DataConnectionMode mode;
    bool operator==(const IOIDWithMode& other) const {return ioid == other.ioid; /* Ignore modes for comparison. */}
  };
  /** The list of all data connections. */
  std::map<IOID, std::list<IOIDWithMode>> data_connections;

private:
  /** Private constructor. Use CreateEmpty() instead. */
  Canvas();

  /** The server node group that encapsulates all contents of this canvas. */
  std::shared_ptr<Group> group;
  
  /** The parent canvas. */
  std::weak_ptr<Canvas> parent;
  
  /** The subscriptions for reacting on param value change. */
  std::map<IOID, Subscription> data_connections_subscriptions;
  /** The function that provides the reaction on param value changes,
   *  effectively realizing data connections. */
  void PassData(IOID source, float value, float relative);
};

} // namespace AlgAudio

#endif //CANVAS_HPP
