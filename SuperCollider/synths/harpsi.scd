(
SynthDef(\harpsi, {
	arg outbus = 0, freq = 440, ffreq = 2, amp = 1, gate = 1, release = 1;
	var out;
	freq = freq * Rand(1, 1.005);
	out = EnvGen.ar(Env.adsr(releaseTime:release), gate, doneAction: Done.freeSelf)
	* amp * Pulse.ar([freq,freq*1.005]/1, 0.25, mul:0.75 );
	out = out * [Rand(0.0, 1.0), Rand(0.0, 1.0)];
	out = RLPF.ar(out, freq * (ffreq / 4) + 1 );
	Out.ar(outbus, out);
}).add;
)
