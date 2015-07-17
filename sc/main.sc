// This method estabilishes two-way osc connection

OSCdef.new(
	'hello',
	{
		arg msg, time, addr, recvPort;
		[msg, time, addr, recvPort].postln;
		// This is when the ~addr is stored.
		~addr = addr;
		"Hello World!".postln;
		~addr.sendMsg("/algaudio/reply",msg[msg.size-1]);
	},
	'/algaudioSC/hello'
).postln;


OSCdef.new(
	'installtemplate',
	{
		arg msg;
		var command, f;
		if((msg.size < 2),{
			"Invalid message".postln;
		},{
			command = "SynthDef.new('aa/" ++ msg[1] ++ "', {" ++ msg[2] ++ "}).add;";
			//command.postln;
			f = command.compile();
			f.value();
		});
		~addr.sendMsg("/algaudio/reply", "aaaaaa", msg[msg.size-1]);
	},
	'/algaudioSC/installtemplate'
).postln;

// A helper method for notifying the app if starting the server succeeded.
OSCdef.new(
	'boothelper',
	{
		arg msg;
		s.waitForBoot({
			~addr.sendMsg("/algaudio/reply", 1, msg[msg.size-1]);
		}, 50, {
			~addr.sendMsg("/algaudio/reply", 0, msg[msg.size-1]);
		});
	},
	'/algaudioSC/boothelper'
).postln;
