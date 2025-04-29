(
var floatToRgb, intToRgb, hueToRgb, hslToRgb, rgbToHsl, rgbArray, hexColor;

floatToRgb = { |value, mul, add|
	hslToRgb.(value * (mul ? 1) + (add ? 0));
};

intToRgb = { |value, min, max|
	floatToRgb.((value - min) / (max - min));
};

hueToRgb = { |p, q, t|
	if(t < 0, { t = t + 1 },
		if (t > 1, { t = t - 1 }));
	if(t < (1/6), { p + (q - p) * 6 * t },
		if(t < (1/2), { q },
			if(t < (2/3), { p + (q - p) * ((2/3) - t) * 6 },
				{ p })));
};

hslToRgb = { |h, s=1, l=0.5|
	var r, g, b;
	if (s === 0) {
		r = g = b = l; // achromatic
	} {
		var q = if (l < 0.5, { l * (1 + s) }, { l + s - l * s });
		var p = 2 * l - q;
		r = hueToRgb.(p, q, h + (1/3));
		g = hueToRgb.(p, q, h);
		b = hueToRgb.(p, q, h - (1/3));
	};
	[(r * 255).round.asInteger, (g * 255).round.asInteger, (b * 255).round.asInteger];
};

rgbArray = { |rgb|
	if (rgb.isArray.not) {
		var red = (rgb >> 16) & 0xFF;
		var green = (rgb >> 8) & 0xFF;
		var blue = rgb & 0xFF;
		rgb = [red, green, blue];
	};
	rgb;
};

rgbToHsl = { |rgb|
	var r, g, b, vmax, vmin, h, s, l;
	var rgbArr = rgbArray.(rgb);
	r = rgbArr[0] / 255;
	g = rgbArr[1] / 255;
	b = rgbArr[2] / 255;
	vmax = [r, g, b].maxItem;
	vmin = [r, g, b].minItem;
	h = s = l = (vmax + vmin) / 2;

	if (vmax == vmin) {
		[0, 0, l]; // achromatic
	} {
		var d = vmax - vmin;
		s = if (l > 0.5, { d / (2 - vmax - vmin) }, { d / (vmax + vmin) });
		if (vmax === r) {
			h = (g - b) / d + if (g < b, 6, 0);
		};
		if (vmax === g) {
			h = (b - r) / d + 2;
		};
		if (vmax === b) {
			h = (r - g) / d + 4;
		};
		h = h / 6;

		[h, s, l];
	};
};

hexColor = { |color|
	if (color.isArray) {
		color = ((color[0] & 0x0ff) << 16) | ((color[1] & 0x0ff) << 8) | (color[2] & 0x0ff);
	};
	color = color ? -1;
	if (color < 0) {
		nil
	} {
		color;
	};
};

~floatToRgb = floatToRgb;
~intToRgb = intToRgb;
~hexColor = hexColor;
)