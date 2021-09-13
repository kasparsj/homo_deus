(

/*var notes, notes2, notes_rev, notes_rev2;
var faStream, percStream, arpStream, leadStream, bassStream;

notes = (~phi8P.one ++ ~phi8P.one.reverse[1..]);
faStream = Pseq(notes, inf).asStream;

notes = ~phi8P.four([0], 2) ++ ~phi8P.circle([1, 0]) ++ ~phi8P.four([1], 2) ++ ~phi8P.circle([1, 0]);
notes2 = ~phi8P.eye(\large, nil, 1);
percStream = Pseq([Pseq(notes, 14), Pseq(notes2, 8)], inf).asStream;

notes = ~phi8P.two([2, 3]);
notes_rev = notes.reverse;
notes2 = ~phi8P.two([2]);
notes_rev2 = notes2.reverse;
notes2 = (notes2 ++ notes2 ++ notes2 ++ notes2 ++ notes2 ++ notes2 ++ notes2 ++ notes2 ++ notes_rev2[1..] ++ notes_rev2 ++ notes_rev2 ++ notes_rev2 ++ notes_rev2 ++ notes_rev2 ++ notes_rev2 ++ notes_rev2[0..(notes2.size-2)]);
arpStream = Pseq([Pseq((notes ++ notes ++ notes_rev[1..] ++ notes_rev ++ notes[1..] ++ notes), 1), Pseq(notes2, inf)], 1).asStream;

notes = ~phi8P.four([3]);
leadStream = Pseq(notes, inf).asStream;

notes = ~phi8P.triangle(\large, [3]);
bassStream = Pseq(notes, inf).asStream;*/

~phi8StrangerThings = ~strangerThings.mapToEvent((\type: \hd), [
/*	\fastarpeggio, { |event|
		[\midinote, faStream.next];
	},
	\percussion, { |event|
		if (event[\midinote].class != Rest) {
			[\midinote, percStream.next];
		} {
			[\midinote, \];
		};
	},
	\arpeggio, { |event|
		[\midinote, arpStream.next];
	},
	\introFasterAttack, { |event|
		var notes = ~phi8P.circle([3])[0];
		notes = notes.add(notes[0]);
		[\midinote, notes, \strum, 0.1];
	},
	\flute, { |event|
		var notes = ~phi8P.circle([3])[0];
		notes = notes.reverse.add(notes[0]);
		[\midinote, notes, \strum, 0.2];
	},
	\lead1, { |event|
		[\midinote, leadStream.next];
	},
	\bass, { |event|
		[\midinote, bassStream.next];
	},*/
]);

)