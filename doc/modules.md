Module description specification
===

Version 1 **DRAFT**

This document specifies the format of AlgAudio module collection files and defines the exact meaning of its contents.

The specification is yet subject to change. Some fields are TBD. We restrict right to introduce modifications to this document as long as it is marked as a draft version.

Every module collection consists of two parts:

 - A **module collection description file** which is a plain-text XLM file describing the metadata of all modules. Throughout this document, this file will be sometimes called "the XML file".
 - An optional **module implementation shared library plugin** which is a platform-specific binary executable file and contains a custom implementation of module features. Throughout this document, this file will be sometimes called "the AA file". This file is only necessary it the collection has modules that require custom implementation.

 Usually, both files are provided together. AlgAudio loads all .xml files from the module directory, and the corresponding AA file gets loaded only if it specified within the XML file.

 It is possible and completely normal for a collection to come with no AA file. This is the case if all modules are plain SuperCollider synths. However, if you wish to implement a custom module GUI, interact with an external controller using a third party library or implement custom module logic, you will provide this implementation in an AA file.

## The module collection description file

Module collections description files are plain-text XML files and shall conform to the XML schema. The top-level node of the XML tree shall be unique, and it must be a `collection` node.

Each module collection may define any number of contained modules. Each module is fully independent from others.

Unless explicitly specified differently, each node of given type can only appear once in its parent (the meaning of twin nodes is not defined). Similarly, the value of an attribute that appears more than once in a node is not defined. Also, unless stated differently, the order in which nodes appear is not relevant.

## `collection` node

Represents a single module collection. Must be a top-level node in the collection file.

Attributes:

  - `version` - **required**. Specifies the format the collection conforms to. As of this specification, the value of this attribute must be `1`.
  - `id` - **required**. The identifier of this collection. It is your responsibility to ensure that this name is unique, i.e. AlgAudio will refuse to load two collections with the same `id`. This value can be any text you wish, but:
    1. It may not contain a `/` char.
    2. It is recommended to choose an identified that makes it easy to recognize the collection, as all modules contained within this collection will have their id prefixed with collection id.

Child nodes:

  - `name` - **required**. The value of this field will be presented to the user as the name of this collection.
  - `defaultlib` - *optional*. Used to specify the custom implementation plugin associated with this library. If some of your modules come with a custom implementation which expands module functionality, you can use this node to point AlgAudio to the shared library file (see also: `class` node). The only attribute of this node, `name` (**required**), must be set to the AA file name, but without file extension (it will be added automatically depending on the platform). For example, if your plugin shared library file name is `mycollection.aa.dll`, then the value of the `name` attribute must be `mycollection.aa`.
  - `module` - *optional, may appear more than once*. Defines a single module belonging to this collection. See `module` node for details.

## `module` node

This node represents a single module. It must be a child of a `collection` node, where it may appear multiple times.

Attributes:

  - `id` - **required**. The identifier of this module. It is up to you to make sure the identifiers within a collection are unique. It is not an issue if a module id is the same as the id of a module from a different collection. The id may not contain a `/` char.
  - `name` - **required**. The value of this attribute will be presented to the user as the name of this module, e.g. when browsing available modules, or in the module GUI box - so choose something short but descriptive.

Child nodes:
  - `class` - *optional*. This node can be used to link the module with a custom implementation inside your shared library plugin. See `class` node description for details.
  - `params` - *optional*. This node groups the specification of this module's input and output, both audio and control streams. See `params` node description for details.
  - `description` - *optional*. The value of this node will be used as the description of this module's features. You are welcome to provide here a longer text. Multiple text lines are supported. Tailing and heading whitespace characters will be trimmed.
  - `sc` - *optional*. This node can be used to embed SuperCollider SynthDef source code that shall be associated with your module. See `sc` node description for details.
  - `gui` - *optional. While a module definition is correct without this node, it makes little sense to describe a module without defining its GUI layout.* This node defines the look and layout of this module's user interface which gets displayed on the canvas alongside other modules. See `gui` node for details.

## `class` node

This node specifies which class from the AA file should be used for this module. It must be a child of a module node.

Attributes:

  - `name` - **required**. Specifies the class name. Note that this is not necessarily a class name as seen by the language you use to write the AA file, this is just a string that will be passes to the loader inside your AA file. See AA file information for details.

## `params` node

This node has no attributes and it's purpose is to group the parameter specification for a module. It must be a child of a module node.

