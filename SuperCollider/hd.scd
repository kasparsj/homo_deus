(
"synths/*.scd".loadRelative;

s.sync;

~hd = (
	fps: 62.5,
	noteId: 1,
);

~hd[\CMD] = (
	TOGGLE_AUTO: "e",
	DEBUG_ALL: "a",
	DEBUG_INTERSECTION: "i",
	DEBUG_CONNECTION: "c",
);

~hd[\PARAM] = (
	MODEL: 0,
	SPEED: 1,
	EASE: 2,
	FADE: 3,
	FADE_THRESH: 4,
	THRESH: 4, // alias
	FADE_EASE: 5,
	LENGTH: 6,
	TRAIL: 7,
	ORDER: 8,
	HEAD: 9,
	LINKED: 10,
	FROM: 11,
	LIFE: 12,
	COLOR: 13,
	NOTE_ID: 14,
	MIN_BRI: 15,
	MAX_BRI: 16,
	BRIGHTNESS: 16, // alias
	BEHAVIOUR: 17,
	EMIT_GROUPS: 18,
	EMIT_OFFSET: 19,
	OFFSET: 19, // alias
	COLOR_CHANGE_GROUPS: 20,
);

~hd[\BEHAVIOUR] = (
	POS_CHANGE_FADE: 1,
	BRI_CONST_NOISE: 2,
	RENDER_SEGMENT: 4,
	ALLOW_BOUNCE: 8,
	FORCE_BOUNCE: 16,
	EXPIRE_IMMEDIATE: 32,
	EMIT_FROM_CONN: 64,
	FILL_EASE: 128,
);

~hd[\LIST_ORDER] = (
	SEQUENTIAL: 0,
	RANDOM: 1,
	NOISE: 2,
	OFFSET: 3,
);

~hd[\EASE] = (
	NONE: 0,
    LINEAR_IN: 1,
    LINEAR_OUT: 2,
    LINEAR_INOUT: 3,
    SINE_IN: 4,
    SINE_OUT: 5,
    SINE_INOUT: 6,
    CIRCULAR_IN: 7,
    CIRCULAR_OUT: 8,
    CIRCULAR_INOUT: 9,
    QUADRATIC_IN: 10,
    QUADRATIC_OUT: 11,
    QUADRATIC_INOUT: 12,
    CUBIC_IN: 13,
    CUBIC_OUT: 14,
    CUBIC_INOUT: 15,
    QUARTIC_IN: 16,
    QUARTIC_OUT: 17,
    QUARTIC_INOUT: 18,
    QUINTIC_IN: 19,
    QUINTIC_OUT: 20,
    QUINTIC_INOUT: 21,
    EXPONENTIAL_IN: 22,
    EXPONENTIAL_OUT: 23,
    EXPONENTIAL_INOUT: 24,
);

~hd.init = { |self|
	Event.addEventType(\hd, { |server|
		var timingOffset = ~timingOffset.value;
		var lag = ~lag.value;
		var sustain = ~sustain.value;
		var serverLatency = ~latency.value ? (server.latency ? 0);
		var emitLatency = (thisThread.clock.tempo.reciprocal*timingOffset)+lag+serverLatency;
		var life = (thisThread.clock.tempo.reciprocal*(sustain+timingOffset))+lag+serverLatency;
		var noteId = self.getNoteId();
		var args = [emitLatency, "/note_on",
			self[\PARAM][\NOTE_ID], noteId,
			self[\PARAM][\MODEL], ~model.value,
			self[\PARAM][\SPEED], ~speed.value ? -1.0,
			self[\PARAM][\EASE], if (~ease.value.isNil, { 0 }, { self.getEase(~ease.value) }),
			self[\PARAM][\FADE], ~fade.value ? 0.0,
			self[\PARAM][\THRESH], ~thresh.value ? 0.0,
			self[\PARAM][\FADE_EASE], if (~fadeEase.value.isNil, { 0 }, { self.getEase(~fadeEase.value) }),
			self[\PARAM][\LENGTH], ~length.value,
			self[\PARAM][\TRAIL], ~trail.value ? 0,
			self[\PARAM][\ORDER], ~order.value ? 0,
			self[\PARAM][\HEAD], ~head.value ? 0,
			self[\PARAM][\LINKED], ~linked.value ? 1,
			self[\PARAM][\FROM], ~from.value ? -1,
			self[\PARAM][\MIN_BRI], ~minBri.value ? 0,
			self[\PARAM][\BRIGHTNESS], ~brightness.value ? 1.0,
			self[\PARAM][\BEHAVIOUR], if (~behaviour.value.isNil, { 0 }, { self.getBehaviour(~behaviour.value) }),
			self[\PARAM][\EMIT_GROUPS], if (~emit.value.isNil, { 0 }, { self.getGroups(~emit.value) }),
			self[\PARAM][\EMIT_OFFSET], ~offset.value ? 0,
			self[\PARAM][\COLOR_CHANGE_GROUPS], if (~colorChange.value.isNil, { 0 }, { self.getGroups(~colorChange.value) }),
		];
		//args = args.add((life*self.fps).ceil.asInteger);
		var argsOff = [life, "/note_off", noteId];
		if (~color.value != nil) {
			args = args.addAll([self[\PARAM][\COLOR], ~color.value]);
		};
		self.sendMsgIn(*args);
		self.sendMsgIn(*argsOff);
	}, (legato: 1.0));
	Event.addEventType(\hd_note, { |server|
		~eventTypes[\hd].value(server);
		~eventTypes[\note].value(server);
	}, (legato: 1.0));
	CmdPeriod.add {
		self.stopAll();
	};
};

~hd.getGroups = { |self, idxs|
	var groups = 0;
	if (idxs.isArray.not, {
		idxs = [idxs];
	});
	idxs.do { |idx|
		groups = groups | (2.pow(idx).asInteger);
	};
	groups;
};

~hd.getBehaviour = { |self, flags|
	var behaviour = 0;
	if (flags.isArray.not, {
		flags = [flags];
	});
	flags.do { |flag|
		var value = if (flag.isInteger, { flag }, { self[\BEHAVIOUR][flag.asString.toUpper.asSymbol] });
		behaviour = behaviour | value;
	};
	behaviour;
};

~hd.getEase = { |self, ease|
	if (ease.isInteger.not) {
		ease = self[\EASE][ease.asString.toUpper.asSymbol];
	};
	ease;
};

~hd.getCommand = { |self, cmd|
	self[\CMD][cmd.asString.toUpper.asSymbol] ? cmd;
};

~hd.getNoteId = { |self|
	self.noteId = self.noteId + 1;
	self.noteId;
};

~hd.sendMsg = { |self ... args|
	self.net.sendMsg(*args);
};

~hd.sendMsgIn = { |self, latency ... args|
	SystemClock.sched(latency ? 0, {
		self.net.sendMsg(*args);
	});
};

~hd.translatePairs = { |self, pairs|
	var i=0;
	pairs.pairsDo {|key, value|
		var keyUp = key.asString.toUpper;
		if (key.isInteger.not, {
			pairs[i] = self[\PARAM][keyUp.asSymbol];
		});
		pairs[i+1] = switch (keyUp,
			"EASE", { self.getEase(value) },
			"FADE_EASE", { self.getEase(value) },
			"BEHAVIOUR", { self.getBehaviour(value) },
			"EMIT_GROUPS", { self.getGroups(value) },
			"COLOR_CHANGE_GROUPS", { self.getGroups(value) },
		);
		i = i + 2;
	};
};

~hd.emit = { |self ... args|
	var pairs = self.translatePairs(args);
	self.sendMsg("/emit", *pairs);
};

~hd.noteOn = { |self ... args|
	var pairs = self.translatePairs(args);
	pairs = pairs.addAll([self[\PARAM][\NOTE_ID], self.getNoteId()]);
	self.sendMsg("/note_on", *pairs);
};

~hd.noteOff = { |self, noteId|
	self.sendMsg("/note_off", noteId);
};

~hd.command = { |self, cmd|
	self.sendMsg("/command", self.getCommand(cmd));
};

~hd.stopAll = { |self|
	self.sendMsg("/note_off");
};

~hd.createDrone = { |self ... args|
	var dict = args.asDict;
	var which = (0..2).choose;
	var instruments = [\SineDrone, \SineDrone, \SawDrone, \PulseDrone];
	var dur = rrand(5, 30).asInteger;
	var freq = (51..(51+rrand(0, 3))).scramble;
	var modFreq = dict[\speed] ? Pwhite(0.5, 10.0);
	var pan = [-1, 1].scramble;
	Pbind(
		\type, \hd_note,
		\instrument, Pfunc{ instruments[(0..3).choose] },
		\freq, freq,
		\modFreq, modFreq,
		\dur, dur,
		\pan, pan,
		\phase, Pwhite(0, 2pi, 1),
		\out, 0,
		\amp, 0.1,
		\model, Pwhite(0, 4, 1),
		\from, -1,
		\speed, modFreq,
		\length, freq.size*25,
		//\color, which * 85
		*args
	);
};

~hd.createSpawn = { |self, func, delta|
	Pspawn(Pbind(
		\pattern, Pfunc(func),
		\delta, delta,
		\method, \par
	));
};

~hd.setPalette = { |self, palette|
	self.sendMsg("/palette", palette);
};

~hd.init;

s.sync;

"compositions/*.scd".loadRelative;
"mapping/*.scd".loadRelative;

"###     homo deus loaded     ####".postln;

)