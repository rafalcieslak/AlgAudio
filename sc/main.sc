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
		s.waitForBoot({
			~addr.sendMsg("/algaudio/reply", 1, msg[msg.size-1]);
		}, 50, {
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
		~minstances.add( id -> Synth.new(name)); // TODO: default args
		~addr.sendMsg("/algaudio/reply", id, msg[msg.size-1]);
	}, '/algaudioSC/newinstance'
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

// Args: instance id, parram name, value
OSCdef.new( 'setparram', {
		arg msg;
		if((msg[1] == -1),{
			("Not setting parram " ++ msg[2].asString ++ " of " ++ msg[1].asString ++ " to " ++ msg[3] ++ ", because it's not a valid instance!").postln;
		});
		("Setting parram " ++ msg[2].asString ++ " of " ++ msg[1].asString ++ " to " ++ msg[3]).postln;
		~minstances[msg[1]].set(
			msg[2].asString,
			msg[3]
		);
	}, '/algaudioSC/setparram'
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
