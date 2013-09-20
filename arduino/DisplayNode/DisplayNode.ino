// Do not remove the include below
#define __STDC_LIMIT_MACROS
#include "DisplayNode.h"
#include "printf.h"
#include <avr/pgmspace.h>
#include <inttypes.h>

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
#undef DO_DEBUG
#include "debug.h"

const int RF24_CHANNEL = 0x4c;

// Option 1: use any pins but a little slower
//Adafruit_ST7735 tft = Adafruit_ST7735(cs, dc, mosi, sclk, rst);

// Option 2: must use the hardware SPI pins
// (for UNO thats sclk = 13 and sid = 11) and pin 10 must be
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
TFT tft = TFT(TFT_CS_PIN, TFT_DC_PIN, TFT_RST_PIN);
const uint64_t my_addr = 0xF0F0F0F087LL;
//const uint64_t my_addr = 0xf0f0f0f003LL;
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
	pinMode(10, OUTPUT);
	Serial.begin(9600);
	printf_begin();

	tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab

	//tft.setRotation(2);

	currentScreen->clear();

	tft.setTextColor(ST7735_BLACK);
	tft.print(F("Initializing system\n"));
	if (!initComms()) {
		status.ok = false;
		strcpy_P(status.error, PSTR("radio failed"));
		printf_P(PSTR("radio failed"));
		printf("\n");
	}
	zoneInfo = obtainZoneInfo();
	zoneInfo.markDirty(true);
	tft.println(F("\nReady"));

	if (!status.ok) {
		currentScreen = &statusScreen;
	}
	currentScreen->clear();
}

int freeRam() {
	extern int __heap_start, *__brkval;
	int v;
	return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}
boolean initComms() {
	tft.println("Initializing radio\n");
	radio.begin();
	radio.setRetries(15, 15);
	radio.setDataRate(RF24_1MBPS);
	radio.setPALevel(RF24_PA_LOW);
	radio.setChannel(RF24_CHANNEL);
	radio.setCRCLength(RF24_CRC_16);
	radio.enableDynamicPayloads();
	radio.setAutoAck(1);

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
	printf("--- END ---\n");
	delay(1000);
	return (radio.getChannel() == RF24_CHANNEL);
}

ZoneInfo obtainZoneInfo() {
	ZoneInfo *zi = new ZoneInfo();
	int nzones = 12;
	zi->setNumZones(nzones);
	debug("obtainZoneInfo nzones=")
	debug(zi->getNumZones())
	zi->zones = (struct Zone*) malloc(sizeof(struct Zone) * nzones);
	zi->initZone(0, "H1", 0, 0, 0, 0, true);
	zi->initZone(1, "MB", 40, 5, 40, 22, false);
	zi->initZone(2, "FB", 5, 35, 35, 35, false);
	zi->initZone(3, "BB", 5, 5, 35, 30, false);
	zi->initZone(4, "SB", 70, 27, 20, 20, false);
	zi->initZone(5, "B1", 40, 50, 30, 15, false);

	zi->initZone(6, "DR", 5, 85, 35, 30, false);
	zi->initZone(7, "KI", 40, 85, 45, 30, false);
	zi->initZone(8, "LO", 5, 115, 35, 35, false);
	zi->initZone(9, "Hg", 0, 0, 0, 0, true);
	zi->initZone(10, "Bg", 55, 115, 20, 20, false);
	zi->initZone(11, "GA", 70, 135, 15, 15, false);
	return *zi;
}

void loop() {
	static unsigned long lastStatusPrint = millis();
	getInput();
	if (lastStatusPrint + 10000 < millis()) {
		//radio.print_status();
		zoneInfo.markDirty(true);
		lastStatusPrint = millis();
	}
	updateDisplay();
	//	delay(1000);
}

/**
 * input from user action or radio.
 */
void getInput() {
	debug("getInput()")

	char receivePayload[32];
	uint8_t len = 0;
	uint8_t pipe = 0;

	while (radio.available(&pipe)) {

		len = radio.getDynamicPayloadSize();
		radio.read(receivePayload, len);
//		printf("read %d data \n", len);
		receivePayload[len] = 0; //terminate
		//printf("read %d data \n", len);
		printf("read from  %" PRIu8 " '%s', RPD=%d \n", pipe, receivePayload,radio.testRPD() );

		// decipher message
		if (receivePayload[0] == 'S') {
			uint8_t zone_num, sensor_num, state;
			char sensor_type;
			sscanf(receivePayload, "S%hhd %c%hhd %hhd", &zone_num, &sensor_type,
					&sensor_num, &state);
			zoneInfo.setZoneStatus(zone_num, sensor_type, sensor_num, state);
		}
	}
}

void updateDisplay() {
	currentScreen->refresh(&zoneInfo, &status);
}
