(
"midi.scd".loadRelative;
"rgb.scd".loadRelative;
)

// define file paths
(
var basePath = PathName(thisProcess.nowExecutingPath).parentPath;
var musicPath = basePath ++ "/literally the moon";
// var wavPath = musicPath ++ "/literally the moon - dependencies.wav";
// ~midiPath = musicPath ++ "/literally the moon - dependencies.mid";
var wavPath = musicPath ++ "/literally the moon - alpine upstream (102 BPM).wav";
~midiPath = musicPath ++ "/literally the moon - alpine upstream (102 BPM).mid";
// ~tempo = 72.5;
~buffer = Buffer.read(s, wavPath);
~lights = NetAddr("192.168.1.100", 54321);
// ~lights = NetAddr("127.0.0.1", 54321);
)

~midi.printStats(~midiPath);

(
var params = (
	SPEED: 1,
	COLOR: 14,
	NOTEID: 16,
	LIFE: 13,
	LENGTH: 6,
	BRIGHTNESS: 18,
	BEHAVIOUR: 19,
);
var stats = ~midi.getStats(~midiPath, ~tempo);
~onMidi = { |time, status, chan, note, velocity|
	var rgb = ~intToRgb.(note, stats.minNote, stats.maxNote);
	// var rgb = ~intToRgb.(velocity * 2, 0, 255);
	("[%.2f] MIDI: [%] % (%)\ n".format(time, chan, note, velocity)).postln;
	if (status == \noteOn) {
		var args = [
			'/note_on',
			params[\SPEED], (velocity / 30.0),
			params[\COLOR], ~hexColor.(rgb),
			params[\NOTEID], note,
			params[\LENGTH], (velocity / 4).asInteger,
			params[\BRIGHTNESS], (velocity * 2).asInteger,
			params[\LIFE], 10000,
		];
		if (note < 49) {
			args = args.addAll([params[\BEHAVIOUR], 4]);
		};
		~lights.sendMsg(*args);
	} {
		~lights.sendMsg('/note_off', note);
	};

};
)

(
~midi.playMidi(~midiPath, ~onMidi, ~tempo);
Synth(\playBuf, [\buf, ~buffer]);
)

// (
// ~lights.sendMsg('/note_on',
// 	14, 0x0000ff,
// 	16, 1,
// 	13, 10000,
// );
// )
//
// ~lights.sendMsg('/note_off', 1);
