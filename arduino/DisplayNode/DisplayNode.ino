// Do not remove the include below
#include "DisplayNode.h"
#include "printf.h"
#include <avr/pgmspace.h>

/***************************************************
 * This is an example sketch for the Adafruit 1.8" SPI display.
 * This library works with the Adafruit 1.8" TFT Breakout w/SD card
 * ----> http://www.adafruit.com/products/358
 * as well as Adafruit raw 1.8" TFT display
 * ----> http://www.adafruit.com/products/618
 *
 * Check out the links above for our tutorials and wiring diagrams
 * These displays use SPI to communicate, 4 or 5 pins are required to
 * interface (RST is optional)
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * Written by Limor Fried/Ladyada for Adafruit Industries.
 * MIT license, all text above must be included in any redistribution
 ****************************************************/

//#define sclk 13
//#define mosi 11
#define TFT_CS_PIN   10
#define TFT_DC_PIN   7
#define TFT_RST_PIN  -1  // you can also connect this to the Arduino reset
#include <TFT.h>
#include <SPI.h>

#include <RF24.h>

#include "Screen.h"
#include "MonitorScreen.h"
#include "StatusScreen.h"
#include "ZoneInfo.h"
#include "debug.h"

namespace {

const int RF24_CHANNEL = 76;

}

//#include <RF24_config.h>
//#include <nRF24L01.h>

// Option 1: use any pins but a little slower
//Adafruit_ST7735 tft = Adafruit_ST7735(cs, dc, mosi, sclk, rst);

// Option 2: must use the hardware SPI pins
// (for UNO thats sclk = 13 and sid = 11) and pin 10 must be
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
TFT tft = TFT(TFT_CS_PIN, TFT_DC_PIN, TFT_RST_PIN);

const uint64_t my_addr = 0xF0F0F0F080LL;
const uint64_t hub_addr = 0xF0F0F0F0D2LL;

RF24 radio(8, 6);

ZoneInfo zoneInfo;

boolean initComms();
ZoneInfo obtainZoneInfo();

void getInput();
void updateDisplay();

struct Status status;

MonitorScreen monitorScreen(&tft);
StatusScreen statusScreen(&tft);

Screen *currentScreen = &monitorScreen;

void setup(void) {
	Serial.begin(9600);
	printf_begin();

	tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab

	//tft.setRotation(2);

	currentScreen->clear();

	tft.setTextColor(ST7735_BLACK);
	tft.print(F("Initializing system"));
	if (!initComms()) {
		status.ok = false;
		strcpy_P(status.error, PSTR("radio failed"));
	}
	zoneInfo = obtainZoneInfo();
	tft.println(F("\nReady"));

	if (!status.ok) {
		currentScreen = &statusScreen;
	}
	currentScreen->clear();
}

void loop() {
	getInput();
	updateDisplay();
	delay(1000);
}

int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}
boolean initComms() {
	tft.println("Initializing radio");
	radio.begin();
	radio.setRetries(15, 15);
	radio.setDataRate(RF24_1MBPS);
	radio.setPALevel(RF24_PA_LOW);
	radio.setChannel(RF24_CHANNEL);
	radio.setCRCLength(RF24_CRC_16);
	radio.enableDynamicPayloads();

	printf("opening pipes\n");
	radio.openReadingPipe(1, my_addr);
	radio.openWritingPipe(hub_addr);

	radio.startListening();
	Serial.println(freeRam());
	//
	// Dump the configuration of the rf unit for debugging
	//
	printf("+++ RADIO DETAILS +++\n");
	radio.printDetails();
	printf("---------------------\n");

	return (radio.getChannel() == RF24_CHANNEL);
}

ZoneInfo obtainZoneInfo() {
	ZoneInfo *zi = new ZoneInfo();
	int nzones = 12;
	zi->setNumZones(nzones);
	debug("obtainZoneInfo nzones=");debug(zi->getNumZones());
	zi->zones = (struct Zone*) malloc(sizeof(struct Zone) * nzones);
	zi->zones[0] = {"BB", 5, 5, 35, 30, false};
	zi->zones[1] = {"MB", 40, 5, 40, 22, false};
	zi->zones[2] = {"FB", 5, 35, 35, 35, false};
	zi->zones[3] = {"H1", 0,0,0,0, true};
	zi->zones[4] = {"SB", 70, 27, 20, 20, false};
	zi->zones[5] = {"B1", 40, 50, 30, 15, false};

	zi->zones[6] = {"DR", 5, 85, 35, 30, false};
	zi->zones[7] = {"KI", 40, 85, 45, 30, false};
	zi->zones[8] = {"LO", 5, 115, 35, 35, false};
	zi->zones[9] = {"Hg", 0,0,0,0,true};
	zi->zones[10] = {"Bg", 55 ,115, 20, 20, false};
	zi->zones[11] = {"GA", 70, 135, 15, 15,false};
	return *zi;
}

/**
 * input from user action or radio.
 */
void getInput() {
	debug("getInput()");

	char receivePayload[32];
	uint8_t pipe = 0;

	if (radio.available(&pipe)) {

		uint8_t len = radio.getDynamicPayloadSize();
		radio.read(receivePayload, len);
		receivePayload[len] = 0; //terminate
		printf("read %d data \n", len);
		printf("read '%s'\n", receivePayload);
	}
}

void updateDisplay() {
	debug("updateDisplay()...");

	debug("nzones=");debug(zoneInfo.getNumZones());
	currentScreen->refresh(zoneInfo, status);
	debug("...updateDisplay()");
}
