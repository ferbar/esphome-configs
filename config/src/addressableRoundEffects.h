#include "esphome.h"
// #include "HSL2RGB.h" => im yaml includen

#undef delay

class LedPosInfo {
public:
	LedPosInfo()
		{ } ;
	LedPosInfo(int x, int y) :
		x(x), y(y)
		{ } ;
	int x=0; // 0 ... 511
	int y=0; // 0 ... 511
};


#define TAG "addressableRoundEffects"

/**
 *
 * @loopLedStartNumbers: array mit welcher led# ein neuer ring anfängt
 * @loopDiameter: 255 = max
 */
void addressableRoundEffectsLambda(AddressableLight &it, int *loopLedStartNumbers, int *loopDiameter, int effectSpeed) {
	// int x = ESP.getFreeHeap();
	// ESP_LOGD(TAG, "free ram: %d", x);
	static int numLeds=-1;
	static LedPosInfo * leds=NULL;

// moving colors left to right:
// [0] ... -512 -> -1
// [1] ... 0 -> 511
// [2] ... 512 -> 1023
	static esphome::Color movingColor[3];
	static int colorPos=-512;

	static float hue=0;

	// it.size() - Number of LEDs
	// it[num] - Access the LED at index num.
	// Set the LED at num to the given r, g, b values
	// it[num] = esphome::Color(r, g, b);
	// Get the color at index num (esphome::Color instance)
	// it[num].get();
	// n leds
	if(numLeds == -1)  {
		numLeds=it.size();
		leds = new LedPosInfo[numLeds];
		if(!leds) {
			ESP_LOGE(TAG, "init leds failed");
			numLeds=-1;
			return;
		}
		int ledNum=0;
		int loopNum=0;
		while(true) {
			if(loopLedStartNumbers[loopNum] == -1) {
				ESP_LOGD(TAG, "init led %d (loop %d) done", ledNum, loopNum);
				break;
			}
			int ledsInLoop=loopLedStartNumbers[loopNum] - ledNum;
			while(ledNum < loopLedStartNumbers[loopNum] && ledNum < numLeds) {
				int ledNumInLoop=ledNum-loopLedStartNumbers[loopNum];
				leds[ledNum].x=sin(PI*2*ledNumInLoop/ledsInLoop)*loopDiameter[loopNum] + 256;
				leds[ledNum].y=cos(PI*2*ledNumInLoop/ledsInLoop)*loopDiameter[loopNum] + 256;
				ESP_LOGD(TAG, "init led %d (loop %d) [%d:%d]", ledNum, loopNum, leds[ledNum].x, leds[ledNum].y);
				ledNum++;
			}
			if(ledNum != loopLedStartNumbers[loopNum]) {
				ESP_LOGW(TAG, "config error ledNum (%d) != loopLedStartNumbers[%d] (%d) ", ledNum, loopNum, loopLedStartNumbers[loopNum]);
				break;
			}
			loopNum++;
		}
		movingColor[0] = esphome::Color::random_color();
		movingColor[1] = esphome::Color::BLACK;
		movingColor[2] = esphome::Color::BLACK;
	}

	int colorPos0=colorPos;
	int colorPos1=colorPos+512;
	int colorPos2=colorPos+512*2;
	for(int i=0; i < numLeds; i++) {
		int posOffset=leds[i].x; // vielleicht nehma irgendwann y noch dazu
		esphome::Color color;
		if(posOffset < colorPos1) { // mix color0+1
			int value0=(colorPos1-posOffset)/2; // value = 255-abstand
			int value1=255-value0;
			// mix color 0 + 1
			color=movingColor[0]*value0 + movingColor[1]*value1;
			if(i==0) {
//				ESP_LOGD(TAG, "[%d] pos:%d colorPos1=%d value0=%d value1=%d %02x %02x %02x", i, posOffset, colorPos1, value0, value1, color.red, color.green, color.blue);
			}
		} else { // mix color1+2
			int value2=(posOffset-colorPos1)/2; // value = 255-abstand
			int value1=255-value2;
			color=movingColor[1]*value1 + movingColor[2]*value2;
			if(i==0) {
//				ESP_LOGD(TAG, "[%d] pos:%d colorPos1=%d value1=%d value2=%d %02x %02x %02x", i, posOffset, colorPos1, value1, value2, color.red, color.green, color.blue);
			}
		}
		if(i==0) {
//	 		ESP_LOGD(TAG, "[%d]=> %02x %02x %02x", i, color.red, color.green, color.blue);
		}
		it[i]=color;

	}
	colorPos+=effectSpeed*effectSpeed;
	if(colorPos >= 0) {
		colorPos=-512;
		movingColor[2] = movingColor[1];
		movingColor[1] = movingColor[0];
		// movingColor[0] = esphome::Color::random_color();
		//ESP_LOGD(TAG, "new Color: r:%d g:%d b:%d hue:%f", movingColor[0].red, movingColor[0].green, movingColor[0].blue, hue);
		movingColor[0] = HSL2RGB(hue, 1, 0.5);
		hue+=0.345;
		if(hue > 1)
			hue=hue-1;
	}
}
