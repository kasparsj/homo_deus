(

SynthDef(\organ, {
	| out = 0, freq = 440, amp=0.1, gate=1, attack=0.01, decay=0.3, sustain=0.5, release=1 |
	var sig, env;
	sig = 0.5*Klang.ar(`[[freq/8, freq/4, freq/2, freq, freq*2, freq*4, freq*8],[0.7, 0.7, 0.7, 1, 1, 0.6, 0.4],[0,0.1,0.2,0.3,0.4,0.5,0.6]]);
	sig = RLPF.ar(sig, 5000, 1);
	env = EnvGen.ar(Env.adsr(attack,decay, sustain, release), gate, doneAction:Done.freeSelf);
	sig = env*sig;
	4.do({
		sig = AllpassN.ar(sig, 0.050, [0.050.rand, 0.050.rand], 1);
	});
	Out.ar(out, amp*sig.tanh);
}).add;

)
