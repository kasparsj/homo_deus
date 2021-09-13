(
SynthDef(\SineDrone, {
	arg out = 0, freq = 50, gate = 1.0, phase = 0, sustain = 180, release = 0.2, pan = 0, amp = 1.0, modFreq = 1/11;
	var env, noise, sig;
	env = EnvGen.ar(Env([0, 1.0, 1.0, 0], [0.01, sustain, release]), gate, doneAction: 2);
	sig = SinOsc.ar(freq * [1, 2, 3, 4], phase).sum * SinOsc.ar(modFreq).range(0.1, 1);
	sig = sig * env;
	sig = Pan2.ar(sig, pan) * amp;
	Out.ar(out, sig);
}).add;

SynthDef(\SawDrone, {
	arg out = 0, freq = 50, gate = 1.0, phase = 0, sustain = 180, release = 0.2, pan = 0, amp = 1.0, modFreq = 1/11;
	var env, noise, sig;
	env = EnvGen.ar(Env([0, 1.0, 1.0, 0], [0.01, sustain, release]), gate, doneAction: 2);
	sig = Saw.ar(freq * [1, 2, 3, 4], phase).sum * SinOsc.ar(modFreq).range(0.1, 1);
	sig = sig * env;
	sig = Pan2.ar(sig, pan) * amp;
	Out.ar(out, sig);
}).add;

SynthDef(\PulseDrone, {
	arg out = 0, freq = 50, gate = 1.0, phase = 0, sustain = 180, release = 0.2, pan = 0, amp = 1.0, modFreq = 1/11;
	var env, noise, sig;
	env = EnvGen.ar(Env([0, 1.0, 1.0, 0], [0.01, sustain, release]), gate, doneAction: 2);
	sig = Splay.ar(Pulse.ar(freq * [1, 1.5, 2, 2.5, 3], SinOsc.kr(2, phase, 0.5)).sum * SinOsc.ar(modFreq).range(0.1, 1));
	sig = sig * env;
	sig = Pan2.ar(sig, pan) * amp;
	Out.ar(out, sig);
}).add;
)