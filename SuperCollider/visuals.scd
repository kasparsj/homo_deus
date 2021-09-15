(
~hd.command(\DEBUG_ALL);
~hd.command(\DEBUG_INTERSECTION);
~hd.command(\DEBUG_CONNECTION);
)

~hd.stopAll();

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
