(
SynthDef(\cricket1, { |out=0, amp = 1.0|
	var modulator, mod1, mod2, mod3, sig;

	// repeat time is 0.7s: equates to 1.43 Hz.
	modulator = LFSaw.ar(1.43, 1, 0.5, 0.5);
	//SendReply.ar(modulator-0.5, '/cricket');
	mod2 = (modulator * 40.6 * 2pi).cos.squared;
	mod3 = modulator * 3147;
	mod3 = (mod3 * 2pi).cos + ((mod3 * 2 * 2pi).cos * 0.3);
	mod1 = ((Wrap.ar(modulator.min(0.1714) * 5.84) - 0.5).squared * (-4) + 1) * (mod2 * mod3);
	sig = mod1 * 0.1;
	sig = LinPan2.ar(sig, FSinOsc.kr(1/5)) * amp * SinOsc.ar(1/5, 0);
	Out.ar(out, sig);
}).add;

SynthDef(\cricket2, { |out=0, amp=1.0, trigFreq=58.82, trigPhase=0|
	var trig, seq, demand, cricket, sig;

	// instead of [metro], Impulse.kr is used here. Delta t = 17 ms equates to 58.82 Hz.
	trig = Impulse.kr(trigFreq, trigPhase);

	// the main idea of the following line was to use an approach
	// that uses the same coefficients as described in the pd patch
	seq = Dseq(Array.fill(41, {|i| if(i<7, {(i+2)/9},{0}) }),inf);
	demand = Demand.kr(trig,0,seq);

	// Implementation of the pd code for pulses including amplitude grow:
	// cricket = EnvGen.ar(Env.new([0, 1, 1, 0], [0.0001, 0.0001, 0]), trig) * demand;

	// 2nd implementation: pure data seemed to slightly disobey its own specifications,
	// so I analysed the waveform and came up with this:
	cricket = EnvGen.ar(Env.new([0, 1, 0], [4/44100, 0]), trig) * demand;


	cricket = OnePole.ar(cricket, exp(-2pi * (1000 * SampleDur.ir)));
	cricket = (
			// changed the Q factor of the first 3 BPFs to approximate farnells sound
			BPF.ar(cricket, 4500 + ((0..2)*50), 300.reciprocal, 100)).sum
			+ BPF.ar(cricket, 9000, 500.reciprocal, 42
	);
	cricket = (cricket - OnePole.ar(cricket, exp(-2pi * (4000 * SampleDur.ir))));
	sig = cricket * 0.5;
	sig = LinPan2.ar(sig, FSinOsc.kr(1/7)) * amp * SinOsc.ar(1/7, 2pi*1/3);
	Out.ar(out, sig);
}).add;

SynthDef(\cricket3, { |out=0, amp=1.0, trigFreq=0.2, trigPhase=0|
	var sig, trig, seq, freq, mul, vals;

	trig = Impulse.kr(trigFreq, trigPhase);
	vals = [
		[0.5, 128],
		[8, 6],
		[30, 3]
	];
	freq = TChoose.kr(trig, vals);


	sig = WhiteNoise.ar;
	// The one pole filters in pure data and SC differ, so I changed the coefficents
	// a little. Also the  multiplication by 5 is not in the book, but helps to
	// approach the audible result of Farnells patch.
	sig = (sig - OnePole.ar(sig, exp(-2pi * (8000 * SampleDur.ir))));
	sig = (sig - OnePole.ar(sig, exp(-2pi * (8000 * SampleDur.ir))));
	sig = OnePole.ar(sig, exp(-2pi * (10000 * SampleDur.ir)));
	sig = OnePole.ar(sig, exp(-2pi * (10000 * SampleDur.ir)));
	sig = sig * 5;


	sig = BPF.ar(sig, [7500, 5500], 40.reciprocal).sum * SinOsc.ar(500);
	sig = sig * (1 / (SinOsc.ar( freq[0], 0, freq[1] ).squared + 1));
	sig = (sig - OnePole.ar(sig, exp(-2pi * (4000 * SampleDur.ir))));
	//sig = sig*4;
	//sig = LinPan2.ar(sig, FSinOsc.kr(1/11)) * amp * SinOsc.kr(1/11, 2pi*2/3);
	sig = BPeakEQ.ar(sig*4, SinOsc.kr(1/22, 0, 2000, 2000), SinOsc.kr(1/22, 0, 2.0, 2.0), SinOsc.kr(1/22, 0, 24));
	sig = LinPan2.ar(sig, FSinOsc.kr(1/22)) * amp * SinOsc.kr(1/22, 2pi*2/3);
	Out.ar(out, sig);
}).add;

)
