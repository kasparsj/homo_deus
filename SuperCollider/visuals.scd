(
~hd.command(\DEBUG_ALL);
~hd.command(\DEBUG_INTERSECTION);
~hd.command(\DEBUG_CONNECTION);
)

~hd.stopAll();

// every second new from outside
(
Pbind(
	\type, \hd,
	\model, 0,
	\ease, \EXPONENTIAL_INOUT,
	\behaviour, \FILL_EASE,
	\brightness, Pwhite(127, 255),
).play;
~c31 = Synth(\cricket3, [\out,0,\trigFreq, 2]);
~c32 = Synth(\cricket3, [\out,1,\trigFreq, 0.5]);
)

//
(
Pbind(
	\type, \hd,
	\behaviour, \RENDER_SEGMENT,
	\length, 1,
).play;
)

//
(
Pbind(
	\type, \hd,
	\head, 1,
	\trail, 0,
	\length, 60,
	//\offset, Pwhite(0, 100),
	\speed, 0,
	\fade, 0.01,
	//\thresh, 0.98,
	\fadeEase, \EXPONENTIAL_IN,
	//\minBri, 1.0,
).play;
)
