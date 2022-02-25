(
~hdP = ~hdP ? ();

~hdP.drone = {|self ... args|
	var pArgs = [
		\type, \hd_note,
		\phase, [2pi.rand, 2pi.rand, 2pi.rand],
		\model, Pfunc{ [1, 2, 4, 5].choose },
		//\from, -1,
		\length, 100,
		\amp, 0.05,
		\color, Pfunc{
			var i = [0, 2].choose;
			var col = [0, 0, 0];
			col[i] = 255;
			col;
		},
		\modFreq, rrand(0.09, 0.5),
		\speed, Pfunc{ |event|
			event[\modFreq] * 10;
		},
	].addAll(args);
	Pbind(*pArgs);
};

~hdP.harpsi = { |self ... args|
	var pArgs = [
		\type, \hd_note,
		\model, (2..5).choose,
		\behaviour, \RENDER_SEGMENT,
		\instrument, \harpsi,
		\ffreq, ((1..2) ++ (3..1)).choose,
		\release, [5,6,7,8].choose,
	].addAll(args).addAll([
		\color, Pfunc{ |event|
			var col = [127,255,0];
			if (event[\root].isNil) {
				if (event[\freq] > 250) { col = [255,255,0]; }; // ~C4
				if (event[\freq] > 500) { col = [255,255,255]; }; // ~C5
			} {
				if (event[\root] == -12) { [255,255,0] };
				if (event[\root] == 12) { [255,255,255] };
			};
			col;
		},
		\brightness, Pfunc{ |event|
			event[\amp] * 127.0;
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
	]);
	Pbind(*pArgs);
};

~hdP.cricketsBus = Bus.control(s, 1);
~hdP.cricketsBus.set(1.0);
~hdP.p1 = nil;
~hdP.c1 = Pmono(\cricket1,
	\amp, ~hdP.cricketsBus.asMap,
	\pan, PSinOsc(1/5),
	\mod, PSinOsc(1/5, 0),
);
~hdP.c2 = Pmono(\cricket2,
	\amp, ~hdP.cricketsBus.asMap,
	\pan, PSinOsc(1/7),
	\mod, PSinOsc(1/7, 2pi*1/3),
);
~hdP.c31 = Pmono(\cricket3,
	\amp, ~hdP.cricketsBus.asMap,
	\trigFreq, 2,
	\pan, PSinOsc(1/22),
	\mod, PSinOsc(1/22, 2pi*2/3),
);
~hdP.c32 = Pmono(\cricket3,
	\amp, ~hdP.cricketsBus.asMap,
	\trigFreq, 0.5,
	\pan, PSinOsc(1/22),
	\mod, PSinOsc(1/22, 2pi*2/3),
);

~hdP.crickets = { |self ... args|
	var pArgs = [
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
		\brightness, Pwhite(127, 255, inf),
		\color, Pfunc{ [256.rand, 256.rand, 256.rand] },
	].addAll(args).addAll([
		\crickets, Pfunc{ |event|
			if (event[\type] == \hd) {
				if (self.p1.isNil or: { self.p1.isPlaying.not }) {
					self.p1 = self.c1.play;
					self.c2.play;
					self.c31.play;
					self.c32.play;
				};
				self.cricketsBus.set(1.0);
			} {
				self.cricketsBus.set(0);
			};
			true;
		},

	]);
	Pdef(\hd_crickets, Pbind(*pArgs));
};

Ndef(\wind).clear;
Ndef(\wind).fadeTime = 10;
Ndef(\wind, { |change=0.01, rq=0.8|
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
});
~hdP.wind = Ndef(\wind);
~hdP.restartWind = true;
~hdP.startWind = { |self, change, rq|
	if (self.restartWind or: { self.wind.isPlaying.not }) {
		self.restartWind = false;
		self.wind.set(\change, change ? rrand(0.001, 0.01), \rq, rq ? rrand(0.1, 1.5));
		self.wind.play(fadeTime: 10);
		"wind started".postln;
	};
};
~hdP.stopWind = { |self, secs = 0|
	self.wind.stop(secs, true);
	self.restartWind = true;
	"wind stopped".postln;
};
)