#include "esphome.h"

float _hue2rgb(float p, float q, float t) {
	if(t < 0) t += 1;
	if(t > 1) t -= 1;
	if(t < 1.0/6) return p + (q - p) * 6 * t;
	if(t < 1.0/2) return q;
	if(t < 2.0/3) return p + (q - p) * (2.0/3 - t) * 6;
	return p;
}
/**
 * @param hue
 * @param saturation
 * @param luminance
 */
esphome::Color HSL2RGB(float h, float s, float l) {
	float q, p;

	float r, g, b;

	if(s == 0){
		r = l;
		g = l;
		b = l; // achromatic
	} else {
		q = l < 0.5 ? l * (1 + s) : l + s - l * s;
		p = 2 * l - q;
		r = _hue2rgb(p, q, h + 1.0/3);
		g = _hue2rgb(p, q, h);
		b = _hue2rgb(p, q, h - 1.0/3);
	}

	return esphome::Color(r * 255, g * 255, b * 255);
}
