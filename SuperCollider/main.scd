(

s.latency = 0.1;
s.waitForBoot {
	"hd.scd".loadRelative;
	"patterns.scd".loadRelative;
	"superdirt.scd".loadRelative;

	//~hd.net = NetAddr("homo-deus", 54321);
	//~hd.net = NetAddr("192.168.209.181", 54321);
	//~hd.net = NetAddr("192.168.1.105", 54321);
	~hd.net = NetAddr("127.0.0.1", 54321);

	Pspawn(Pbind(
		\pattern, Pfunc {
			var which = 2.rand;
			switch (which,
				0, {
					~hdP.drone(
						\instrument, [\SineDrone, \SineDrone, \PulseDrone].choose,
						\freq, (51..53).scramble,
						\pan, [-1, 1].scramble,
						\amp, 0.1,
						\dur, Pwhite(15, 30, 1),
					);
				},
				1, {
					~hdP.harpsi(
						\root, ([-2, -1, 1] * 12).choose,
						\degree, [
							Pseries({ rrand(0, 7) }, { ([1,2]).choose }, 7),
							Pseries({ rrand(0, 7) }, { ([1,2]).choose }, 7),
							Pseries({ rrand(7, 14) }, { ([1,2]).choose }, 7)
						].choose,
						//\scale, Scale.at(Scale.names.choose),
						\scale, Scale.minor,
						\dur, ((1..8)/4).choose,
						\amp, (((1..8)/8)+1).choose,
					);
				},
			);
		},
		\delta, Pwhite(45, 90),
		\method, \par,
	)).play;

	~c1 = Synth(\cricket1, [\amp, ~hdP.cricketsBus.asMap]);
	~c2 = Synth(\cricket2, [\amp, ~hdP.cricketsBus.asMap]);
	~c31 = Synth(\cricket3, [\amp, ~hdP.cricketsBus.asMap, \trigFreq, 2]);
	~c32 = Synth(\cricket3, [\amp, ~hdP.cricketsBus.asMap, \trigFreq, 0.5]);

	Ndef(\x).clear;
	Ndef(\x).fadeTime = 10;
	Ndef(\x, { |change=0.01, rq=0.8|
		var freq1=36, freq2=75, freq3=157, freq4=329, freq5=688, freq6=1440, freq7 = 3013,
		freq8 = 6303, freq9=13184, freq10=18000;
		var sig = [PinkNoise.ar(1), PinkNoise.ar(1)];
		sig = BPeakEQ.ar(sig, freq1, rq, SinOsc.kr(change, 2pi.rand, 48));
		sig = BPeakEQ.ar(sig, freq2, rq, SinOsc.kr(change, 2pi.rand, 48));
		sig = BPeakEQ.ar(sig, freq3, rq, SinOsc.kr(change, 2pi.rand, 48));
		sig = BPeakEQ.ar(sig, freq4, rq, SinOsc.kr(change, 2pi.rand, 48));
		sig = BPeakEQ.ar(sig, freq5, rq, SinOsc.kr(change, 2pi.rand, 48));
		sig = BPeakEQ.ar(sig, freq6, rq, SinOsc.kr(change, 2pi.rand, 48));
		sig = BPeakEQ.ar(sig, freq7, rq, SinOsc.kr(change, 2pi.rand, 48));
		sig = BPeakEQ.ar(sig, freq8, rq, SinOsc.kr(change, 2pi.rand, 48));
		sig = BPeakEQ.ar(sig, freq9, rq, SinOsc.kr(change, 2pi.rand, 48));
		sig = BPeakEQ.ar(sig, freq10, rq, SinOsc.kr(change, 2pi.rand, 48));
		sig = Limiter.ar(sig * 0.1, 1, 0.01);
		sig = sig * 0.1;
	}).play;

	~hdP.crickets(
		\type, Pfunc{ [\hd, \pause].wchoose([0.98, 0.02]); },
	).play;
};

)

/*(
Ppar([
~hdGameMusicNoClock.asPattern,
~hdGameMusicWithClock.asPattern(nil, 1),
]).play;
)*/
