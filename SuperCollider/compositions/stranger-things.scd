(
var intro, introFasterAttack, fastarpeggio, arpeggio, percussion, flute, lead1, bass, score;
var tempoFactor = 0.18;
var volFactor = 0.6;

intro = Pbind(
	\instrument, \fatsaw,
	\amp, 1.2*volFactor,
	\attack, 3,
	\decay, 0.1,
	\sustain, 1,
	\release, 5,
	\filterSpeed, Pfunc({tempoFactor*0.5}),
	\filterFreqStart, 50,
	\filterFreqEnd, 800,
	\midinote, Pseq("E3m".chordProg, 1),
	\dur, Pseq([6],1));

introFasterAttack = Pbind(
	\instrument, \fatsaw,
	\amp, 0.5*volFactor,
	\attack, 0.01,
	\decay, 0.3,
	\sustain, 1,
	\release, 2,
	\filterSpeed, Pfunc({1/(tempoFactor*6*4);}),
	\filterFreqStart, 50,
	\filterFreqEnd, 3000,
	\midinote, Pseq("E3m".chordProg, 1),
	\dur, 4);

fastarpeggio = Pbind(
	\instrument, \organ,
	\amp, Pseq([0.6,0.15,0.2,0.25,0.3,0.25,0.2,0.15]*0.1*volFactor, inf),
	\attack, 0.1,
	\decay, 0.1,
	\sustain, 0.3,
	\release, 1,
	\dur, Pfunc({tempoFactor/2}),
	\midinote, Pseq("c5 e5 g5 b5 c6 b5 g5 e5".noteProg, 6)
);

percussion = Pbind(
	\instrument, \kick,
	\amp, Pseq([0.9,0.8]*volFactor, inf),
	\filterFreq, Pseq([Pseq([1000], 4), Pseq([2000], 4)], inf),
	\dur, Pseq([tempoFactor], inf),
	\filterSpeed, 0.1,
	\filterFreqStart, 50,
	\filterFreqEnd, 300,
	\midinote, Pseq("c3 c3".noteProg ++ [Rest(), Rest()], 60),
);

arpeggio = Pbind(
	\instrument, \fatsaw,
	\amp, 0.6*volFactor,
	\attack, 0.01,
	\decay, 0.3,
	\sustain, 0.1,
	\release, 0.4,
	\time, Ptime(inf),
	\resonance, Pfunc({ |ev| ev['time'].linexp(0,10,1,0.2); }),
	\midinote, Pseq("c2 e2 g2 b2 c3 b2 g2 e2".noteProg, 14+4+4+6),
	\dur, Pfunc({tempoFactor}));

flute = Pbind(
	\instrument, \fatsaw,
	\amp, 0.4*volFactor,
	\attack, 3,
	\decay, 0.3,
	\sustain, 3,
	\release, 4,
	\filterSpeed, Pfunc({tempoFactor}),
	\filterFreqStart, 200,
	\filterFreqEnd, 6000,
	\midinote, Pseq(["g4 b4 g5 b5".noteProg], 1),
	\dur, Pfunc({tempoFactor*5});
);

lead1 = Pbind(
	\instrument, \fatsaw,
	\amp, 0.2*volFactor,
	\attack, 0.01,
	\decay, 0.3,
	\sustain, 1,
	\release, 1,
	\resonance, 0.2,
	\filterSpeed, Pfunc({tempoFactor}),
	\filterFreqStart, 3000,
	\filterFreqEnd, 4000,
	\hfFilterFreq, 10,
	\midinote, Pseq(["b1 b2 b3", "g2 g3 g4"].noteProg, 1),
	\dur, Pfunc({tempoFactor*4}),
);

bass = Pbind(
	\instrument, \fatsaw,
	\amp, Pseq([0.3, 0.2, 0.2, 0.18, 0.2, 0.18, 0.2, 0.18]*2*volFactor,inf),
	\attack, 0.01,
	\decay, 0.5,
	\sustain, Pseq([1,0.7,1,0.7],inf),
	\release, 8,
	\filterSpeed, 0.1,
	\filterFreqStart, 2000,
	\filterFreqEnd, 3000,
	\resonance, 0.8,
	\hfFilterFreq, 5,
	\midinote, Pseq(["c1 c2 c3", "d1 d2 d3", "e1 e2 e3", "d1 d2 d3"].noteProg, 2),
	\dur, Pseq([tempoFactor*24, tempoFactor*8, tempoFactor*24, tempoFactor*8],inf)
);

~strangerThings = Composition([
	\intro, intro,
	\fastarpeggio, fastarpeggio,
	\percussion, percussion,
	\arpeggio, arpeggio,
	\introFasterAttack, introFasterAttack,
	\flute, flute,
	\lead1, lead1,
	\bass, bass,
]).times_([
	0.0, \intro,
	0.0, \fastarpeggio,
	tempoFactor*8*3, \percussion,
	tempoFactor*8*3, \arpeggio,
	tempoFactor*8*9, \introFasterAttack,
	tempoFactor*8*10, \flute,
	tempoFactor*8*13, \lead1,
	tempoFactor*8*14, \bass
]);

)
