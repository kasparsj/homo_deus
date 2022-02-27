(
var hd = ~hd;
var hdP = ~hdP;
~dirt.addModule(\hd, { |dirtEvent|
	(
		type: \hd,
		sustain: ~hdsustain ? ~sustain,
		model: ~hdmodel,
		speed: ~hdspeed,
		ease: ~hdease,
		fade: ~hdfade,
		thresh: ~hdthresh,
		fadeEase: ~hdfadease,
		length: ~hdlength,
		trail: ~hdtrail,
		order: ~hdorder,
		head: ~hdhead,
		linked: ~hdlinked,
		from: ~hdfrom,
		minBri: ~hdminbri,
		brightness: ~hdbrightness,
		behaviour: ~hdbehaviour,
		emit: ~hdemit,
		offset: ~hdoffset,
		colorChange: ~hdcolorchange,
		color: ~hdcolor,
	).play;
}, {~hdmodel.notNil or: { ~hdlength.notNil } or: { ~hdspeed.notNil } or: { ~hdorder.notNil } or: { ~hdbehaviour.notNil }});
~dirt.soundLibrary.addSynth(\hd_drone, (play: {
	~freq = (~freq ? 51);
	hdP.drone(
		\instrument, ~instrument ? [\SineDrone, \SawDrone, \PulseDrone].choose,
		\freq, (~freq..(~freq+2)).scramble,
		\pan, ~pan ? [-1, 1].scramble,
		\dur, Pseq([~sustain], 1),
		\sustain, ~hdsustain ? ~sustain,
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
	if (~gain > 0) {
		hdP.startCrickets(
			//\freq, ~freq,
			//\pan, ~pan,
			// \dur, Pseq([~sustain], 1),
			\dur, Pwhite(1, 4, inf),
			\color, ~hdcolor,
			//\behaviour, ~hdbehaviour ? 0,
			\behaviour, Prand([[\FILL_EASE], [\FILL_EASE, \RANDOM_COLOR]], inf),
			\brightness, ~hdbrightness ? hd[\fullBrightness],
		);
	} {
		hdP.stopCrickets(~fade);
	};
}));
~dirt.soundLibrary.addSynth(\hd_wind, (play: {
	if (~gain > 0) {
		hdP.startWind(~windchange, ~windrq, ~gain);
	} {
		hdP.stopWind(~fade);
	}
}));
)