// paskatities stohastiskos procesus
// audio sprakski varetu trigerot - uguns degot.

(
SynthDef(\acsound,{|freq=440,amp=0.1,dur= 0.2,cutoff=2000|
	var sound, filter;
	sound= Saw.ar(freq, amp)*EnvGen.kr(Env([0,1,1,0],[0.01,0.05,(dur.max(0.07))-0.06]),doneAction:2);
	filter= LPF.ar(sound,Line.kr(cutoff,300,dur));
	Out.ar(0,filter.dup(2))
}).add;

SynthDef(\sin, {arg freq=50, lw=0, index=1000, pDown = 0, lpF=20000, hpF=40, mix=0.75, dur, amp=0.125, pan=0, out=0;
	var env = {arg dr, ml; Decay.ar(Impulse.ar(0),dr,ml)};
	var src1 = SinOsc.ar(
		freq+env.(dur*Rand(0.1,0.125),freq*(IRand(4,8))*pDown),2pi,
		env.(dur,amp*mix)
	);
	var src2 = SinOscFB.ar(
		freq+(src1*index),Rand(lw,2),env.(dur*Rand(0.5,1),amp*(1-mix))
	);
	var src = Pan2.ar(src1+src2,pan);
	var free = DetectSilence.ar(src,doneAction:2);
	src = LPF.ar(src,lpF);
	src = HPF.ar(src,hpF);

	Out.ar(out,Compander.ar(src,src,0.25,1,1/4,0.01,0.1).dup(2))
}).add;

SynthDef(\ping, { |out, t_trig = 1, freq=440, sustain=1, amp=0.5|
	var sound = SinOsc.ar(freq);
//	var sound = Saw.ar(freq);
	sound = (RLPF.ar(sound, freq * XLine.ar(4, 2.5, sustain), 0.15) * 0.3);
	sound = sound * EnvGen.ar(Env.perc(0.01, sustain, 1, -6), t_trig, doneAction: Done.none);
	//sound = Pan2.ar(sound);
	Out.ar(out, (sound * amp).dup(2));
}).add
)

(
var value = 1.0.rand, value1 = 1.0.rand, value2 = 1.0.rand;
var logmap, logmap1, logmap2;
~r=2;
logmap = {|previous=60|
	((1.0-previous)*previous*~r);
};
logmap1 = {|previous=60|
	((1.0-previous)*previous*(~r+1));
};
logmap2 = {|previous=60|
	((1.0-previous)*previous*(~r-1));
};
{
	500.do{
		value = logmap.(value);
		Synth(\sin,[
			\freq, (36+((value*12).round(0.5))).midicps,
			//\freq, 60,
			\lw, 0,
			\index, 10,
			\pDown, 0.75,
			\lpF, 20000,
			\hpF, 40,
			\dur, value/2,
			\amp, 0.25,
		]);
		value.wait;
	};
}.fork;
{
	1500.do{
		value1 = logmap1.(value1);
		Synth(\ping,[\freq, (48+((value1*12).round(0.5))).midicps, \dur, value1/2]);
		value1.wait;
	};
}.fork;
{
	1500.do{
		value2 = logmap2.(value2);
		Synth(\sin,[\freq, (60+((value2*12).round(0.5))).midicps, \dur, value2/2]);
		value2.wait;
	};
}.fork;
{
	~r = 4;
	200.do {
		//~r = 1.5 + 2.0.rand;
		~r = ~r + 0.01;
		~r.postln;
		3.wait;
	}
}.fork;
)