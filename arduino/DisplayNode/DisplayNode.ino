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
#define dc   9
#define rst  8  // you can also connect this to the Arduino reset
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
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
Adafruit_ST7735 tft = Adafruit_ST7735(cs, dc, rst);
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

	tft.fillScreen(ST7735_CYAN);

	tft.setTextColor(ST7735_BLACK);
	tft.print(F("Initializing system"));
	initComms();
	zoneInfo = obtainZoneInfo();
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
	tft.println(F("\nReady"));
}

ZoneInfo obtainZoneInfo() {
	ZoneInfo *zi = new ZoneInfo();
	int nzones = 6;
	zi->setNumZones(nzones);
	debug("obtainZoneInfo nzones=");
	debug(zi->getNumZones());
	zi->zones = (struct Zone*) malloc(sizeof(struct Zone) * nzones);
	zi->zones[0] = {"O&I",20,10, 30, 40,false};
	zi->zones[1] = {"master",50,10, 50, 30,false};
	zi->zones[2] = {"G&H",20,50, 30, 40,false};
	zi->zones[3] = {"hall",50,40, 20, 40,true};
	zi->zones[4] = {"E",70,40, 20, 20,false};
	zi->zones[5] = {"bathroom",50,80, 20, 10,false};
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

	debug("nzones=");
	debug(zoneInfo.getNumZones());
	currentScreen->refresh(zoneInfo);
	debug("...updateDisplay()");
}
