/**
 * Variable "SD" nicht gefunden => einfach die 2 Dateien löschen + neu kompilieren
 *   rm ./.piolibdeps/audio32/ESP8266Audio_ID1964/src/AudioFileSourceSD.h
 *   rm ./.piolibdeps/audio32/ESP8266Audio_ID1964/src/AudioFileSourceSD.cpp
 */
// https://esphome.io/components/switch/custom.html
#include "esphome.h"

#include <alloca.h>

#ifdef ESP32
  #include "SPIFFS.h"
#endif

//#include <AudioFileSourceSPIFFS.h>
#include "AudioFileSourceHTTPStream.h"
//#include "AudioGeneratorMP3.h"
#include "AudioGeneratorWAV.h"
#include "AudioOutputI2SNoDAC.h"
#include "AudioFileSourceBuffer.h"

// #include "AudioStatus.h"
#include "AudioGenerator.h"

static const char *TAG = "audio";

// für minicom/serial \r\n
//#define DEBUGf(format, ...) Serial.printf(format "\r\n", ##__VA_ARGS__)
// geht mit esp32:
#define DEBUGf(tag, format, ...)   ESP_LOGD(tag, format, ##__VA_ARGS__);

/**
 * im cbData muss das TAG stehn - vom regster
 */
void statusCBFn(void *cbData, int code, const char *message) {
	//ESP_LOGD(TAG, "da1 %p", message);
	// string => PROGMEM => %S
	if (message >= (const char *)0x40000000) {
		char *s=(char *) alloca(strlen_P(message));
		strcpy_P(s, message);
		message=s;
	}
	DEBUGf( (char *) cbData, "code:%d statusCBFn: %s", code, message);
}

// == RX
#define I2SO_DATA 3

// FIXME: remove LightOutput, set to Template number + add_on_state_callback
// see: https://loopylab.de/2021/10/10/arduino-to-esphome.html

class EsphomeAudio : public Component, public LightOutput {
public:
	EsphomeAudio() : wav(NULL), /*mp3(NULL),*/ file(NULL), out(NULL), buff(NULL), lightState(NULL) {
		DEBUGf(this->get_name().c_str(), "::EsphomeAudio() ***********");
	}
	void setup() override {
		DEBUGf(this->get_name().c_str(), "::setup() *********** file=%p",this->file);
		// This will be called by App.setup()
		pinMode(I2SO_DATA, OUTPUT);
		digitalWrite(I2SO_DATA,false);
	}
	void setup_state(LightState *state) override {
		DEBUGf(this->get_name().c_str(), "::setup_state() ***********");
		this->lightState=state;
	}

// lightOutput hat kein get_name! -> LightState hätts
	std::string get_name() {
		if(this->lightState)
			return this->lightState->get_name();
		else
			return "EsphomeAudio";
	}

	LightTraits get_traits() override {
		DEBUGf(this->get_name().c_str(), "::get_traits() ***********");
		// return the traits this light supports
		auto traits = LightTraits();
		traits.set_supports_brightness(true);
		traits.set_supports_rgb(false);
		traits.set_supports_rgb_white_value(false);
		traits.set_supports_color_temperature(false);
		return traits;
	}

	void loop() override {
		// This will be called by App.loop()

		//ESP_LOGD(TAG, "'%s': *********** loop ", this->get_name().c_str());
		if (this->wav && this->wav->isRunning()) {
			if (!this->wav->loop()) {
				cleanup();
				DEBUGf(this->get_name().c_str(), "loop() *********** stopped *******");
			}
		} 

	}

	// void write_state(bool state) override { // switch
	void write_state(LightState *lightState) override {
		// esp8266_store_log_strings_in_flash: False im yaml !!!!!
		bool state;
		lightState->current_values_as_binary(&state);
 		float volume;
		lightState->current_values_as_brightness(&volume);
		DEBUGf(this->get_name().c_str(), "write_state() *********** %d %f state=%p", state, volume, lightState);
		if(state && !this->file) {
			DEBUGf(this->get_name().c_str(), "start play wav");
			DEBUGf(this->get_name().c_str(), "init i2s %s", state ? "ON" : "OFF");
			// this->file = new AudioFileSourceSPIFFS("/jamonit.mp3");
			this->file = new AudioFileSourceHTTPStream("http://home-assistant/old-car-engine_daniel_simion.wav");
			this->file->RegisterStatusCB(statusCBFn, (void *) "AudioFileSourceHTTPStream");
			// Create a buffer using that stream
			this->buff = new AudioFileSourceBuffer(this->file, 2048);
			this->buff->RegisterStatusCB(statusCBFn, (void *) "AudioFileSourceBuffer");
#ifdef ESP32
			this->out = new AudioOutputI2S(0, 1); // pin25 esp32-DAC !! geht nicht gemeinsam mit der kamera !!
			// this->out->SetRate(22050);
#else
			this->out = new AudioOutputI2SNoDAC();
			this->out->RegisterStatusCB(statusCBFn, (void *) "AudioOutputI2SNoDAC");
#endif
			this->out->SetGain(volume);

			//this->mp3 = new AudioGeneratorMP3();
			//this->mp3->RegisterStatusCB(statusCBFn, (void *) "AudioGeneratorMP3");
			this->wav = new AudioGeneratorWAV();
			this->wav->RegisterStatusCB(statusCBFn, (void *) "AudioGeneratorWAV");

			uint32_t free = ESP.getFreeHeap();
			
			DEBUGf(this->get_name().c_str(), "wav->begin, free=%d", free);
			if(this->wav->begin(this->buff, this->out)) {
				DEBUGf(this->get_name().c_str(), "started playing" );
			} else {
				DEBUGf(this->get_name().c_str(), "failed to play");
				this->cleanup();
			}
			// this->publish_state();
		} else {
			DEBUGf(this->get_name().c_str(), "didn't start playing state=%d file=%p", state, this->file);
			if(this->out != NULL)
				this->out->SetGain(volume);
		}
	}

	~EsphomeAudio() {
		DEBUGf(this->get_name().c_str(), "~EsphomeAudio()");
		if(this->file)
			this->file->RegisterStatusCB(NULL, NULL);

	}

	void cleanup() {
		DEBUGf(this->get_name().c_str(), "cleanup() state=%p",this->lightState);
		if(this->wav) {
			this->wav->stop();
			delete(this->wav);
			this->wav=NULL;
		}
		if(this->buff) {
			delete(this->buff);
			this->buff=NULL;
		}
		if(this->out) {
			delete(this->out);
			this->out=NULL;
		}
		if(this->file) {
			delete(this->file);
			this->file=NULL;
		}
		pinMode(I2SO_DATA, OUTPUT);
		digitalWrite(I2SO_DATA,false);
		if(this->lightState != NULL) {
			// this->lightState->current_values.set_state(false);
			// this->lightState->publish_state();
			auto call = this->lightState->turn_off();
			call.perform();
			// this->lightState->publish_state();
		}
	}
private:
	//AudioGeneratorMP3 *mp3;
	AudioGeneratorWAV *wav;
	// AudioFileSourceSPIFFS *file;
	AudioFileSourceHTTPStream *file;
	AudioOutput *out;
	AudioFileSourceBuffer *buff;

	LightState *lightState;
};
