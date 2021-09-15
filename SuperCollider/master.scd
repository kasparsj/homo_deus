(

var filterKeys = [\lpf, \moogVcf, \delay, \comb, \reverb, \gverb, \tanh, \limiter];

MainFX.clear;
~phi8Master = MainFX(Server.default, 2, filterKeys);
~phi8Master.add(\limiter, 1);

[\add, \remove].do { |method|
	OSCdef(("phi8MasterFilter" ++ method).asSymbol, { |msg|
		var index = msg.indexOf(1.0);
		if (index != nil) {
			~phi8Master.perform(method, filterKeys[index - 1]);
		};
	}, "/phi8/master/filters/" ++ method);
	filterKeys.do { |key|
		OSCdef(("phi8MasterFilter" ++ method ++ key).asSymbol, { |msg|
			if (msg[1] == 1.0) {
				~phi8Master.perform(method, key);
			};
		}, "/phi8/master/filters/" ++ method ++ "/" ++ key);
	};
};

filterKeys.do { |filter|
	[\wet].do { |param|
		OSCdef(("phi8MasterFilters" ++ filter ++ param).asSymbol, { |msg|
			var index = ~phi8Master.slotNames.indexOf(filter);
			~phi8Master.proxy.set((param.asString ++ index.asString).asSymbol, msg[1]);
		}, "/phi8/master/filters/" ++ filter);
	};
	[\freq].do { |param|
		OSCdef(("phi8MasterFilters" ++ filter ++ param).asSymbol, { |msg|
			var index = ~phi8Master.slotNames.indexOf(filter);
			~phi8Master.proxy.setAt(index, param, msg[1]);
		}, "/phi8/master/filters/" ++ filter ++ "/" ++ param);
	};
	[\drive, \ampLimit].do { |param|
		OSCdef(("phi8MasterFiltersLimiter" ++ param).asSymbol, { |msg|
			var index = ~phi8Master.slotNames.indexOf(\limiter);
			~phi8Master.proxy.setAt(index, param, msg[1] * 1.5);
		}, "/phi8/master/filters/limiter" ++ "/" ++ param);
	};
};

)