(

SynthDef(\IkedaClick, { arg out = 0, t_trig = 0, centerFreq = 15000, rq = 0.9, mul = 25, pan = 0, level = 1, amp = 1.0;
	var env, noise, sig;
	env = EnvGen.ar(Env.perc(0.001, 0.00001 * LFNoise1.kr(4).abs, 1, -4), t_trig, doneAction: Done.freeSelf);
	noise = PinkNoise.ar(env);
	sig = BPF.ar(noise, centerFreq.max(1), rq, amp).fold(-1, 1);
	Out.ar(out, Pan2.ar(sig, pan, level.curvelin(0, 1, 0, 1, log(10))) * amp);
}).add;

SynthDef(\IkedaBass, { arg out = 0, t_trig = 0, pan = 0, level = 1, amp = 1.0;
	var env, sin, sig;
	env = EnvGen.ar(Env([0, 0.5, 0.4, 0], [0, 0.2, 0.01], -5), t_trig, doneAction: Done.freeSelf);
	sin = SinOsc.ar(0, (Sweep.ar(t_trig, 2pi * [52.8, 740]) + (pi/3)).wrap(-pi, pi), [2, 0.05]).mean.tanh;
	sig = sin * env;
	Out.ar(out, Pan2.ar(sig, pan, level.curvelin(0, 1, 0, 1, log(10))) * amp);
}).add;

)

/*(
Pbind(
	\instrument, \IkedaClick,
	\amp, Pbjorklund(5, 8, inf),
	\dur, 1/8,
).play;
)*/