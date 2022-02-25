(

s.latency = 0.1;
s.waitForBoot {
	"hd.scd".loadRelative;
	"patterns.scd".loadRelative;
	"superdirt.scd".loadRelative;

	//~hd.net = NetAddr("homo-deus", 54321);
	//~hd.net = NetAddr("192.168.209.181", 54321);
	~hd.net = NetAddr("192.168.0.105", 54321);
	// ~hd.net = NetAddr("127.0.0.1", 54321);

	~playAuto = true;
	if (~playAuto) {
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

		~hdP.crickets(
			\type, Pfunc{ [\hd, \pause].wchoose([0.98, 0.02]); },
			\dur, Pfunc{ |event|
				if (event[\type] == \hd) {
					// cricket animation length
					1 + 4.rand;
				} {
					var silence = 5 + 10.rand;
					"pause for %".format(silence).postln;
					silence;
				};
			},
			\wind, Pfunc{ |event|
				if (event[\type] == \hd) {
					~hdP.startWind();
				} {
					~hdP.stopWind(event[\dur]-1);
				};
				true;
			},
		).play;
	};
};

)

/*(
Ppar([
~hdGameMusicNoClock.asPattern,
~hdGameMusicWithClock.asPattern(nil, 1),
]).play;
)*/
