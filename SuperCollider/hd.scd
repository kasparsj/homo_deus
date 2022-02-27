(
"synths/*.scd".loadRelative;

s.sync;

~hd = (
	fps: 62.5,
	noteId: 1,
	fullBrightness: 255,
	infDur: 4294967295,
	useEmit: true,
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
	DURATION: 12,
	DURATION_MS: 12,
	DURATION_FRAMES: 13,
	LIFE: 13,
	COLOR: 14,
	COLOR_INDEX: 15,
	NOTE_ID: 16,
	MIN_BRI: 17,
	MAX_BRI: 18,
	BRIGHTNESS: 18, // alias
	BEHAVIOUR: 19,
	EMIT_GROUPS: 20,
	EMIT_OFFSET: 21,
	OFFSET: 21, // alias
	COLOR_CHANGE_GROUPS: 22,
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
	RANDOM_COLOR: 256,
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
	BACK_IN: 25,
    BACK_OUT: 26,
    BACK_INOUT: 27,
	BOUNCE_IN: 28,
    BOUNCE_OUT: 29,
    BOUNCE_INOUT: 30,
	ELASTIC_IN: 31,
    ELASTIC_OUT: 32,
    ELASTIC_INOUT: 33,
);

~hd.init = { |self|
	Event.addEventType(\hd, { |server|
		var timingOffset = ~timingOffset.value;
		var lag = ~lag.value;
		var sustain = ~sustain.value;
		var serverLatency = ~latency.value ? (server.latency ? 0);
		var emitLatency = (thisThread.clock.tempo.reciprocal*timingOffset)+lag+serverLatency;
		var dur = (sustain+timingOffset);
		var args = [emitLatency, "/emit",
			self[\PARAM][\MODEL], ~model.value,
			self[\PARAM][\SPEED], ~speed.value ? -1.0,
			self[\PARAM][\EASE], if (~ease.value.isNil, { 0 }, { self.getEase(~ease.value) }),
			self[\PARAM][\FADE], ~fade.value ? 0.0,
			self[\PARAM][\THRESH], ~thresh.value ? 0.0,
			self[\PARAM][\FADE_EASE], if (~fadeEase.value.isNil, { 0 }, { self.getEase(~fadeEase.value) }),
			self[\PARAM][\LENGTH], if (~length.value.isNil, { nil }, { ~length.value.asInteger }),
			self[\PARAM][\TRAIL], ~trail.value ? 0,
			self[\PARAM][\ORDER], ~order.value ? 0,
			self[\PARAM][\HEAD], ~head.value ? 0,
			self[\PARAM][\LINKED], ~linked.value ? 1,
			self[\PARAM][\FROM], ~from.value ? -1,
			self[\PARAM][\MIN_BRI], ~minBri.value ? 0,
			self[\PARAM][\BRIGHTNESS], ~brightness.value ? self.fullBrightness,
			self[\PARAM][\BEHAVIOUR], if (~behaviour.value.isNil, { 0 }, { self.getBehaviour(~behaviour.value) }),
			self[\PARAM][\EMIT_GROUPS], if (~emit.value.isNil, { 0 }, { self.getGroups(~emit.value) }),
			self[\PARAM][\EMIT_OFFSET], ~offset.value ? 0,
			self[\PARAM][\COLOR_CHANGE_GROUPS], if (~colorChange.value.isNil, { 0 }, { self.getGroups(~colorChange.value) }),
		];
		var argsOff;
		if (~color.value != nil) {
			args = args.addAll([self[\PARAM][\COLOR], self.getColor(~color.value)]);
		};
		if (self.useEmit) {
			// emit (with duration)
			var duration = dur+lag+serverLatency;
			args[1] = "/emit";
			args = args.addAll([self[\PARAM][\DURATION], (duration*1000).asInteger]);
			self.sendMsgIn(*args);
		} {
			// note_on + note_off
			var noteId = self.getNoteId();
			var offTime = (thisThread.clock.tempo.reciprocal*dur)+lag+serverLatency;
			args = args.addAll([self[\PARAM][\NOTE_ID], noteId]);
			args[1] = "/note_on";
			self.sendMsgIn(*args);
			argsOff = [offTime, "/note_off", noteId];
			self.sendMsgIn(*argsOff);
		};
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
		groups = groups | (2.pow(idx.asInteger).asInteger);
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

~hd.getColor = { |self, color|
	if (color.isArray) {
		color = ((color[0] & 0x0ff) << 16) | ((color[1] & 0x0ff) << 8) | (color[2] & 0x0ff);
	};
	color ? 0;
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

~hd.setPalette = { |self, palette|
	self.sendMsg("/palette", palette);
};

~hd.init;

s.sync;

//"compositions/*.scd".loadRelative;
//"mapping/*.scd".loadRelative;
"master.scd".loadRelative;

"###     homo deus loaded     ####".postln;

)