//Quarks.install("https://github.com/dkmayer/SimpleMIDIFile");

(
SynthDef(\playBuf, {
	|out = 0, buf|
	var sig = PlayBuf.ar(2, buf, BufRateScale.kr(buf), doneAction: 2);
	// todo: sync using SendReply to Routine
	Out.ar(out, sig);
}).add;

~midi = (
	ticksPerQuarter: 96,
	cache: (),
);

~midi.parse = { |self, midiPath, defaultTempo = 120|
	if (self.cache[midiPath].isNil) {
		var midiFile = SimpleMIDIFile.read(midiPath);
		var events = midiFile.noteEvents;
		var tempo = if (midiFile.tempoMap.size > 0, { midiFile.tempoMap[0][1] }, { self.getTempo(midiPath, defaultTempo) });
		self.cache[midiPath] = (events: events.sortBy(1), tempo: tempo)
	};
	self.cache[midiPath];
};

~midi.getTempo = { |self, filename, defaultTempo = nil|
	var match = filename.findRegexp("\\((\\d+) BPM\\)");
	var tempo = if (match.notNil and: { match.size > 0 }) {
		match[1][1].asInteger;
	} {
		defaultTempo;
	};
	tempo;
};

~midi.ticksToSecs = { |self, ticks, tempo|
	ticks * (60 / tempo) / self.ticksPerQuarter;
};

~midi.findNoteOff = { |self, events, key, i|
	var ev = nil;
	var status, velocity;
	var continue = true;
	while { (i < events.size) && continue } {
		ev = events[i];
		i = i + 1;
		status = ev[2];
		velocity = ev[5];
		if (status == \noteOff or: { status == \noteOn and: { velocity == 0 } }) {
			continue = false;
		};
	};
	ev;
};

~midi.findEndTick = { |self, events, key, i|
	var endEv = self.findNoteOff(events, key, (i+1));
	endEv[1];
};

~midi.getStats = { |self, midiPath, defaultTempo = 120|
	var noteStats = Dictionary.new;
	var durations = List.new;
	var velocities = List.new;
	var activeNotes = Dictionary.new;
	var midi = self.parse(midiPath, defaultTempo);
	var events = midi.events;
	var tempo = midi.tempo;
	var notes;

	events.do { |ev, i|
		var status = ev[2];  // 'noteOn' or 'noteOff'
		var tick = ev[1];
		var chan = ev[3];
		var note = ev[4];
		var velocity = ev[5];
		var key = [chan, note];

		if (status == \noteOn and: { velocity > 0 }) {
			// var dur = self.ticksToSecs((self.findEndTick(events, key, i) - tick), tempo).postln;
			// Save note start time
			activeNotes[[chan, note]] = tick;
		} {
			if (status == \noteOff or: { status == \noteOn and: { velocity == 0 } }) {
				var startTick = activeNotes[key];
				if (startTick.notNil) {
					var durTicks = tick - startTick;
					var durSecs = self.ticksToSecs(durTicks, tempo);

					// Record stats
					durations.add(durSecs);
					velocities.add(velocity);
					noteStats[note] = if (noteStats[note].isNil, 1, { noteStats[note] + 1 });

					// Cleanup
					activeNotes.removeAt(key);
				};
			};
		};
	};

	notes = noteStats.keys.asList;
	(
		noteStats: noteStats,
		minNote: notes.minItem,
		maxNote: notes.maxItem,
		durations: durations,
		minDur: durations.minItem,
		maxDur: durations.maxItem,
		avgDur: durations.mean,
		minVel: velocities.minItem,
		maxVel: velocities.maxItem,
		avgVel: velocities.mean,
	);
};

~midi.printStats = { |self, midiPath, defaultTempo = 120|
	var stats = self.getStats(midiPath, defaultTempo);

	"Note Value Frequencies:".postln;
	stats.noteStats.asSortedArray.do { |note|
		("Note %: %".format(note[0], note[1])).postln;
	};


	"---".postln;
	"Min note: %".format(stats.minNote).postln;
	"Max note: %".format(stats.maxNote).postln;
	"Min duration: % seconds".format(stats.minDur).postln;
	"Max duration: % seconds".format(stats.maxDur).postln;
	"Avg duration: % seconds".format(stats.avgDur).postln;
	"Min velocity: %".format(stats.minVel).postln;
	"Max velocity: %".format(stats.maxVel).postln;
	"Avg velocity: %".format(stats.avgVel).postln;
};

~midi.playMidi = { |self, midiPath, callback, defaultTempo = 120|
	var midi = self.parse(midiPath);
	var events = midi.events;
	var tempo = midi.tempo;

	Routine({
		var lastTime = 0.0;
		events.do { |ev|
			var tick = ev[1];
			var status = ev[2];  // 'noteOn' or 'noteOff'
			var chan = ev[3];
			var note = ev[4];
			var velocity = ev[5];
			var time = self.ticksToSecs(tick, tempo);
			var wait = time - lastTime;
			lastTime = time;

			wait.wait;

			callback.(time, status, chan, note, velocity);
		};
	}).play(SystemClock);
};
)
