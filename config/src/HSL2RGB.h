#include "esphome.h"

/**
 * spuckt aus einen wert am regenbogen:
 * 0 => blau
 * 255 => rosa
 * 511 => rot
 * 256*3 => gelb
 * 256*4 => grün
 * 256*5 => türkis
 *
 * pos 0 ... 256*6-1
 *
 */
esphome::Color rainbow2Color(int pos) {
	int red;
	int green;
	int blue;

	if(pos > 1279) {
		red=0;
		green=0;
		blue=255;
	} else if(pos > 1023) {
		red=0;
		green=1279-pos;
		blue=pos-1023;
	} else if(pos > 767) {
		red=1023-pos;
		green=255;
		blue=0;
	} else if(pos > 511) {
		red=255;
		green=pos-255;
		blue=0;
	} else if(pos > 255) {
		red=255;
		green=0;
		blue=255-pos;
	} else {
		red=pos;
		green=0;
		blue=255;
	}
	return esphome::Color(red, green, blue);
}

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
