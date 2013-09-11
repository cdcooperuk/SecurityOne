// Do not remove the include below
#include "DisplayNode.h"

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
#define cs   10
#define dc   7
#define rst  8  // you can also connect this to the Arduino reset
#include <TFT.h>
#include <SPI.h>

#include <RF24.h>

#include "Screen.h"
#include "MonitorScreen.h"
#include "ZoneInfo.h"
#include "debug.h"

//#include <RF24_config.h>
//#include <nRF24L01.h>

// Option 1: use any pins but a little slower
//Adafruit_ST7735 tft = Adafruit_ST7735(cs, dc, mosi, sclk, rst);

// Option 2: must use the hardware SPI pins
// (for UNO thats sclk = 13 and sid = 11) and pin 10 must be
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
TFT tft = TFT(cs, dc, rst);
float p = 3.1415926;

RF24 radio(8, 7);

ZoneInfo zoneInfo;

void initComms();
ZoneInfo obtainZoneInfo();

void getInput();
void updateDisplay();

Screen *currentScreen = new MonitorScreen(&tft);

void setup(void) {
	Serial.begin(9600);

	tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab

	tft.setRotation(2);

	currentScreen->clear();

	tft.setTextColor(ST7735_BLACK);
	tft.print(F("Initializing system"));
	initComms();

	zoneInfo = obtainZoneInfo();
	tft.println(F("\nReady"));

	currentScreen->clear();
}

void loop() {
	getInput();
	updateDisplay();
	delay(1000);
}

void initComms() {
	for (int i = 1; i < 20; i++) {
		delay(50);
		tft.print(".");
	}
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

}

void updateDisplay() {
	debug("updateDisplay()...");

	debug("nzones=");debug(zoneInfo.getNumZones());
	currentScreen->refresh(zoneInfo);
	debug("...updateDisplay()");
}
