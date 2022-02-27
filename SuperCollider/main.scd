(

s.latency = 0.1;
s.waitForBoot {
	"hd.scd".loadRelative;
	"patterns.scd".loadRelative;
	"superdirt.scd".loadRelative;

	//~hd.net = NetAddr("homo-deus", 54321);
	//~hd.net = NetAddr("192.168.209.181", 54321);
	//~hd.net = NetAddr("192.168.0.105", 54321);
	~hd.net = NetAddr("127.0.0.1", 54321);

	~playAuto = false;
	if (~playAuto) {
		~hdP.startSpawn;
		~hdP.startCrickets(
			\type, Pfunc{ [\hd, \pause].wchoose([0.98, 0.02]); },
			\colorChange, Pfunc{
				if (3.rand == 1, {
					0;
				}, { {nil} });
			},
			\brightness, Pwhite(127, 255, inf),
			\color, Pfunc{ [256.rand, 256.rand, 256.rand] },
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
					~hdP.startWind;
				} {
					~hdP.stopWind(event[\dur]-1);
				};
				true;
			},
		);
	};
};

)

(
~hdP.spawn.stop;
~hdP.stopCrickets(10);
~hdP.stopWind(10);
)

/*(
Ppar([
~hdGameMusicNoClock.asPattern,
~hdGameMusicWithClock.asPattern(nil, 1),
]).play;
)*/
