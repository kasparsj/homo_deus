(

// Quarks.install("wslib");
// Quarks.install("Bjorklund");
// Quarks.install("Feedback");
// Quarks.install("UGenPatterns");
// Quarks.install("JITLibExtensions");
// Quarks.install("SuperDirt");

s.latency = 0.1;
s.waitForBoot {
	"../hd.scd".loadRelative;
	"../patterns.scd".loadRelative;
	"../master.scd".loadRelative;
	if (~dirt.notNil, {
		"../superdirt.scd".loadRelative;
		"../circuit.scd".loadRelative;
	});

	// does not work with hotspot (works with router)
	// ~hd.net = NetAddr("homo-deus", 54321);
	// ~hd.net = NetAddr("127.0.0.1", 54321);
	~hd.net = NetAddr("192.168.1.105", 54321); // esp32-6E0B08
	//~hd.net.disconnect;

	~playAuto = false;
	~soundInterval = 0;
	~lastWindChange = 0;

	if (~playAuto) {
		~hdP.startSpawn;
		~hdP.startCrickets([
\type, Pfunc{ [\hd, \pause].wchoose([0.97, 0.03]); },
			\colorChange, Pfunc{
				if (3.rand == 1, {
					0;
				}, { {nil} });
			},
			//\color, Pfunc{ [256.rand, 256.rand, 256.rand] },
			\color, Pfunc{ [0, 128 + 128.rand, 0] },
			\behaviour, Pfunc{
				var be = [\FILL_EASE];
				//if (5.rand == 1, {
				if (3.rand == 1, {
					be = be.add(\RANDOM_COLOR);
				});
				be;
			},
			\brightness, Pfunc{ |event|
				if (event[\behaviour].indexOf(\RANDOM_COLOR) != nil) {
					rrand(32, 192);
				} {
					rrand(96, 255);
				}

			},
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
					if ((~lastWindChange + 5) < SystemClock.seconds) {
						~hdP.startWind;
						~lastWindChange = SystemClock.seconds;
						(
							type: \hd,
							\color: [0, 87, 183],
							\model: 6,
							\speed: ~hdP.wind.nodeMap.at(\change)*10,
							\linked: 0,
							\sustain: 1,
							\length: 2+(20.rand),
						).play;
					};
				} {
					~hdP.stopWind(event[\dur]-1);
				};
				true;
			},
		]);

		if (~soundInterval > 0) {
			~isOn = true;
			~hdMaster.proxy.xset(\vol, 1.0);
			SystemClock.sched(110, {
				var func = {
					~hdMaster.proxy.fadeTime = 5;
					if (~isOn) {
						~hdMaster.proxy.xset(\vol, 0.0);
						~isOn = false;
						("off " + SystemClock.seconds).postln;
					} {
						~hdMaster.proxy.xset(\vol, 1.0);
						~isOn = true;
						("on" + SystemClock.seconds).postln;
					};
					~soundInterval;
				};
				func.value;
				SystemClock.sched(~soundInterval, func);
				nil;
			});
		};
	};

};

)

~hd.command(\debug_all);
~hd.command(\debug_intersection);
~hd.command(\debug_connection);

// (
// ~hdP.spawn.stop;
// ~hdP.stopCrickets(10);
// ~hdP.stopWind(10);
// )
