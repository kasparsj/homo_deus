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
		\modFreq, rrand(0.09, 0.5),
		\speed, Pfunc{ |event|
			event[\modFreq] * 10;
		},
	].addAll(args).addAll([
		\color, Pfunc{ |event|
			// var i = [0, 2].choose;
			// var col = [0, 0, 0];
			// col[i] = 255;
			// col;
			switch (event[\instrument],
				\SineDrone,   { [0, 0, 255] },
				\PulseDrone, { [255, 0, 0] },
				\SawDrone, { [255, 255, 255] },
			);
		},
	]);
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
			((event[\release] + event[\dur]) * 2).asInteger;
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
~hdP.c1p = ~hdP.c2p = ~hdP.c31p = ~hdP.c32p = nil;
~hdP.c1 = Pmono(\cricket1,
	\amp, ~hdP.cricketsBus.asMap,
	\pan, PSinOsc(1/5),
	\mod, PSinOsc(1/5, 0, mul: 0.75),
);
~hdP.c2 = Pmono(\cricket2,
	\amp, ~hdP.cricketsBus.asMap,
	\pan, PSinOsc(1/7),
	\mod, PSinOsc(1/7, 2pi*1/3, mul: 0.75),
);
~hdP.c31 = Pmono(\cricket3,
	\amp, ~hdP.cricketsBus.asMap,
	\trigFreq, 2,
	\pan, PSinOsc(1/22),
	\mod, PSinOsc(1/22, 2pi*2/3, mul: 0.75),
);
~hdP.c32 = Pmono(\cricket3,
	\amp, ~hdP.cricketsBus.asMap,
	\trigFreq, 0.5,
	\pan, PSinOsc(1/22),
	\mod, PSinOsc(1/22, 2pi*2/3, mul: 0.75),
);

~hdP.startCrickets = { |self ... args|
	var pArgs = [
		\type, \hd,
		\amp, 0,
		\model, Pfunc{ [0, 2, 3].wchoose([0.75, 0.1, 0.15]) },
		\speed, Pfunc{ 0.5.exprand(10.0) },
		\ease, \EXPONENTIAL_INOUT,
	].addAll(args).addAll([
		\crickets, Pfunc{ |event|
			if (event[\type] == \hd) {
				self.unmuteCrickets;
			} {
				self.muteCrickets;
			};
			true;
		},

	]);
	if (self.cricketsP != nil) {
		self.cricketsP.stop;
	};
	self.cricketsP = Pdef(\hd_crickets, Pbind(*pArgs)).play;
};
~hdP.muteCrickets = { |self|
	self.cricketsBus.set(0);
};
~hdP.unmuteCrickets = { |self|
	if (self.c1p.isNil or: { self.c1p.isPlaying.not }) {
		self.c1p = self.c1.play;
		self.c2p = self.c2.play;
		self.c31p = self.c31.play;
		self.c32p = self.c32.play;
	};
	self.cricketsBus.set(1.0);
};
~hdP.stopCrickets = { |self, fadeTime|
	var stopAndMute = {
		self.c1p.stop;
		self.c2p.stop;
		self.c31p.stop;
		self.c32p.stop;
		self.muteCrickets;
	};
	var currentValue = self.cricketsBus.getSynchronous;
	var numSteps = 10;
	var deltaValue = currentValue / 10;
	fadeTime = fadeTime ? 0;
	self.cricketsP.stop;
	if (fadeTime > 0) {
		AppClock.sched(fadeTime / numSteps, {
			if (currentValue > 0) {
				currentValue = currentValue - deltaValue;
				self.cricketsBus.set(currentValue);
				fadeTime / numSteps;
			} {
				stopAndMute.value;
				nil;
			}
		});
	} {
		stopAndMute.value;
	};
};

Ndef(\wind).clear;
Ndef(\wind).fadeTime = 10;
Ndef(\wind, { |change=0.01, rq=0.8, amp = 0.1|
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
	sig = sig * amp;
});
~hdP.wind = Ndef(\wind);
~hdP.restartWind = true;
~hdP.startWind = { |self, change, rq, gain|
	change = change ? change ? rrand(0.001, 0.01);
	rq = rq ? rq ? rrand(0.1, 1.5);
	gain = gain ? 1.0;
	self.wind.xset(\change, change, \rq, rq, \amp, gain * 0.1);
	if (self.restartWind or: { self.wind.isPlaying.not }) {
		self.restartWind = false;
		self.wind.play(fadeTime: 10);
		"wind started % % %".format(change, rq, gain).postln;
	} {
		"wind changed % % %".format(change, rq, gain).postln;
	};
};
~hdP.stopWind = { |self, fadeTime|
	fadeTime = fadeTime ? 0;
	self.wind.stop(fadeTime, true);
	AppClock.sched(fadeTime, {
		self.restartWind = true;
		nil;
	});
	"wind stopping in %".format(fadeTime).postln;
};

~hdP.spawn = nil;
~hdP.startSpawn = { |self|
	self.spawn = Pspawn(Pbind(
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
						\scale, Pstutter(7, Scale.choose),
						\dur, ((1..8)/4).choose,
						\amp, (((1..8)/8)+1).choose,
					);
				},
			);
		},
		\delta, Pwhite(45, 90),
		\method, \par,
	)).play;
};
)