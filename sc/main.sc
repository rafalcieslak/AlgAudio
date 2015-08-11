// This method estabilishes two-way osc connection

OSCdef.new( 'hello', {
		arg msg, time, addr, recvPort;
		[msg, time, addr, recvPort].postln;
		// This is when the ~addr is stored.
		~addr = addr;
		~minstances = Dictionary.new(0);
		~instanceid = 0;
		~buses = Dictionary.new(0);
		"Hello World!".postln;
		~addr.sendMsg("/algaudio/reply",msg[msg.size-1]);
	}, '/algaudioSC/hello'
).postln;


OSCdef.new( 'installtemplate', {
		arg msg;
		var command, f;
		if((msg.size < 2),{
			"Invalid message".postln;
		},{
			("Installing template " ++ msg[1]).postln;
			command = "SynthDef.new('aa/" ++ msg[1] ++ "', {" ++ msg[2] ++ "}).add;";
			//command.postln;
			f = command.compile();
			f.value();
		});
		~addr.sendMsg("/algaudio/reply", "aaaaaa", msg[msg.size-1]);
	}, '/algaudioSC/installtemplate'
).postln;

// A helper method for notifying the app if starting the server succeeded.
OSCdef.new( 'boothelper', {
		arg msg;
		s.options.device = msg[1].asString;
		if((msg[2].asInt == 1),{
			"Server will start in Supernova mode".postln;
			Server.supernova;
		},{
			"Server will start in SCSynth mode".postln;
			Server.scsynth;
		});
		s.waitForBoot({
			~addr.sendMsg("/algaudio/reply", 1, msg[msg.size-1]);
		}, 35, {
			~addr.sendMsg("/algaudio/reply", 0, msg[msg.size-1]);
		});
	}, '/algaudioSC/boothelper'
).postln;

// 1 argument: the template name
// reply value: instance id
OSCdef.new( 'newinstance', {
		arg msg;
		var name = "aa/" ++ msg[1];
		var id = ~instanceid;
		~instanceid = ~instanceid + 1;
		("Creating new \"" ++ name ++ "\" instance " ++ id.asString).postln;
		~minstances.add( id -> Synth.new(name));
		~addr.sendMsg("/algaudio/reply", id, msg[msg.size-1]);
	}, '/algaudioSC/newinstance'
).postln;

// args: the template name + a list of params and values
// reply value: instance id
OSCdef.new( 'newinstanceparams', {
		arg msg;
		var name = "aa/" ++ msg[1];
		var id = ~instanceid;
		var params = msg[2..(msg.size-2)];
		~instanceid = ~instanceid + 1;
		("Creating new \"" ++ name ++ "\" instance " ++ id.asString ++ " with " ++ params.asString ).postln;
		~minstances.add( id -> Synth.new(name, params));
		~addr.sendMsg("/algaudio/reply", id, msg[msg.size-1]);
	}, '/algaudioSC/newinstanceparams'
).postln;

// 1 argument: the template id
// reply value: instance id
OSCdef.new( 'removeinstance', {
		arg msg;
		var id = msg[1];
		("Removing instance \"" ++ id.asString ++ "\".").postln;
		~minstances[id].free;
		~minstances.removeAt( id );
		~addr.sendMsg("/algaudio/reply", msg[msg.size-1]);
	}, '/algaudioSC/removeinstance'
).postln;

// reply value: bus instance id
OSCdef.new( 'newbus', {
		arg msg;
		var newbus = Bus.audio(s,1);
		var id = newbus.index;
		("Creating new bus " ++ id.asString).postln;
		~buses.add( id -> newbus); // TODO: default args
		~addr.sendMsg("/algaudio/reply", id, msg[msg.size-1]);
	}, '/algaudioSC/newbus'
).postln;

// Args: instance id, param name, value
OSCdef.new( 'setparam', {
		arg msg;
		if((msg[1] == -1),{
			("Not setting param " ++ msg[2].asString ++ " of " ++ msg[1].asString ++ " to " ++ msg[3] ++ ", because it's not a valid instance!").postln;
		});
		("Setting param " ++ msg[2].asString ++ " of " ++ msg[1].asString ++ " to " ++ msg[3]).postln;
		~minstances[msg[1]].set(
			msg[2].asString,
			msg[3]
		);
	}, '/algaudioSC/setparam'
).postln;

// Args: instance id, param name, values
OSCdef.new( 'setparamlist', {
		arg msg;
		var list = msg[3..(msg.size-2)];
		if((msg[1] == -1),{
			("Not setting param " ++ msg[2].asString ++ " of " ++ msg[1].asString ++ " to a list, because it's not a valid instance!").postln;
		});
		("Setting param " ++ msg[2].asString ++ " of " ++ msg[1].asString ++ " to " ++ list).postln;
		~minstances[msg[1]].set(
			msg[2].asString,
			list
		);
	}, '/algaudioSC/setparamlist'
).postln;

// Args: instance id, outlet id, bus id
OSCdef.new( 'connectoutlet', {
		arg msg;
		("Connecting outlet " ++ msg[1].asString ++ "/" ++ msg[2].asString ++ " to bus " ++ msg[3]).postln;
		~minstances[msg[1]].set(
			msg[2].asString,
			msg[3]
		);
	}, '/algaudioSC/connectoutlet'
).postln;

// Args: instance id, inlet id, bus id
OSCdef.new( 'connectinlet', {
		arg msg;
		("Connecting inlet " ++ msg[1].asString ++ "/" ++ msg[2].asString ++ " to bus " ++ msg[3]).postln;
		~minstances[msg[1]].set(
			msg[2].asString,
			msg[3]
		);
	}, '/algaudioSC/connectinlet'
).postln;

// This is the helper method that realizes a synth ordering.
OSCdef.new( 'ordering', {
		arg msg;
		var first = msg[1];
		var all = msg[1..(msg.size-2)];
		("Applying ordering: " ++ all.asString).postln;
		~minstances[first].moveToHead(~minstances[first].group);
		for(1,msg.size-3,{ arg i;
			~minstances[all[i]].moveAfter(~minstances[all[i-1]]);
		});
	}, '/algaudioSC/ordering'
).postln;

// Helper catcher for SendReply-ies
OSCdef.new( 'sendreply', {
		arg msg;
		("Got SendReply: " ++ msg.asString).postln;
	}, '/algaudioSC/sendreply'
).postln;

// A dummy helper for listing all installed synthdefs
OSCdef.new( 'listall', {
		SynthDescLib.global.synthDescs.do { |desc|
			if(desc.def.notNil) {
				"\nSynthDef %\n".postf(desc.name.asCompileString);
				//desc.def.func.postcs;
			};
		};
	}, '/algaudioSC/listall'
).postln;