Child nodes: **(all are optional and may appear more than once!)**

  - `inlet` - Specifies an audio input of this module. Attributes:

    - `id` - **required**. The inlet identifier. The value of this attribute specifies the SynthDef argument associated with the input bus. For example, if your Synth reads input using `In.ar(inbus)`, then the value of this attribute should be `inbus`.

    AlgAudio will take care to set the SynthDef's argument value when the module is connected to another etc. so you just need to expose these values as SynthDef arguments, specify the `inlet` param, and AlgAudio will manage synth connections automatically.


  - `outlet` - Specifies an audio output of this module. This node is analogous to `inlet` node. Attributes:

    - `id` - **required**. The outlet identifier. Similar to inlet identifier, see above.


  - `param` - Specifies a parameter of the module. All parameters have values represented as IEEE floats. Quite frequently, classic params are associated with UI sliders, but they have a more universal semantics. Attributes:

    - `id` - **required**. The param id. It is significant in two ways:

      1. If the param is linked to a SynthDef argument, then the id must be strictly the same as the argument name.
      2. If your custom implementation wishes to control a parameter on their own, it can access it by asking for a param with a given id.

    - `mode` - *optional, default value: input*. Provides a hint on what is the purpose of this param. This attribute is crucial for automatically building module GUI. Possible values are:

      - `input` - this param is an input value. It will be represented by a slider, and the user will be able to modify this value.

      - `output` - this param is an output value, or a result of some computations. This value will be show to the user, but they will not be able to modify it.

      - `none` - this parram shall be not used externally. It will not be presented to the user.

    - `action` - *optional, default value: `sc` when `mode` is `input`, `none` when `mode` is `custom` or `none`*. This attribute is used to specify what action should be taken when this param's value changes. It should have one of following values:

      - `sc` - Set SuperCollider Synth's (the one's that corresponds to this module) argument, which has the name equal to this param's id, to this param's value. This is the default behavior. This way you can use `param`s to expose SynthDef's arguments to the user.

      - `custom` - Pass the value to module's custom implementation. Setting this param will call `on_param_set` of this module (with the first argument set to the `id` of this param, and the second to the new value). Your custom `Module` implementation can override that method and react on it in a custom way.

      - `none` - Setting this param has no effect (though the value is stored).

    - `defaultval` - *optional*. The float value this param is set when creating a new module.

    - `defaultmin`, `defaultmax` - *optional, default values: `0.0` and `1.0`* - The default range for this parameter.

    - `name` - *optional*. This attributes are only significant when automatically building module GUI. It will be passed to slider configuration. See `gui` node, especially `slider`, for details.

  - `reply` - Specifies a value reply from the synth to AlgAudio. This is useful if your synth performs some kind of signal analysis (e.g. amplitude measuring or pitch detection) and you want to send a result from the synth back to your module in AlgAudio.

  To do so, use SuperCollider's `SendReply`, like this: `SendReply.kr(trigger, '/algaudioSC/sendreply', value_to_send, replyid)`, where:

   - `trigger` is a signal that triggers message sending. Commonly, this is `Imlpulse.kr(10)`. Tune up the impulse frequency for more/less frequent messages.

   - The message path is, as specified, `/algaudioSC/sendreply`. Sending to any other path will skip AlgAudio reply management.

   - `value_to_send` is whatever value you wish to probe and send via this reply.

   - `replyid` is an unused SynthDef argument. It can have whatever name you wish, but it must be equal to `reply` node's `id` attribute. AlgAudio will set it's value automatically.

  This node has following attributes:

    - `id` - The reply id. Must be equal to the SynthDef's argument which is used as the "replyid" parameter of SendReply.

    - `param` - The param id which this reply is linked to. When a reply is received from synth, that param will be set to the received value (and that param's action will be executed).

## `sc` node

This node contains the SynthDef source code. It must be a child of a module node. This node has no children and no attributes.

The source inside sc node will be passed to SuperCollider as a SynthDef template. Each instance of the module will be linked with an instance of such Synth.

If your Synth processes audio data, use SynthDef arguments for input/output bus numbers, and mark them using `inlet` and/or `outlet` params. AlgAudio will bind them to correct buses, so that modules connected together in AlgAudio can pass audio data according to connections.

Example:
```supercollider
<sc>
arg inbus, outbus;
var in = In.ar(inbus);
var out = in * SinOsc.ar(440,0,1);
Out.ar(ourbus, out);
// Remember to add <inlet id="inbus"/> and <outlet id="outbus"/> to <params> node.
</sc>
```

## `gui` node

The gui node defines the layout and look of module's GUI. It must be a child of a module node.

The possible child nodes depend on the GUI type, as set by `type` attribute.

This node has only one attribute:

- `type` - **required**. This attribute specifies the type of the module GUI. It is used to select the general graphical style of the module. Currently, only the two following values are supported:

  - `standard auto` - Classic style. This GUI will be build automatically according using module's param list. In this case the `gui` node shall have no children.

  - `standard` - Classic style. The children of the `gui` node determine the visible UI elements contained in the module gui box. The following children nodes are supported (their relative order of appearance determines the layout order):

    - **TBD**

# The shared library plugin file

The AA file should be a platform-specific, binary shared library file, which means a .dll on Windows, and .so on \*nix. The shared library **must** be compatible with GCC 4.9.x C++ ABI. Seriously, this is important. Any libraries build with MSVC will not work, as they use a completely different C++ ABI than AlgAudio, which was build using GCC 4.9.x.

When building your shared library, remember to link to `libalgaudio` (and possibly it's dependencies, if it's necessary on your platform).

The following ANSI-C symbols must be available in the library:

  - `void* create_instance(const char* name)`

    This function is used by AlgAudio to ask your library for a new instance of your custom module. When the user adds a new module, and the module has a `class` node in the XML file, then instead of simply creating an instance of `AlgAudio::Module`, AlgAudio will call `create_instance` instead, with `name` argument set to the value of the `name` attribute of the `class` node. Your implementation should then return a raw pointer to a new instance `AlgAudio::Module`.

    Because `AlgAudio::Module` is polymorhphic, it is reccommended to define a subclass of `AlgAudio::Module`, override some of it's methods (see below), and have create_instance create a new instance of that custom subclass, and return it as a pointer to the base `AlgAudio::Module`.

    In case of problems, `create_instance` should return a null pointer.

    Returning anything else than a pointer to `AlgAudio::Module` or a null pointer is an error.

    Typical implementation:

    ```c
    extern "C" {
      void* create_instance(const char* name){
        if(strcmp(name,"CustomModuleA")==0) return new CustomModuleA();
        if(strcmp(name,"CustomModuleB")==0) return new CustomModuleB();
        else return nullptr;
      }
    }
    ```

  - `void delete_instance(void* obj)`

    Freeing memory must be done from the same binary module (in the OS sense) as where it was allocated. Thus AlgAudio cannot completely destroy the instance of a Module, your library must give it a hand a bit.

    This function is supposed to free the memory associated with the module passed as `obj`.

    Do not simply `delete obj`, remember to cast the pointer to the correct type, so that all the destructors are called.

    If your custom modules implement no destructors (which should be the case almost always), then the following example implementation will suffice completely:

    ```c
    extern "C" {
      void delete_instance(void* obj){
        delete reinterpret_cast<AlgAudio::Module*>(obj);
      }
    }
    ```

### Creating custom classes

When creating a custom class that publicly inherits from `Module::AlgAudio`, you have a great degree of freedom, as `libalgaudio` exposes 99.9% of AlgAudio's functionality. Thus you can potentially hook up anywhere you want, up to the point where we believe it might be even possible (though insanely difficult) to have a module substitute SuperCollider with a different engine.

The development headers for `libalgaudio` are either available packet together within AlgAudio SDK, or you can browse them at `libalgaudio/include/.`.

However, most modules will simply want to override these several useful methods from `AlgAudio::Module`:

  - `void AlgAudio::Module::on_init()` and
  - `void AlgAudio::Module::on_destroy()`

    These methods are called on module instance creation and deletion. We recommend not implementing a custom constructor and destructor (if possible), and instead relying on these two methods. The reason for this is that these methods are called when the module has been successfully initialized, including creating a SuperCollider Synth, and is ready to work.

  - `void AlgAudio::Module::on_gui_build(std::shared_ptr<AlgAudio::ModuleGUI>)`

    This method is called a bit later than `on_init`, immediately after GUI building for this module instance has been completed. You get a pointer that module GUI as a method argument. This is the right moment to perform GUI post-initialization.

  - `void AlgAudio::Module::on_param_set(std::string, float)`

    This method is called when a param is set, and its `mode` is set to `custom`. Place here the code that reacts on param change. The arguments passed to this method are: param's `id` and the new value.

Browse the header files, especially `Module.hpp` for useful method you can call from your custom module subclass. For example, you may be interested in `AlgAudio::Module::GetParamControllerByID(std::string)`, which returns a pointer to a ParamController, allowing you to set a param value as you wish.
