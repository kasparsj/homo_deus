(
~threshold = 1.0;
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
	var input, amp, chain, onset, loudness, mfcc, trig;
    input = SoundIn.ar(in);
	chain = FFT(LocalBuf(1024), input);
	onset = Onsets.kr(chain, ~threshold);
	amp = Amplitude.kr(input);
    loudness = Loudness.kr(chain);
	mfcc = MFCC.kr(chain, 3);
    trig = Impulse.kr(rate);
    SendReply.kr(trig, '/visuals', [in, onset, amp, loudness] ++ mfcc);
//	Out.ar(out, input);
}).add;

SynthDef(\buf, {|out=0, in=0, rate=60, bufnum|
	var input, amp, chain, onset, loudness, mfcc, trig;
	input = PlayBuf.ar(1, bufnum, BufRateScale.kr(bufnum), loop: 1);
	chain = FFT(LocalBuf(1024), input);
	onset = Onsets.kr(chain, ~threshold);
	amp = Amplitude.kr(input);
    loudness = Loudness.kr(chain);
	mfcc = MFCC.kr(chain);
    trig = Impulse.kr(rate);
    SendReply.kr(trig, '/visuals', [in, onset, amp, loudness] ++ mfcc);
	Out.ar(out, Splay.ar(input));
}).add;
)

(
OSCdef(\visuals, {|msg|
    var data = msg[3..];
	var in = data[0];
	var onset = data[1];
	var amp = data[2];
	var loudness = data[3];
	var mfcc = data[4..];
	if (onset == 1, {
		(
			type: \hd,
			from: in.asInteger,
			length: loudness / 4,
			//color: (mfcc * 255).collect(_.asInteger),
			color: [(mfcc[0] * 255).asInteger, 64, (mfcc[1] * 127).asInteger],
			//life: (amp / 100).asInteger,
			speed: amp * 40,
			life: loudness / 5,
			behaviour: [\EXPIRE_IMMEDIATE, \MIRROR],
		).play;
	});
}, '/visuals');
)

(
var synthName = if (~file.isNil, { \soundin }, { \buf });
~numChannels.do {|i|
	var bufnum = if (~file.isNil, { nil }, { ~buffers[i].bufnum; });
	~synths = ~synths.add(Synth(synthName, [in: i, rate: 120, bufnum: bufnum]));
};
)

(
~synths.do { |synth|
	synth.free;
}
)