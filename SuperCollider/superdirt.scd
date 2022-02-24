(
var hdP = ~hdP;
~dirt.soundLibrary.addSynth(\hd_drone, (play: {
	~freq = (~freq ? 51);
	hdP.drone(
		\instrument, ~instrument ? [\SineDrone, \SineDrone, \PulseDrone].choose,
		\freq, (~freq..(~freq+2)).scramble,
		\pan, ~pan ? [-1, 1].scramble,
		\dur, Pseq([~sustain], 1),
		\sustain, ~sustain,
		\amp, ~amp * ~gain,
	).play;
}));
~dirt.soundLibrary.addSynth(\hd_harpsi, (play: {
	hdP.harpsi(
		\freq, ~freq,
		\pan, ~pan,
		\dur, Pseq([~sustain], 1),
		\amp, ~amp * ~gain,
	).play;
}));
~dirt.soundLibrary.addSynth(\hd_crickets, (play: {
	hdP.crickets(
		\type, if (~gain > 0, { \hd }, { \pause }),
		//\freq, ~freq,
		//\pan, ~pan,
		//\dur, Pseq([~sustain], 1),
	).play;
}));
)