(

var notes, notes2, notes_rev, notes_rev2;
var pulseStream, melodyStream, squeekStream, echoStream, ticksStream, glassStream;

// pulseStream = Pfunc{ ~phi8P.noise([0], 1)[0] }.asStream;
//
// notes = ~phi8P.leaf(\medium, [1, 2]);
// melodyStream = Pseq(notes, inf).asStream;
//
// notes = ~phi8P.arc.copy;
// squeekStream = Pseq(notes, inf).asStream;
//
// notes = ~phi8P.two([3]);
// echoStream = Pseq(notes, inf).asStream;
//
// ticksStream = Pfunc{ ~phi8P.noise(nil, 1)[0] }.asStream;
//
// notes = ~phi8P.compass([2, 3]);
// glassStream = Pseq(notes, inf).asStream;

~hdGameMusicNoClock = ~gameMusicNoClock;
~hdGameMusicWithClock = ~gameMusicWithClock.mapToEvent((\type: \hd), [
	\pulse, { |event|
		/*[
			\model, 2,
			\speed, 0.0,
			\length, 1,
			\order, 1,
		];*/
	},
	\melody, { |event|
		/*if (event[\midinote] != \ && event[\midinote].class != Rest) {
			[
				\model, 0,
				\speed, 1.0,
				\length, 10,
			];
			//var c = [1, 2][(event[\midinote] >= 70).asInteger];
			//[\midinote, ~phi8P.mask(melodyStream.next, [c])];
		} {
			//[\midinote, \];
		};*/
	},
	\squeek1, { |event|
		//[\midinote, [squeekStream.next, squeekStream.next, squeekStream.next], \sustain, 0.75, \strum, 0.05];
		[
			\model, 1,
			\speed, 3.5,
			\length, 70,
			\color, 0,
		];
	},
	\squeek2, { |event|
		//[\midinote, [squeekStream.next, squeekStream.next, squeekStream.next], \sustain, 0.75, \strum, 0.05];
		[
			\model, 1,
			\speed, 3.5,
			\length, 70,
			\color, 0,
		];
	},
	\echo1, { |event|
		/*[
			\model, 2,
			\speed, 4.0,
			\length, 10,
			\color, 0,
		];*/
	},
	\echo2, { |event|
		/*[
			\model, 2,
			\speed, 4.0,
			\length, 10,
			\color, 0,
		];*/
	},
	\echo3, { |event|
		/*[
			\model, 2,
			\speed, 4.0,
			\length, 10,
			\color, 0,
		];*/
	},
	\ticks, { |event|
		/*[
			\model, 3,
			\speed, 4.0,
			\length, 5,
			\color, 0,
		];*/
	},
	\glass, { |event|
		//[\midinote, ~phi8P.mask(glassStream.next, [[2, 3].choose])];
	},
]);

)
