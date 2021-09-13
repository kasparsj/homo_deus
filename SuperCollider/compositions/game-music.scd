(
var pad, pulse, melody;
var echo1, echo2, squeek1, squeek2, echo3, glass, ticks;
var clock = TempoClock.new(90/60).permanent_(true);
var rbus = Bus.audio(Server.default, 2);
// I tried with Pfx, Pfxb, but it didn't work
var reverb = { Synth(\reverb, [\in, rbus]); };
CmdPeriod.doOnce {
	rbus.free;
	reverb.free;
};

//background pad using simple wavetables
pad = Pbind(
	\instrument, \wavetable,
	\dur, Pwrand([1,4,6,9,12],[0.35,0.25,0.2,0.15,0.05],inf),
	\atk, Pexprand(3,6),
	\sus, 0,
	\rel, Pexprand(5,10),
	\c0, Pexprand(1,2),
	\c1, Pexprand(1,2).neg,
	\detune, Pfunc({rrand(0.15,0.4)}!3),
	\buf, Prand(~wt_buf[0..3], inf),
	\scale, Scale.minorPentatonic,
	\degree, Pfunc({
		(-12,-10..12).scramble[0..rrand(1,3)]
	}),
	\amp, Pexprand(0.05,0.07),
	\pan, Pwhite(-0.4,0.4),
	\rout, rbus,
	\rmix, (-10).dbamp,
);

//arpeggiated bass pulse using mid/high complexity wavetables
pulse = Pbind(
	\instrument, \wavetable,
	\dur, Pseq([
		Pstutter(24,Pseq([1/4],1)),
		Prand([1,2,4,6,12],1)
	],inf),
	\atk, 0.001,
	\sus, 0,
	\rel, Pexprand(0.4,1),
	\c0, 0,
	\c1, Pwhite(5,10).neg,
	\detune, 0.3,
	\buf, Prand(~wt_buf[4..9], inf),
	\scale, Scale.minorPentatonic,
	\degree, Pseq([Prand([-15,-10,-5],24), Pseq([\],1)],inf)
	+ Pstutter(25,Pwrand([0,2,-1],[0.78,0.1,0.12],inf)),
	\amp, Pseq([Pgeom(0.45,-1.dbamp,25)],inf),
	\pan, Pwhite(0.01,0.3) * Pseq([1,-1],inf),
	\rout, rbus,
	\rmix, (-10).dbamp,
);

//minimal melody using simple wavetables
melody = Pbind(
	\instrument, \wavetable,
	\dur, Prand([
		Pseq([Prand([12,16,20]),2,1.5,0.5],1),
		Pseq([Prand([12,16,20]),1.5,1,1.5],1),
	],inf),
	\atk, 0.01,
	\sus, 0.3,
	\rel, 1.5,
	\c0, -2,
	\c1, -2,
	\detune, Pexprand(0.18,0.25),
	\buf, Pwrand([
		Pseq([~wt_buf[0]],4),
		Pseq([~wt_buf[1]],4),
		Pseq([~wt_buf[2]],4),
	],[9,3,1].normalizeSum,inf),
	\midinote, Pxrand([
		Pseq([\,67,60,Prand([58,70,\])],1),
		Pseq([\,67,58,Prand([57,63,\])],1),
		Pseq([\,70,72,Prand([65,79,\])],1)
	], inf),
	\amp, Pseq([0,0.18,0.24,0.28],inf),
	\rout, rbus,
	\rmix, (-6).dbamp,
);

echo1 = Pbind(
	\instrument, \wavetable,
	\dur,Pseq([1/8],4),
	\freq, Pstutter(4, Prand([
		Pexprand(10000,20000,1),
		Pexprand(100,200,1),
		Pexprand(1,2,1)
	],inf)),
	\detune, 100,
	\buf, Pstutter(4, Prand(~wt_buf[5..9],inf)),
	\atk, 0,
	\sus, 0,
	\rel, Pstutter(2, Pexprand(0.01,0.06)),
	\c1, exprand(8,20).neg,
	\amp, Pgeom(0.9, -6.dbamp, 4) * Pstutter(4,Pexprand(0.3,1)),
	\pan, Pwhite(-0.6,0.6),
	\rout, rbus,
	\rmix, Pwhite((-30).dbamp, (-15).dbamp),
);

echo2 = Pbind(
	\instrument, \wavetable,
	\dur, Pseq([1/4],2),
	\freq, Pstutter(2, Pexprand(1,200)),
	\detune, Pstutter(2, Pexprand(1,100)),
	\buf, Pstutter(2, Prand(~wt_buf[8..9],inf)),
	\atk, 0,
	\sus, 0,
	\rel, Pstutter(2, Pexprand(0.01,0.2)),
	\c1, -10,
	\amp, Pgeom(0.4, -3.dbamp, 2)  * Pexprand(0.4,1),
	\rout, rbus,
	\rmix, Pwhite((-30).dbamp, (-15).dbamp),
);

squeek1 = Pbind(
	\instrument, \wavetable,
	\dur, Pseq([1/2,1/4,1/4],1),
	\freq, Pstutter(6, Pexprand(1000,2000)),
	\detune, 100,
	\buf, Pstutter(6, Prand(~wt_buf[2..5],inf)),
	\atk, 0,
	\sus, Pseq([1/3,0,0],1),
	\rel, Pseq([0,Pexprand(0.01,0.3,2)],1),
	\c1, -12,
	\amp, Pseq([0.1,0.5,0.3],1),
	\rout, rbus,
	\rmix, Pwhite((-30).dbamp, (-15).dbamp),
);

squeek2 = Pbind(
	\instrument, \wavetable,
	\dur, Pseq([1/4,1/2,1/4],1),
	\freq, Pstutter(6, Pexprand(1000,2000)),
	\detune, 100,
	\buf, Pstutter(6, Prand(~wt_buf[2..5],inf)),
	\atk, 0,
	\sus, Pseq([0,1/3,0],1),
	\rel, Pseq([Pexprand(0.01,0.3,1),0,Pexprand(0.01,0.3,1)],1),
	\c1, -12,
	\amp, Pseq([0.5,0.1,0.4],1),
	\rout, rbus,
	\rmix, Pwhite((-30).dbamp, (-18).dbamp),
);

echo3 = Pbind(
	\instrument, \wavetable,
	\dur, Pseq([1/6],6),
	\freq, Pstutter(6, Pexprand(1,200)),
	\detune, Pstutter(6, Pexprand(1,100)),
	\buf, Pstutter(6, Prand(~wt_buf[8..9],inf)),
	\atk, 0,
	\sus, 0,
	\rel, Pstutter(6, Pexprand(0.01,0.1)),
	\c1, -10,
	\amp, Pgeom(0.7, -4.dbamp, 6)  * Pexprand(0.4,1),
	\rout, rbus,
	\rmix, Pwhite((-30).dbamp, (-18).dbamp),
);

glass = Pbind(
	\instrument, \wavetable,
	\dur, Prand([
		Pseq([1/2],2),
		Pseq([1],2),
		Pseq([1,1/2,1/2],1),
		Pseq([2],1),
	],1),
	\freq, Pstutter(2, Pexprand(1,200)),
	\detune, Pstutter(2, Pexprand(1,100)),
	\buf, Pstutter(2, Prand(~wt_buf[8..9],inf)),
	\atk, 0,
	\sus, 0,
	\rel, Pstutter(2, Pexprand(0.01,0.2)),
	\c1, -10,
	\amp, 0.5,
	\rout, rbus,
	\rmix, Pwhite((-20).dbamp, (-10).dbamp),
);

ticks = Pbind(
	\instrument, \wavetable,
	\dur, Prand([
		Pseq([1/16],16),
		Pseq([1/16],8)
	],1),
	\freq, Pstutter(16,Pexprand(1000,20000,inf)),
	\detune, 0,
	\buf, Pstutter(16, Prand(~wt_buf[0..9],inf)),
	\atk, 0,
	\sus, 0,
	\rel, Pexprand(0.02,0.04),
	\c1, -4,
	\amp, 0.13,
	\pan, Pseq([1,-1],inf),
	\rout, rbus,
	\rmix, (-30).dbamp,
);

~gameMusicNoClock = Composition([
	\pad, pad,
]);

~gameMusicWithClock = Composition([
	\pulse, pulse,
	\melody, melody,
	\rhythms, Composition([
		\echo1, echo1,
		\echo2, echo2,
		\squeek1, squeek1,
		\squeek2, squeek2,
		\echo3, echo3,
		\glass, glass,
		\ticks, ticks,
	]).weights_([
		40, \echo1,
		18, \echo2,
		3, \squeek1,
		3, \squeek2,
		15, \echo3,
		25, \glass,
		5, \ticks,
	]).repeats_(inf),
]).clock_(clock).quant_(1);

)