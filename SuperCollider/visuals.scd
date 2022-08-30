(
~hd.command(\DEBUG_ALL);
~hd.command(\DEBUG_INTERSECTION);
~hd.command(\DEBUG_CONNECTION);
)

~hd.stopAll();

(
Pdef(\segment_test, Pbind(
	\type, \hd,
	\behaviour, \RENDER_SEGMENT,
	\length, 1,
)).play;
)

Pdef(\segment_test).stop;

(
Pdef(\fade_test, Pbind(
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
)).play;
)

Pdef(\fade_test).stop;

(
Pdef(\mirror_test, Pbind(
	\type, \hd,
	\behaviour, \MIRROR,
	// \behaviour, \MIRROR_ROTATE,
	// \from, 0,
	// \behaviour, [\MIRROR_FLIP, \RENDER_SEGMENT],
	// \speed, 1,
	// \length, 1,
)).play;
)

Pdef(\mirror_test).stop;