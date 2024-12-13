(
s.latency = 0.1;
s.waitForBoot {
	"../hd.scd".loadRelative;
	"../patterns.scd".loadRelative;
	if (~dirt.notNil, {
		"../superdirt.scd".loadRelative;
		"../circuit.scd".loadRelative;
	});

	// does not work with hotspot (works with router)
	// ~hd.net = NetAddr("homo-deus", 54321);
	~hd.net = NetAddr("127.0.0.1", 54321);
	// ~hd.net = NetAddr("192.168.1.105", 54321); // esp32-6E0B08
	//~hd.net.disconnect;
};
)

(
b = Buffer.read(s, "../../pagrabs-imwi.wav".resolveRelative);

RedMst.clear;
RedMst.clock = TempoClock(60/60);
RedMst.quant = 1;

RedTrack(\buf, Synth(\playbuf, [\bufnum, b.bufnum]).run(false));
RedTrack(\all, RedFn({
	~hd.command(\debug_all); // red
}, {
	~hd.command(\debug_all); // red
}));
RedTrack(\inter, RedFn({
	~hd.command(\debug_intersection); // blue
}, {
	~hd.command(\debug_intersection); // blue
}));
RedTrack(\conn, RedFn({
	~hd.command(\debug_connection); // green
}, {
	~hd.command(\debug_connection); // green
}));

~score = RedScore([
	20, [\buf],
	20, [\buf, \inter],
	20, [\buf, \conn],
	20, [\buf, \all],
	20, [\buf, \all, \conn],
	20, [\buf, \all, \inter],
	20, [\buf, \all, \inter, \conn],
	inf, [\buf]
]);
// todo: use RedSeq instead
~sound = ~score.createSeq;
)

~sound.play;
