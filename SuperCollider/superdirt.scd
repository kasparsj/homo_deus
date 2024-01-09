(
var hd = ~hd;
var hdP = ~hdP;
var openFrameworks = NetAddr("127.0.0.1", 12345);

~dirt.addModule(\hd, { |dirtEvent|
	if (dirtEvent.event.gain > 0) {
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
			from: if (~hdfrom.notNil, { ~hdfrom.asInteger }, nil),
			minBri: ~hdminbri,
			brightness: ~hdbrightness,
			behaviour: ~hdbehaviour,
			emit: ~hdemit,
			offset: ~hdoffset,
			colorChange: ~hdcolorchange,
			color: ~hdcolor,
		).play;
	};
}, {~hdmodel.notNil or: { ~hdlength.notNil } or: { ~hdspeed.notNil } or: { ~hdorder.notNil } or: { ~hdbehaviour.notNil }});

~dirt.soundLibrary.addSynth(\hd_drone, (play: {
	~freq = (~freq ? 51);
	hdP.drone(
		\instrument, if (~synth.notNil, { ~synth.asSymbol }, { [\SineDrone, \SineDrone, \PulseDrone].choose }),
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
		\color, ~hdcolor ? -1,
	).play;
}));

~dirt.soundLibrary.addSynth(\hd_crickets, (play: {
	if (~gain > 0) {
		hdP.startCrickets(
			//\freq, ~freq,
			//\pan, ~pan,
			// \dur, Pseq([~sustain], 1),
			\dur, Pwhite(1, 4, inf),
			\color, ~hdcolor ? [0, 255, 0],
			\behaviour, Prand([[\FILL_EASE], [\FILL_EASE, ~hdbehaviour ? 0]], inf),
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

~dirt.addModule('videosync', { |dirtEvent|
	openFrameworks.sendMsg("/swap_buffers");
}, {~videosync.notNil});

~dirt.addModule('videospeed', { |dirtEvent|
	openFrameworks.sendMsg("/set_speed", ~videospeed);
}, {~videospeed.notNil});

~dirt.addModule('pg', { |dirtEvent|
	var pg = ~pg; // toggle
	var pgon = ~pgon; // turn on
	var pgoff = ~pgoff; // turn off
	if (pg.notNil, {
		openFrameworks.sendMsg("/toggle_glitch", pg);
		if (~sustain.notNil, {
			SystemClock.sched(~sustain, {
				openFrameworks.sendMsg("/toggle_glitch", pg);
				nil;
			});
		});
	}, {
		if (pgon.notNil, {
			openFrameworks.sendMsg("/set_glitch", pgon, true);
		});
		if (pgoff.notNil, {
			openFrameworks.sendMsg("/set_glitch", pgoff, false);
		});
	});
}, {~pg.notNil or: { ~pgon.notNil } or: { ~pgoff.notNil }});

)