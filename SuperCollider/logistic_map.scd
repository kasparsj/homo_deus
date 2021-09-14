// paskatities stohastiskos procesus
// audio sprakski varetu trigerot - uguns degot.

(
var value = 1.0.rand, value1 = 1.0.rand, value2 = 1.0.rand;
var logmap, logmap1, logmap2;
~r=2;

"synths/log_map_synths.scd".loadRelative;

//s.sync;

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
		//Synth(\ping,[
			\freq, (36+((value*12).round(0.5))).midicps,
			//\freq, 60,
			\lw, 0,
			\index, 10,
			\pDown, 0.75,
			\lpF, 20000,
			\hpF, 40,
			\dur, value,
			\amp, 0.25,
		]);
		value.wait;
	};
}.fork;
{
	1000000.do {
		value1 = logmap1.(value1);
		Synth(\ping,[\freq, (48+((value1*12).round(0.5))).midicps, \dur, value1, \amp, 0.25]);
		value1.wait;
	};
}.fork;
{
	1000000.do {
		value2 = logmap2.(value2);
		//Synth(\ping,[\freq, (60+((value2*12).round(0.5))).midicps, \dur, value2]);
		Synth(\sin,[\freq, (60+((value2*12).round(0.5))).midicps, \dur, value2]);
		value2.wait;
	};
}.fork;
{
	~r = 4.5;
	1000000.do {
		//~r = 1.5 + 4.0.rand;
		~r = ~r + 0.01;
		~r.postln;
		5.wait;
	}
}.fork;
)

//{SinOsc.ar!2}.play