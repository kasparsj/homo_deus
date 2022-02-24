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

~hdP.restartWind = false;
~hdP.cricketsBus = Bus.control(s, 1);
~hdP.cricketsBus.set(1.0);
~hdP.p1 = nil;
~hdP.c1 = Pmono(\cricket1,
	\amp, ~hdP.cricketsBus.asMap
);
~hdP.c2 = Pmono(\cricket2,
	\amp, ~hdP.cricketsBus.asMap
);
~hdP.c31 = Pmono(\cricket3,
	\amp, ~hdP.cricketsBus.asMap,
	\trigFreq, 2
);
~hdP.c32 = Pmono(\cricket3,
	\amp, ~hdP.cricketsBus.asMap,
	\trigFreq, 0.5
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
		\dur, Pfunc{ |event|
			if (event[\type] == \hd) {
				if (self.restartWind or: { Ndef(\x).isPlaying.not }) {
					self.restartWind = false;
					Ndef(\x).set(\change, rrand(0.001, 0.01), \rq, rrand(0.1, 1.5));
					Ndef(\x).play(fadeTime: 10);
				};
				if (self.p1.isNil or: { self.p1.isPlaying.not }) {
					self.p1 = self.c1.play;
					self.c2.play;
					self.c31.play;
					self.c32.play;
				};
				self.cricketsBus.set(1.0);
				1 + 4.rand;
			} {
				var silence = 5 + 10.rand;
				self.cricketsBus.set(0);
				Ndef(\x).stop(silence-1, true);
				self.restartWind = true;
				silence;
			};
		},
	]);
	Pdef(\hd_crickets, Pbind(*pArgs));
};
)