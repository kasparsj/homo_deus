(
var hd = ~hd;
var hdP = ~hdP;
var openFrameworks = NetAddr("127.0.0.1", 12345);
var floatToRgb, hueToRgb, hslToRgb, rgbToHsl, rgbArray, getColorFrom;

floatToRgb = { |value, mul, add|
	hslToRgb.(value * (mul ? 1) + (add ? 0));
};

hueToRgb = { |p, q, t|
	if(t < 0, { t = t + 1 },
		if (t > 1, { t = t - 1 }));
	if(t < (1/6), { p + (q - p) * 6 * t },
		if(t < (1/2), { q },
			if(t < (2/3), { p + (q - p) * ((2/3) - t) * 6 },
				{ p })));
};

hslToRgb = { |h, s=1, l=0.5|
	var r, g, b;
	if (s === 0) {
		r = g = b = l; // achromatic
	} {
		var q = if (l < 0.5, { l * (1 + s) }, { l + s - l * s });
		var p = 2 * l - q;
		r = hueToRgb.(p, q, h + (1/3));
		g = hueToRgb.(p, q, h);
		b = hueToRgb.(p, q, h - (1/3));
	};
	[(r * 255).round.asInteger, (g * 255).round.asInteger, (b * 255).round.asInteger];
};

rgbArray = { |rgb|
	if (rgb.isArray.not) {
		var red = (rgb >> 16) & 0xFF;
		var green = (rgb >> 8) & 0xFF;
		var blue = rgb & 0xFF;
		rgb = [red, green, blue];
	};
	rgb;
};

rgbToHsl = { |rgb|
	var r, g, b, vmax, vmin, h, s, l;
	var rgbArr = rgbArray.(rgb);
	r = rgbArr[0] / 255;
	g = rgbArr[1] / 255;
	b = rgbArr[2] / 255;
	vmax = [r, g, b].maxItem;
	vmin = [r, g, b].minItem;
	h = s = l = (vmax + vmin) / 2;

	if (vmax == vmin) {
		[0, 0, l]; // achromatic
	} {
		var d = vmax - vmin;
		s = if (l > 0.5, { d / (2 - vmax - vmin) }, { d / (vmax + vmin) });
		if (vmax === r) {
			h = (g - b) / d + if (g < b, 6, 0);
		};
		if (vmax === g) {
			h = (b - r) / d + 2;
		};
		if (vmax === b) {
			h = (r - g) / d + 4;
		};
		h = h / 6;

		[h, s, l];
	};
};

getColorFrom = { |src|
	if (~hdhue.notNil) {
		var hsl = rgbToHsl.(~hdhue);
		~hdhueadd = (~hdhueadd ? 0) - 0.5 + hsl[0];
	};
	switch ((src ? 0))
	// gain
	{ 1 } {
		if (~gain.notNil, {
			floatToRgb.(~gain, ~hdhuemul, ~hdhueadd);
		}, nil);
	}
	// note
	{
		if (~freq.notNil, {
			floatToRgb.(~freq.cpsmidi.floor/127, ~hdhuemul, ~hdhueadd);
		}, nil);
	};
};

~dirt.addModule(\hd, { |dirtEvent|
	if (~gain > 0 and: { ~hdon.isNil } or: { ~hdon.notNil and: { ~hdon > 0 } }) {
		var sustain = ~hdsustain ? ~sustain;
		(
			type: \hd,
			sustain: sustain,
			model: ~hdmodel,
			speed: ~hdspeed,
			ease: ~hdease,
			fade: ~hdfade,
			thresh: ~hdthresh,
			fadeEase: ~hdfadease,
			length: (~hdlength ? ~hdlen) ?? { sustain * (~hdlenmul ? 10) },
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
			color: ~hdcolor ?? { getColorFrom.(~hdcolorsrc) },
		).play;
	};
}, {~hdmodel.notNil or: { ~hdlength.notNil } or: { ~hdspeed.notNil } or: { ~hdorder.notNil } or: { ~hdbehaviour.notNil }});

~dirt.soundLibrary.addSynth(\hd_drone, (play: {
	var args;
	~freq = (~freq ? 51);
	// todo: support all hd params
	args = [
		\instrument, if (~synth.notNil, { ~synth.asSymbol }, { [\SineDrone, \SineDrone, \PulseDrone].choose }),
		\freq, (~freq..(~freq+2)).scramble,
		\pan, ~pan ? [-1, 1].scramble,
		\dur, Pseq([~sustain], 1),
		\sustain, ~hdsustain ? ~sustain,
		\amp, ~amp * ~gain,
	];
	hdP.drone(*args).play;
}));

~dirt.soundLibrary.addSynth(\hd_harpsi, (play: {
	var args;
	// todo: support all hd params
	args = [
		\freq, ~freq,
		\pan, ~pan,
		\dur, Pseq([~sustain], 1),
		\amp, ~amp * ~gain,
		\color, ~hdcolor ? -1,
	];
	hdP.harpsi(*args).play;
}));

~dirt.soundLibrary.addSynth(\hd_crickets, (play: {
	if (~gain > 0) {
		var args = [
			//\freq, ~freq,
			//\pan, ~pan,
			// \dur, Pseq([~sustain], 1),
			\dur, Pwhite(1, 4, inf),
			\color, ~hdcolor ? [0, 255, 0],
			\behaviour, Prand([[\FILL_EASE], [\FILL_EASE, ~hdbehaviour ? 0]], inf),
			\brightness, ~hdbrightness ? hd[\fullBrightness],
		];
		hdP.startCrickets(args, ~gain, ~fade);
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