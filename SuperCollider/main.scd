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
			\amp, 0.05,
			\dur, Pwhite(10, 30, 1),
			\color, [255,255,255],
			//\behaviour, \FILL_EASE,
			//\ease, \EXPONENTIAL_INOUT,
		);
	}, Pwhite(5, 60)).play;
};

)

// (
// Pseq([
// 	/*	~hd.createDrone(\model, 0, \fade, 0.02, \speed, 0.0, \order, 1, \dur, 2, \instrument, \SineDrone, \modFreq, 50/62.5, \length, 10),
// 	~hd.createDrone(\model, 0, \fade, 0.02, \speed, 0.0, \order, 1, \dur, 2, \instrument, \SineDrone, \modFreq, 50/62.5, \length, 20),
// 	~hd.createDrone(\model, 0, \fade, 0.02, \speed, 0.0, \order, 1, \dur, 2, \instrument, \SineDrone, \modFreq, 50/62.5, \length, 30),
// 	~hd.createDrone(\model, 0, \fade, 0.02, \speed, 0.0, \order, 1, \dur, 2, \instrument, \SineDrone, \modFreq, 50/62.5, \length, 40),*/
// 	~hd.createDrone(\model, 2, \from, 0, \speed, 2.0, \dur, 10, \instrument, \SineDrone),
// 	~hd.createDrone(\model, 2, \speed, 3.0, \dur, 9.5, \instrument, \SineDrone),
// 	~hd.createDrone(\model, 0, \speed, 6.0, \dur, 9.5 , \instrument, \SineDrone),
// 	~hd.createDrone(\model, 0, \speed, 9.0, \dur, 10, \instrument, \SawDrone),
// 	// ~hd.createDrone(\model, 1, \from, 0, \fade, 0.05, \speed, 1.0),
// 	// ~hd.createDrone(\model, 1, \from, 0, \fade, 0.05, \speed, 1.0),
// ], 1).play;
// )

/*(
Ppar([
	~hdGameMusicNoClock.asPattern,
	~hdGameMusicWithClock.asPattern(nil, 1),
]).play;
)*/

//~hd.command(\TOGGLE_AUTO);
//~hd.stopAll();

// (
// var baseFreq = 50;
// s.record(duration: 25);
// 4.do { |i|
// 	~hd.createDrone(\freq, baseFreq+i, \speed, 1/8, \dur, 25, \instrument, \SawDrone).play;
// }
// )