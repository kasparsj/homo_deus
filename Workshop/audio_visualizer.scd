(
SynthDef(\wavPlayer, { |sound, fft|
	var sig, chain;
	sig = PlayBuf.ar(2, soundBuf, BufRateScale.kr(soundBuf), doneAction: 2);
	Out.ar(0, sig);

	// FFT analysis with better frequency resolution
	chain = FFT(fftBuf, Mix(sig));
	chain = PV_MagSmear(chain, 3);  // Smooth the frequency data
	chain = PV_BrickWall(chain, 0.99);  // Remove DC offset
}).add;
)

(
// Audio Visualizer for "literally the moon" WAV files
// Features:
// - File selection dropdown
// - Play/Pause and Stop buttons
// - Frequency bin visualization with adjustable bin count

var basePath = PathName(thisProcess.nowExecutingPath).parentPath;
var wavDir = basePath ++ "/literally the moon";
var wavFiles, wavNames, spectrumView;
var playButton, stopButton, binsSlider, binsText;
var numBins = 64;  // Default number of bins
var currentWavFile = nil;

var playState = false;
var win, fileMenu, timeText, timeRoutine;

// Create FFT buffer
~fftBuf = Buffer.alloc(s, 2048);

// Create the GUI window
win = Window("Audio Visualizer", Rect(100, 100, 800, 500))
.front
.onClose_({
	// Clean up when window is closed
	if(~synth.notNil) { ~synth.free };
	if(~soundBuf.notNil) { ~soundBuf.free };
	if(~fftBuf.notNil) { ~fftBuf.free };
	if(timeRoutine.notNil) { timeRoutine.stop };
});

// Get list of WAV files from directory
wavFiles = PathName(wavDir).files.select({ |item| item.extension.toLower == "wav" });
wavNames = wavFiles.collect({ |item| item.fileName });

// Create the file selection dropdown menu
StaticText(win, Rect(20, 20, 120, 20)).string_("Select WAV file:");
fileMenu = PopUpMenu(win, Rect(150, 20, 400, 20))
.items_(wavNames)
.action_({ |menu|
	// Free previous resources
	if(~synth.notNil) { ~synth.free; ~synth = nil; };
	if(~soundBuf.notNil) { ~soundBuf.free; };

	// Load new sound file
	currentWavFile = wavFiles[menu.value].fullPath;
	~soundBuf = Buffer.read(s, currentWavFile);
	playState = false;
	playButton.states_([
		["Play", Color.black, Color.green],
		["Pause", Color.black, Color.yellow]
	]);

	win.name = "Audio Visualizer - " + wavNames[menu.value];
});

// Create the spectrum analyzer view
StaticText(win, Rect(20, 50, 200, 20)).string_("Frequency Spectrum:");
spectrumView = UserView(win, Rect(20, 80, 760, 320))
.background_(Color.black)
.animate_(true)
.drawFunc_({ |view|
	var freqs, vals;
	var width = view.bounds.width;
	var height = view.bounds.height;
	var binWidth = width / numBins;

	// Draw grid
	Pen.strokeColor = Color.gray(0.2);
	numBins.do { |i|
		if (i % 8 == 0) {
			Pen.strokeColor = Color.gray(0.3);
		} {
			Pen.strokeColor = Color.gray(0.2);
		};
		Pen.line(i * binWidth, 0, i * binWidth, height);
		Pen.stroke;
	};

	// Draw horizontal grid lines and labels
	5.do { |i|
		var y = i * (height / 4);
		Pen.strokeColor = Color.gray(0.3);
		Pen.line(0, y, width, y);
		Pen.stroke;

		Pen.fillColor = Color.gray(0.7);
		Pen.stringAtPoint((100 - (i * 25)).asString + "%", Point(5, y - 15));
	};

	// Draw frequency labels at bottom
	Pen.fillColor = Color.gray(0.7);
	[0, 0.25, 0.5, 0.75, 1].do { |factor, i|
		var x = factor * width;
		var freq = (factor * 20000).round;

		if (freq >= 1000) {
			Pen.stringAtPoint((freq/1000).asString + "kHz", Point(x - 20, height - 15));
		} {
			Pen.stringAtPoint(freq.asString + "Hz", Point(x - 20, height - 15));
		};
	};

	// Draw frequency bins if data is available
	~synth.isPlaying.postln;
	if(~synth.notNil && ~synth.isPlaying) {
		freqs = Array.fill(numBins, { |i| i / numBins * 20000 });
		vals = ~fftBuf.getToFloatArray(action: { |data|
			{
				// Clear background for new drawing
				Pen.fillColor = Color.black;
				Pen.fillRect(Rect(0, 0, width, height));

				// Redraw grid
				spectrumView.refresh;

				// Draw the bins
				numBins.do { |i|
					var binIndex = i * (data.size / numBins / 2).floor;
					var binVal = data[binIndex].abs.sqrt * 5; // Adjust scaling factor
					binVal = binVal.min(1.0) * height;

					// Color gradient from low (blue) to high (red) frequencies
					Pen.fillColor = Color.hsv(0.7 - (i / numBins * 0.7), 0.9, 1);

					// Draw filled rectangles for bins
					Pen.fillRect(Rect(i * binWidth, height - binVal, binWidth - 1, binVal));

					// Draw peak lines
					Pen.strokeColor = Color.white;
					Pen.line(i * binWidth, height - binVal, (i + 1) * binWidth - 1, height - binVal);
					Pen.stroke;
				};
			}.defer;
		});
	};
});

// Create bin count slider
StaticText(win, Rect(20, 410, 80, 20)).string_("Bin Count:");
binsText = StaticText(win, Rect(300, 410, 50, 20)).string_(numBins.asString);
binsSlider = Slider(win, Rect(100, 410, 180, 20))
.value_(numBins / 128)
.action_({ |slider|
	numBins = (slider.value * 128).round(1).asInteger.max(8);
	binsText.string = numBins.asString;
});

// Create time display for current position/duration
timeText = StaticText(win, Rect(500, 410, 200, 20))
.string_("Time: 00:00 / 00:00")
.align_(\right);

// Timer for updating time display
timeRoutine = Routine({
	var prevTime = 0;
	loop {
		if(~synth.notNil && playState) {
			s.sendMsg(\n_query, ~synth.nodeID);

			OSCFunc({ |msg|
				var nodeID = msg[1];
				var currentPos = msg[3]; // current position in seconds
				var duration = if (~soundBuf.notNil and: { ~soundBuf.numFrames.notNil }, { ~soundBuf.duration }, 0);

				if(nodeID == ~synth.nodeID) {
					{
						var formattedCurrent = "%:%".format(
							(currentPos / 60).floor.asString.padLeft(2, "0"),
							(currentPos % 60).floor.asString.padLeft(2, "0")
						);
						var formattedTotal = "%:%".format(
							(duration / 60).floor.asString.padLeft(2, "0"),
							(duration % 60).floor.asString.padLeft(2, "0")
						);
						timeText.string = "Time: % / %".format(formattedCurrent, formattedTotal);
					}.defer;
				};
			}, '/n_info').oneShot;
		} {
			// If not playing, show default or current file duration
			var duration = if (~soundBuf.notNil and: { ~soundBuf.numFrames.notNil }, { ~soundBuf.duration }, 0);
			{
				var formattedTotal = "%:%".format(
					(duration / 60).floor.asString.padLeft(2, "0"),
					(duration % 60).floor.asString.padLeft(2, "0")
				);
				timeText.string = "Time: 00:00 / %".format(formattedTotal);
			}.defer;
		};
		0.1.wait; // Update 10 times per second
	};
}).play(AppClock);

// Create play/pause and stop buttons
playButton = Button(win, Rect(20, 440, 100, 40))
.states_([
	["Play", Color.black, Color.green],
	["Pause", Color.black, Color.yellow]
])
.action_({ |button|
	if(currentWavFile.isNil) {
		// If no file selected, select the first one
		if(wavFiles.size > 0) {
			fileMenu.valueAction = 0;
		};
	};

	if(~soundBuf.notNil) {
		if(button.value == 1 and: { ~synth.isNil }) {
			// Play
			~synth = Synth(\wavPlayer, [soundBuf: ~soundBuf.bufnum, fftBuf: ~fftBuf.bufnum]);

			// Set a function to run when the synth ends
			OSCFunc({
				{
					playButton.value = 0;
					playState = false;
				}.defer;
			}, '/n_end', s.addr, nil, [~synth.nodeID]).oneShot;

			playState = true;
		} {
			// Pause
			if(~synth.notNil) { ~synth.free; ~synth = nil; };
			playState = false;
		};
	};
});

stopButton = Button(win, Rect(130, 440, 100, 40))
.states_([["Stop", Color.white, Color.red]])
.action_({
	if(~synth.notNil) { ~synth.free; ~synth = nil; };
	playButton.value = 0;
	playState = false;
});

// If there are wav files, select the first one by default
if(wavFiles.size > 0) {
	fileMenu.valueAction = 0;
};

// Return the window for future reference
win;
)