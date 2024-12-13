(

SynthDef(\playbuf, {
    |out = 0, bufnum, rate = 1.0, amp = 1.0, loop = 0, startPos = 0|
    var sig = PlayBuf.ar(2, bufnum, rate, loop: loop, startPos: startPos);
    sig = sig * amp;
    Out.ar(out, sig);
}).add;

)