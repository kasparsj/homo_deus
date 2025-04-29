(
SynthDef(\freqAnalysis, { |fft, mag|
	var in, chain, mags, bands;
	in = SoundIn.ar(0); // Or use WhiteNoise.ar, etc.
	chain = FFT(fft, in);

	// Get magnitudes
	mags = PV_MagBuffer(chain, mag);

	// Convert to control rate for analysis outside the Synth
	// This stores the magnitudes in a buffer each control period
}).add;

~fft = Buffer.alloc(s, 1024);
~mag = Buffer.alloc(s, 512);
)

(
s.waitForBoot {
	var numBands = 64;
	// You can read the buffer from the language side like this:
	Routine({
		Synth(\freqAnalysis, [fft: ~fft.bufnum, mag: ~mag.bufnum]);
		1.0.wait; // wait for buffer to fill
		inf.do {
			~mag.getn(0, numBands, { |values|
				("Bands: " ++ values).postln;
			});
			0.1.wait;
		};
	}).play;
};
)
