OSCdef.new(
	\hello,
	{
		"Hello World!".postln;
	},
	'/hello'
);


OSCdef.new(
	'installtemplate',
	{
		arg msg, time, addr, recvPort;
		var command, f;
		[msg, time, addr, recvPort].postln;
		if((msg.size < 2),{
			"Invalid message".postln;
		},{
			command = "SynthDef.new('aa/" ++ msg[1] ++ "', {" ++ msg[2] ++ "}).add;";
			//command.postln;
			f = command.compile();
			f.value();
		});
		addr.sendMsg("/algaudio/reply", "aaaaaa", msg[msg.size-1]);
	},
	'/algaudioSC/installtemplate'
)
