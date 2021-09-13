(

//10 wavetables with increasing complexity
~wt_sig = 10.collect({
	arg i;

	//random number of envelope segments
	var numSegs = i.linexp(0,9,4,40).round;

	Env(
		//env always begins and ends with zero
		//inner points are random from -1.0 to 1.0
		[0]++({1.0.rand}.dup(numSegs-1) * [1,-1]).scramble++[0],

		//greater segment duration variety in higher-index wavetables
		{exprand(1,i.linexp(0,9,1,50))}.dup(numSegs),

		//low-index wavetables tend to be sinusoidal
		//high index wavetables tend to have sharp angles and corners
		{[\sine,0,exprand(1,20) * [1,-1].choose].wchoose([9-i,3,i].normalizeSum)}.dup(numSegs)
	).asSignal(1024);
});

//load into 10 buffers in wavetable format
~wt_buf = Buffer.allocConsecutive(10, s, 2048, 1, {
	arg buf, index;
	buf.setnMsg(0, ~wt_sig[index].asWavetable);
});

SynthDef(\wavetable, {
	arg buf=0, freq=200, detune=0.2, amp=0.2, pan=0, out=0, atk=0.01, sus=1, rel=0.01, c0=1, c1=(-1),
	rout = 0, rmix = 0;
	var sig, env, detuneCtrl;
	env = EnvGen.ar(
		Env([0,1,1,0],[atk,sus,rel],[c0,0,c1]),
		doneAction:2
	);

	//array of eight Oscs with uniquely detune frequencies
	//and unique initial phase offsets
	detuneCtrl = LFNoise1.kr(0.1!8).bipolar(detune).midiratio;
	sig = Osc.ar(buf, freq * detuneCtrl, {Rand(0,2pi)}!8);

	sig = Splay.ar(sig); //spread 8 signals over stereo field
	sig = LeakDC.ar(sig); //remove DC bias
	sig = Balance2.ar(sig[0], sig[1], pan, amp); //L/R balance (pan)
	sig = sig * env;
	Out.ar(out, sig);
	// didn't not work any other way, tried Pfx
	Out.ar(rout, sig * rmix);
}).add;

)