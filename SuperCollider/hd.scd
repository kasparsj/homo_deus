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
	COLORINDEX: 15,
	NOTE_ID: 16,
	NOTEID: 16,
	MIN_BRI: 17,
	MAX_BRI: 18,
	BRIGHTNESS: 18, // alias
	BEHAVIOUR: 19,
	EMIT: 20,
	EMIT_GROUPS: 20,
	EMIT_OFFSET: 21,
	OFFSET: 21, // alias
	COLOR_CHANGE_GROUPS: 22,
);

~hd[\INT_PARAM] = [\LENGTH, \MODEL, \FADE, \THRESH, \TRAIL, \ORDER, \HEAD, \LINKED, \FROM, \MIN_BRI, \BRIGHTNESS, \EMIT_OFFSET];
~hd[\FLOAT_PARAM] = [\SPEED];
~hd[\CUSTOM_PARAM] = (EASE: \getEase, FADE_EASE: \getEase, BEHAVIOUR: \getBehaviour, EMIT: \getGroups, EMIT_GROUPS: \getGroups, COLOR_CHANGE_GROUPS: \getGroups);

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
	MIRROR_FLIP: 512,
	MIRROR: 512, // alias
	MIRROR_ROTATE: 1024,
	SMOOTH_CHANGES: 2048,
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
		//var emitLatency = (thisThread.clock.tempo.reciprocal*timingOffset)+lag+serverLatency;
		var emitLatency = (1*timingOffset)+lag+serverLatency;
		var dur = (sustain+timingOffset);
		var duration = dur+lag+serverLatency;
		var args = List.newUsing([emitLatency, "/emit"]);
		var color = self.getColor(~color.value);
		var keys = currentEnvironment.keys.reject({|key| Set[\COLOR, \COLORINDEX, \COLOR_INDEX, \DURATION].includes(key.asString.toUpper.asSymbol) });
		var paramKeys = self[\PARAM].keys;
		keys.do { |key|
			var param = key.asString.toUpper.asSymbol;
			if (paramKeys.includes(param), {
				var value = currentEnvironment[key].value;
				if (self[\INT_PARAM].includes(param), { value = value.asInteger; });
				if (self[\FLOAT_PARAM].includes(param), { value = value.asFloat; });
				if (self[\CUSTOM_PARAM][param].notNil, { value = self.perform(self[\CUSTOM_PARAM][param], value); });
				args.addAll([self[\PARAM][param], value]);
			});
		};
		if (color.notNil) {
			args.addAll([self[\PARAM][\COLOR], color]);
		} {
			if (~colorIndex.value.notNil) {
				args.addAll([self[\PARAM][\COLOR_INDEX], ~colorIndex.value.asInteger]);
			};
		};
		if (self.useEmit) {
			args[1] = "/emit";
			// emit (with duration)
			args.addAll([self[\PARAM][\DURATION], (duration*1000).asInteger]);
			self.sendMsgIn(*args);
		} {
			// note_on + note_off
			var argsOff;
			//var offTime = (thisThread.clock.tempo.reciprocal*dur)+lag+serverLatency;
			var offTime = (1*dur)+lag+serverLatency;
			var noteId = ~noteId.value ? self.getNoteId();
			args[1] = "/note_on";
			self.sendMsgIn(*args);
			argsOff = [offTime, "/note_off", noteId.asInteger];
			self.sendMsgIn(*argsOff);
		};
	}, (legato: 1.0));
	Event.addEventType(\hd_note, { |server|
		~eventTypes[\hd].value(server);
		~eventTypes[\note].value(server);
	}, (legato: 1.0));
	Event.addEventType(\hd_setn, { |server|
		var timingOffset = ~timingOffset.value;
		var lag = ~lag.value;
		var sustain = ~sustain.value;
		var serverLatency = ~latency.value ? (server.latency ? 0);
		var emitLatency = (thisThread.clock.tempo.reciprocal*timingOffset)+lag+serverLatency;
		//var emitLatency = (1*timingOffset)+lag+serverLatency;
		var dur = (sustain+timingOffset);
		var duration = dur+lag+serverLatency;
		var args = List.newUsing([emitLatency, "/notes_set"]);
		var noteIds = ~noteId.value;
		if (noteIds.notNil, {
			if (noteIds.isSequenceableCollection.not, {
				noteIds = [noteIds];
			});
			noteIds.do { |noteId, i|
				var keys = currentEnvironment.keys.reject({|key| Set[\NOTEID, \DURATION].includes(key.asString.toUpper.asSymbol) });
				var paramKeys = self[\PARAM].keys;
				keys.do { |key|
					var param = key.asString.toUpper.asSymbol;
					if (paramKeys.includes(param), {
						var value = currentEnvironment[key].value;
						if (self[\INT_PARAM].includes(param), { value = value.asInteger; });
						if (self[\FLOAT_PARAM].includes(param), { value = value.asFloat; });
						if (self[\CUSTOM_PARAM][param].notNil, { value = self.perform(self[\CUSTOM_PARAM][param], value); });
						if (value.isArray, {
							value = value[i];
						});
						args.addAll([noteId, self[\PARAM][param], value]);
					});
				};
				args.addAll([noteId, self[\PARAM][\DURATION], (duration*1000).asInteger])
			};
			self.sendMsgIn(*args);
		});
	});
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
	color = color ? -1;
	if (color < 0) {
		nil
	} {
		color;
	};
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