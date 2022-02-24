(
var hdP = ~hdP;
~dirt.soundLibrary.addSynth(\hd_drone, (play: {
	hdP.drone(
		\instrument, ~instrument ? [\SineDrone, \SineDrone, \PulseDrone].choose,
		\freq, ~freq ? (51..53).scramble,
		\pan, ~pan ? [-1, 1].scramble,
		\dur, Pseq([~sustain], 1),
		\sustain, ~sustain,
		\amp, ~amp,
	).play;
}));
~dirt.soundLibrary.addSynth(\hd_harpsi, (play: {
	hdP.harpsi(
		\freq, ~freq,
		\pan, ~pan,
		\dur, Pseq([~sustain], 1),
		\amp, ~amp,
	).play;
}));
~dirt.soundLibrary.addSynth(\hd_crickets, (play: {
	hdP.crickets(
		\type, if (~gain > 0, { \hd }, { \pause }),
		\freq, ~freq,
		\pan, ~pan,
		\dur, Pseq([~sustain], 1),
	).play;
}));
)