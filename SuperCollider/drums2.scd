(
var window = 1024;
var threshold = 0.5;
var numcoeffs = 3;
var beattrack = false;

~busChannels = 3 + numcoeffs + if (beattrack, { 4 }, { 0 });

SynthDef(\soundin, {|out, in=0|
	var input, amp, chain, onset, loudness, mfcc, track;
	var trackb, trackh, trackq, tempo;
    input = SoundIn.ar(in);
	chain = FFT(LocalBuf(window), input);
	onset = Onsets.kr(chain, threshold);
	amp = Amplitude.kr(input);
    loudness = Loudness.kr(chain);
	mfcc = MFCC.kr(chain, numcoeffs);
	if (beattrack, {
		track = AutoTrack.kr(input);
	});
	Out.kr(out, [amp, loudness, onset] ++ track ++ mfcc);
}).add;

SynthDef(\buf, {|out, aout=0, in=0, bufnum|
	var input, amp, chain, onset, loudness, mfcc, track;
	var trackb, trackh, trackq, tempo;
	input = PlayBuf.ar(1, bufnum, BufRateScale.kr(bufnum), loop: 1);
	chain = FFT(LocalBuf(window), input);
	onset = Onsets.kr(chain, threshold);
	amp = Amplitude.kr(input);
    loudness = Loudness.kr(chain);
	mfcc = MFCC.kr(chain, numcoeffs);
	track = [];
	if (beattrack, {
		track = AutoTrack.kr(input);
	});
	Out.kr(out, [amp, loudness, onset] ++ track ++ mfcc);
	Out.ar(aout, Splay.ar(input));
}).add;
)

(
~cleanup1 = {
	if (~buffers.notNil) {
		~buffers.do { |buf|
			buf.free;
		};
		~buffers = nil;
	};
};
~cleanup2 = {
	if (~synths.notNil) {
		~synths.do { |synth|
			synth.free;
		};
		~synths = nil;
	};
	if (~buses.notNil) {
		~buses.do { |bus|
			bus.free;
		};
		~buses = nil;
	};
};
~file = "/Users/kasparsj/Music/ivars/tema1.wav";
// ~file = "/Users/kasparsj/Music/ivars/tema1_2.wav";
// ~file = "/Users/kasparsj/Music/ivars/tema1_1.wav";
// ~file = "/Users/kasparsj/Music/SuperCollider Recordings/SC_220411_163046.wav";
if (~file.isNil, {
	~numChannels = 1;
}, {
	~cleanup1.value;
	~buffers = List();
	Buffer.read(s, ~file, action: { |buffer|
		~numChannels = buffer.numChannels;
		~numChannels.do { |i|
			~buffers.add(Buffer.readChannel(Server.default, ~file, channels: [i]));
		};
	});
});
)

(
~cleanup2.value;
~synths = List();
~buses = List();
~numChannels.do { |i|
	var synthName = if (~file.isNil, { \soundin }, { \buf });
	var bufnum = if (~file.isNil, { nil }, { ~buffers[i].bufnum; });
	~buses.add(Bus.control(Server.default, ~busChannels));
	~synths.add(Synth(synthName, [out: ~buses.last.index, in: i, bufnum: bufnum]));
};
)

(
var rate = 1/64;
Pdef(\hd_event, Ppar(
	(0..6).collect { |i|
		Pbind(
			\type, \hd,
			\noteId, (i+1),
			\from, (i*2),
			\delta, rate,
			\dur, rate*8,
			\emit, 0,
			\amp, 0,
			\behaviour, [\MIRROR, \SMOOTH_CHANGES],
			// \length, (amp * 100).max(1).pow(3).asInteger,
			\length, Pfunc { |event|
				var amp = ~buses[i].getSynchronous();
				((amp.exp.pow(5)*60)-60);
			},
			\speed, 0,
			//\color, [(mfcc[0] * 255).asInteger, 64, (mfcc[1] * 127).asInteger],
			//\color, (mfcc * 255).collect(_.asInteger),
			\color, 0xff0000,
			// \colorIndex, (0..6)*32,
		);
	};
)).play;
)

Pdef(\hd_event).stop;

~cleanup2.value;
~cleanup1.value;

~hd.stopAll();
