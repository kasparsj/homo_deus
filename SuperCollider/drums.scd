(
~threshold = 0.5;
~rate = 64;
~synths = [];
~file = "/Users/kasparsj/Music/ivars/tema1.wav";
// ~file = "/Users/kasparsj/Music/ivars/tema1_2.wav";
// ~file = "/Users/kasparsj/Music/ivars/tema1_1.wav";
// ~file = "/Users/kasparsj/Music/SuperCollider Recordings/SC_220411_163046.wav";
if (~file.isNil, {
	~numChannels = 1;
}, {
	~buffers = [];
	Buffer.read(s, ~file, action: { |buffer|
		~numChannels = buffer.numChannels;
		~numChannels.do { |i|
			~buffers = ~buffers.add(Buffer.readChannel(s, ~file, channels: [i]));
		};
	});
});
)

(
SynthDef(\soundin, {|out=0, in=0, rate=60|
	var input, amp, chain, onset, loudness, mfcc, track, trig;
	var trackb, trackh, trackq, tempo;
    input = SoundIn.ar(in);
	chain = FFT(LocalBuf(1024), input);
	onset = Onsets.kr(chain, ~threshold);
	amp = Amplitude.kr(input);
    loudness = Loudness.kr(chain);
	mfcc = MFCC.kr(chain, 3);
	track = [];//track = AutoTrack.kr(input);
    trig = Impulse.kr(rate);
    SendReply.kr(trig, '/visuals', [in, onset, amp, loudness] ++ track ++ mfcc);
//	Out.ar(out, input);
}).add;

SynthDef(\buf, {|out=0, in=0, rate=60, bufnum|
	var input, amp, chain, onset, loudness, mfcc, track, trig;
	var trackb, trackh, trackq, tempo;
	input = PlayBuf.ar(1, bufnum, BufRateScale.kr(bufnum), loop: 1);
	chain = FFT(LocalBuf(1024), input);
	onset = Onsets.kr(chain, ~threshold);
	amp = Amplitude.kr(input);
    loudness = Loudness.kr(chain);
	mfcc = MFCC.kr(chain, 3);
	track = [];//track = AutoTrack.kr(input);
    trig = Impulse.kr(rate);
    SendReply.kr(trig, '/channel_data', [in, onset, amp, loudness] ++ track ++ mfcc);
	Out.ar(out, Splay.ar(input));
}).add;
)

~hd.stopAll();

(
OSCdef(\channel_rec, {|msg|
    var data = msg[3..];
	var in = data[0];
	var onset = data[1];
	var amp = data[2];
	var loudness = data[3];
	var mfcc = data[4..];
	if (onset == 1, {
		/*(
			type: \hd,
			from: in.asInteger,
			emit: 0,
			length: loudness / 4,
			//color: (mfcc * 255).collect(_.asInteger),
			//color: [(mfcc[0] * 255).asInteger, 64, (mfcc[1] * 127).asInteger],
			colorIndex: in.asInteger * 32,
			//life: (amp / 100).asInteger,
			dur: amp * 7,
			speed: amp * 10,
			life: loudness / 5,
			behaviour: [\EXPIRE_IMMEDIATE, \MIRROR],
		).play;*/
	});
	// not working very well
	(
		type: \hd,
		from: (in*2).asInteger,
		// length: (amp * 100).max(1).pow(3).asInteger,
		length: ((amp.exp.pow(5)*60)-60).asInteger,
		speed: 0,
		//color: [(mfcc[0] * 255).asInteger, 64, (mfcc[1] * 127).asInteger],
		//color: (mfcc * 255).collect(_.asInteger),
		//color: 0xffffff,
		color: 0xff0000,
		// colorIndex: in.asInteger * 32,
		dur: 1,
		behaviour: [\MIRROR, \SMOOTH_CHANGES],
		noteId: (1+in).asInteger,
	).play;
}, '/channel_data');
)

(
var synthName = if (~file.isNil, { \soundin }, { \buf });
~numChannels.do {|i|
	var bufnum = if (~file.isNil, { nil }, { ~buffers[i].bufnum; });
	~synths = ~synths.add(Synth(synthName, [in: i, rate: ~rate, bufnum: bufnum]));
};
)

(
~synths.do { |synth|
	synth.free;
}
)