(

s.latency = 0.1;
s.waitForBoot {
	"hd.scd".loadRelative;

	//~hd.net = NetAddr("homo-deus", 54321);
	//~hd.net = NetAddr("192.168.209.181", 54321);
	~hd.net = NetAddr("192.168.1.105", 54321);
	//~hd.net = NetAddr("127.0.0.1", 54321);

	Pspawn(Pbind(
		\pattern, Pfunc {
			var which = 2.rand;
			switch (which,
				0, {
					var instrument = [\SineDrone, \SineDrone, \PulseDrone].choose;
					var freq = (51..53).scramble;
					var modFreq = rrand(0.09, 0.5);
					var pan = [-1, 1].scramble;
					Pbind(
						\type, \hd_note,
						\instrument, instrument,
						\freq, freq,
						\modFreq, modFreq,
						\dur, Pwhite(15, 30, 1),
						\pan, pan,
						\phase, [2pi.rand, 2pi.rand, 2pi.rand],
						\amp, 0.1,
						\model, Pfunc{ [1, 2, 4, 5].choose },
						//\from, -1,
						\speed, modFreq * 10,
						\length, 100,
						\amp, 0.05,
						\color, Pfunc{
							var i = [0, 2].choose;
							var col = [0, 0, 0];
							col[i] = 255;
							col;
						},
					);
				},
				1, {
					Pbind(
						\type, \hd_note,
						\model, (2..5).choose,
						\behaviour, \RENDER_SEGMENT,
						\instrument, \harpsi,
						\root, ([-2, -1, 1] * 12).choose,
						\scale, Scale.at(Scale.names.choose),
						//\scale, Scale.minor,
						\ffreq, ((1..2) ++ (3..1)).choose,
						\degree, [
							Pseries({ rrand(0, 7) }, { ([1,2]).choose }, 7),
							Pseries({ rrand(0, 7) }, { ([1,2]).choose }, 7),
							Pseries({ rrand(7, 14) }, { ([1,2]).choose }, 7)
						].choose,
						\release, [5,6,7,8].choose,
						\dur, ((1..8)/4).choose,
						\amp, (((1..8)/8)+1).choose,
						\color, Pfunc{ |event|
							var col = switch (event[\root],
								-24, { [127,255,0] },
								-12, { [255,255,0] },
								12, { [255,255,255] },
							);
							//event[\degree].value.postln;
							col;
						},
						\brightness, Pfunc{ |event|
							event[\amp]*127.0;
						},
						\length, Pfunc{ |event|
							(event[\release] + event[\dur]) * 2;
						},
						\speed, Pfunc{ |event|
							(event[\release] + event[\dur]) / 5.0;
						},
						\emit, Pfunc{|event|
							if (event[\model] == 2, {0}, {2});
						},
					);
				},
			);
		},
		\delta, Pwhite(45, 90),
		\method, \par,
	)).play;

	~bus1 = Bus.control(s, 1);
	~bus1.set(1.0);

	~c1 = Synth(\cricket1, [\amp, ~bus1.asMap]);
	~c2 = Synth(\cricket2, [\amp, ~bus1.asMap]);
	~c31 = Synth(\cricket3, [\amp, ~bus1.asMap, \trigFreq, 2]);
	~c32 = Synth(\cricket3, [\amp, ~bus1.asMap, \trigFreq, 0.5]);
	~restart = false;

	Pbind(
		\type, Pfunc{ [\hd, \pause].wchoose([0.98, 0.02]); },
		\amp, 0,
		\model, Pfunc{ [0, 2, 3].wchoose([0.75, 0.1, 0.15]) },
		\speed, Pfunc{ 0.5.exprand(10.0) },
		\ease, \EXPONENTIAL_INOUT,
		\behaviour, Pfunc{
			var be = [\FILL_EASE];
			if (5.rand == 1, {
				be = be.add(\RANDOM_COLOR);
			});
			be;
		},
		\colorChange, Pfunc{
			if (3.rand == 1, {
				0;
			}, { {nil} });
		},
		\brightness, Pwhite(127, 255),
		\color, Pfunc{ [256.rand, 256.rand, 256.rand] },
		\dur, Pfunc{ |event|
			if (event[\type] == \hd, {
				if (~restart, {
					~restart = false;
					Ndef(\x).set(\change, rrand(0.001, 0.01), \rq, rrand(0.1, 1.5));
					Ndef(\x).play(fadeTime: 10);
				});
				~bus1.set(1.0);
				1 + 4.rand;
			}, {
				var silence = 5 + 10.rand;
				~bus1.set(0);
				Ndef(\x).stop(silence-1, true);
				~restart = true;
				silence;
			});
		},
	).play;

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
};

)

/*(
Ppar([
	~hdGameMusicNoClock.asPattern,
	~hdGameMusicWithClock.asPattern(nil, 1),
]).play;
)*/
