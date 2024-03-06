(
var tidalNetAddr = NetAddr.new("127.0.0.1", 6010);
var muteNotes = 86 + [0, 2, 3, 5, 7, 8, 10, 12];
var soloNotes = 72 + [0, 2, 3, 5, 7, 8, 10, 12];
~circuit = Circuit.new.connect("M-Track");
~circuit.normalize = true;
~circuit.onControl({ |value, i|
	~dirt.orbits[i].set(\amp, value)
}, \synth1);
~circuit.onControl({ |value, i|
	~dirt.orbits[(8+i)].set(\amp, value);
}, \synth2);
~circuit.toggleMode(\synth1);
~circuit.toggleMode(\synth2);
8.do { |i|
	~circuit.knob(\synth1, i, ~dirt.orbits[i].get(\amp));
	~circuit.onNoteOn({ |value, num|
		tidalNetAddr.sendMsg("/mute", i + 1)
	}, \synth1, muteNotes[i]);
	~circuit.onNoteOff({ |value, num|
		tidalNetAddr.sendMsg("/unmute", i + 1)
	}, \synth1, muteNotes[i]);
	~circuit.onNoteOn({ |value, num|
		tidalNetAddr.sendMsg("/solo", i + 1)
	}, \synth1, soloNotes[i]);
	~circuit.onNoteOff({ |value, num|
		tidalNetAddr.sendMsg("/unsolo", i + 1)
	}, \synth1, soloNotes[i]);

	if ((8+i) < ~dirt.orbits.size, {
		~circuit.knob(\synth2, i, ~dirt.orbits[(8+i)].get(\amp));
		~circuit.onNoteOn({ |value, num|
			tidalNetAddr.sendMsg("/mute", i + 9)
		}, \synth2, muteNotes[i]);
		~circuit.onNoteOff({ |value, num|
			tidalNetAddr.sendMsg("/unmute", i + 9)
		}, \synth2, muteNotes[i]);
		~circuit.onNoteOn({ |value, num|
			tidalNetAddr.sendMsg("/solo", i + 9)
		}, \synth2, soloNotes[i]);
		~circuit.onNoteOff({ |value, num|
			tidalNetAddr.sendMsg("/unsolo", i + 9)
		}, \synth2, soloNotes[i]);
	});
};
)
