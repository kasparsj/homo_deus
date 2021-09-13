(

SynthDef(\fatsaw, {
	| out=0, freq = 440, amp=0.1, gate=1, attack=0.01, decay=0.3, sustain=0.5, release=1, filterSpeed=100, filterFreqStart=300, filterFreqEnd=400, resonance=1, hfFilterFreq=1, hfFilterResonance=1 |
	var sig, env, filtFreq;
	env = EnvGen.ar(Env.adsr(attack, decay, sustain, release), gate, levelScale:0.5, doneAction:Done.freeSelf);
	sig = env*Splay.ar(LFSaw.ar(freq*[0.98,0.99,1.0,1.01,1.02],[LFNoise0.kr(2), LFNoise0.kr(1.9), LFNoise0.kr(2.1)]) + SinOsc.ar(freq*[0.98,0.99,1.0,1.01,1.02],[LFNoise0.kr(2), LFNoise0.kr(1.9), LFNoise0.kr(2.1)]));
	filtFreq = LFSaw.kr(filterSpeed,iphase:1).range(filterFreqStart,filterFreqEnd);
	sig = RLPF.ar(sig, filtFreq, resonance);
	sig = RHPF.ar(sig, hfFilterFreq, hfFilterResonance);
	2.do({
		sig = AllpassN.ar(sig, 0.050, [0.050.rand, 0.050.rand], 1);
	});
	Out.ar(out, amp*sig.tanh);
}).add;

);