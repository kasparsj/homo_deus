(
s.waitForBoot {

	SynthDef(\SineDrone, {
		arg out = 0, freq = 50, gate = 1.0, phase = 0, dur = 180, pan = 0, amp = 1.0;
		var env, noise, sig;
		env = EnvGen.ar(Env([0, 0.01, 1.0, 0], [0.01, 0.98, 0.01]), gate, timeScale: dur*1.25, doneAction: 2);
		sig = SinOsc.ar(freq * [1, 2, 3, 4], phase).sum * SinOsc.ar(1/11).range(0.1, 1);
		sig = sig * env;
		sig = Pan2.ar(sig, pan) * amp;
		Out.ar(out, sig);
	}).add;

	SynthDef(\SawDrone, {
		arg out = 0, freq = 50, gate = 1.0, phase = 0, dur = 180, pan = 0, amp = 1.0;
		var env, noise, sig;
		env = EnvGen.ar(Env([0, 0.01, 1.0, 0], [0.01, 0.98, 0.01]), gate, timeScale: dur*1.25, doneAction: 2);
		sig = Saw.ar(freq * [1, 2, 3, 4], phase).sum * SinOsc.ar(1/11).range(0.1, 1);
		sig = sig * env;
		sig = Pan2.ar(sig, pan) * amp;
		Out.ar(out, sig);
	}).add;

	SynthDef(\PulseDrone, {
		arg out = 0, freq = 50, gate = 1.0, phase = 0, dur = 180, pan = 0, amp = 1.0;
		var env, noise, sig;
		env = EnvGen.ar(Env([0, 0.01, 1.0, 0], [0.01, 0.98, 0.01]), gate, timeScale: dur*1.25, doneAction: 2);
		sig = Splay.ar(Pulse.ar(freq * [1, 1.5, 2, 2.5, 3], SinOsc.kr(2, phase, 0.5)).sum * SinOsc.ar(1/11).range(0.1, 1));
		sig = sig * env;
		sig = Pan2.ar(sig, pan) * amp;
		Out.ar(out, sig);
	}).add;

	s.sync;

	OSCdef(\spawn, { |msg, time, addr|
		var which = (0..2).choose;
		var instruments = [\SineDrone, \SawDrone, \PulseDrone];
		var dur = rrand((30/1.25), (360/1.25)).asInteger;
		var freq = (51..(51+rrand(1, 3))).scramble;
		var pan = [-1, 1].scramble;
		Pbind(
			\instrument, instruments[which],
			\freq, freq,
			\dur, Pseq([msg[1]], 1),
			\pan, pan,
			\phase, Pwhite(0, 2pi, 1),
			\out, 0,
			\amp, 0.3,
		).play;
	}, '/hd/spawn');

	ProxyChain.add(*[
		\in12, \mix -> {
			SoundIn.ar([0, 1]);
		},
		\lpf, \filter -> { |in, freq = 200, mul = 4|
			LPF.ar(in, freq, mul);
		},
		\moogVcf, \filter -> { |in, freq = 200, gain = 1|
			MoogFF.ar(in, freq, gain);
		},
		\freqShift, \filter -> { |in, freq = -200|
			FreqShift.ar(in, freq);
		},
		// \feedback, \filter -> { |in, maxdelaytime=4, delaytime=1|
		// 	FbC({ |fb| FreqShift.ar(in + fb, 200).softclip * 0.8}, maxdelaytime, delaytime);
		// },
		\delay, \filter -> { |in, maxdelaytime=0.25, delaytime=0.25|
			DelayN.ar(in, maxdelaytime, delaytime);
		},
		\comb, \filter -> { |in, maxdelaytime=0.1, delaytime=0.025, decaytime=2|
			CombN.ar(in, maxdelaytime, delaytime, decaytime);
		},
		\reverb, \filter -> { |in, preDelay=0.048, combDelay=0.1, combDecay=4, allpassDelay=0.050, allpassDecay=1|
			var z, y;
			// reverb predelay time :
			z = DelayN.ar(in, preDelay);
			// 7 length modulated comb delays in parallel :
			y = Mix.ar(Array.fill(7,{ CombL.ar(z, combDelay, LFNoise1.kr(combDelay.rand, 0.04, 0.05), combDecay) }));
			// two parallel chains of 4 allpass delays (8 total) :
			4.do({ y = AllpassN.ar(y, allpassDelay, [allpassDelay.rand, allpassDelay.rand], allpassDecay) });
			// add original sound to reverb and play it :
			y;
		},
		\gverb, \filter -> { |in, roomsize=50, revtime=3|
			GVerb.ar(in, roomsize, revtime);
		},
		\tanh, \filter -> { |in|
			in.tanh;
		},
		\limiter, \filter -> { |in, drive=1, ampLimit=1|
			Limiter.ar(in * drive, ampLimit);
		},
	]);

	~filterKeys = [\lpf, \moogVcf, \delay, \comb, \reverb, \gverb, \tanh, \limiter];

	MasterFX.clear('localhost');
	~phi8Master = MasterFX(Server.default, 2, ~filterKeys);
	~phi8Master.add(\lpf, 0.2);
	~phi8Master.add(\moogVcf, 0.4);
	~phi8Master.add(\gverb, 0.1);
	~phi8Master.add(\tanh, 1);
	~phi8Master.add(\limiter, 1);
}
)