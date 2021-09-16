(

s.latency = 0.1;
s.waitForBoot {
	"hd.scd".loadRelative;

	//~hd.net = NetAddr("homo-deus", 54321);
	//~hd.net = NetAddr("192.168.209.181", 54321);
	~hd.net = NetAddr("192.168.1.105", 54321);
	//~hd.net = NetAddr("127.0.0.1", 54321);

	~hd.createSpawn({
		~hd.createDrone(
			\model, Pfunc{ [1,2,3].choose },
			\amp, Pwhite(0.05, 0.07),
			\dur, Pwhite(10, 45, 1),
			\color, Pfunc{
				var i = [0, 2].choose;
				var col = [0, 0, 0];
				col[i] = 255;
				col;
			},
			//\behaviour, \FILL_EASE,
			//\ease, \EXPONENTIAL_INOUT,
		);
	}, Pwhite(40, 120)).play;

	~bus1 = Bus.control(s, 1);
	~bus1.set(1.0);

	~c1 = Synth(\cricket1, [\amp, ~bus1.asMap]);
	~c2 = Synth(\cricket2, [\amp, ~bus1.asMap]);
	~c31 = Synth(\cricket3, [\amp, ~bus1.asMap, \trigFreq, 2]);
	~c32 = Synth(\cricket3, [\amp, ~bus1.asMap, \trigFreq, 0.5]);

	Pbind(
		\type, Pfunc{ [\hd, \pause].wchoose([0.9, 0.1]); },
		\amp, 0,
		\model, 0,
		//\colorChange, Pfunc{ [nil, 0, 1].wchoose([0.5, 0.4, 0.1]) },
		\ease, \EXPONENTIAL_INOUT,
		\behaviour, \FILL_EASE,
		\brightness, Pwhite(127, 255),
		\color, Pfunc{ [256.rand, 256.rand, 256.rand] },
		\dur, Pfunc{ |event|
			if (event[\type] == \hd, {
				~bus1.set(1.0);
				1 + 4.rand;
			}, {
				~bus1.set(0);
				5 + 10.rand;
			});
		},
	).play;
};

)

/*(
Ppar([
	~hdGameMusicNoClock.asPattern,
	~hdGameMusicWithClock.asPattern(nil, 1),
]).play;
)*/
