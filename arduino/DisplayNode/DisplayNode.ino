// Do not remove the include below
#define __STDC_LIMIT_MACROS
#include "DisplayNode.h"
#include "printf.h"
#include <avr/pgmspace.h>
#include <inttypes.h>

#define SS 10 //arduino's own slave select
//#define sclk 13
//#define mosi 11
#define TFT_CS_PIN   9
#define TFT_DC_PIN   7
#define TFT_RST_PIN  -1  // you can also connect this to the Arduino reset
#include <TFT.h>
#include <SPI.h>

#define MODE_PIN   2

#include <RF24.h>

#include <SOCommon.h>
#include "Screen.h"
#include "MonitorScreen.h"
#include "StatusScreen.h"
#include "Status.h"
#include "ZoneInfo.h"
#undef DO_DEBUG
#include "debug.h"

// Option 1: use any pins but a little slower
//Adafruit_ST7735 tft = Adafruit_ST7735(cs, dc, mosi, sclk, rst);

// Option 2: must use the hardware SPI pins
// (for UNO thats sclk = 13 and sid = 11) and pin 10 must be
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
TFT tft = TFT(TFT_CS_PIN, TFT_DC_PIN, TFT_RST_PIN);

RF24 radio(8, 6);

ZoneInfo zoneInfo;

void initKeys();
void initTFT();
boolean initRadio();
ZoneInfo obtainZoneInfo();

void switchScreen(Screen &screen);
void handleKeyInput();
void updateDisplay();

// Interrupt handler, check the radio because we got an IRQ
void check_radio(void);

struct Status status;

MonitorScreen monitorScreen(&tft);
StatusScreen statusScreen(&tft);

Screen *currentScreen = &monitorScreen;
volatile int nMsgsReceived = 0;

int freeRam()
{
	extern int __heap_start, *__brkval;
	int v;
	return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

void printFreeRam()
{
	Serial.print("Free RAM=");
	Serial.println(freeRam());
}

void setup(void)
{
	//make sure arduino never thinks its a slave
	pinMode(SS, OUTPUT);
	digitalWrite(SS, HIGH);

	pinMode(TFT_CS_PIN, OUTPUT);
	Serial.begin(115200);
	printf_begin();

	initTFT();

	initKeys();

	tft.print(F("Initializing system\n"));
	status.ok = true;
	strncpy(status.error, "OK", 2);
	if (!initRadio())
	{
		status.ok = false;
		strcpy_P(status.error, PSTR("radio failed"));
		printf_P(PSTR("radio failed"));
		printf("\n");
	}
	zoneInfo = obtainZoneInfo();
	zoneInfo.markDirty(true);

	tft.println(F("\nReady"));

	if (status.ok)
	{
		switchScreen(monitorScreen);
	}
	else
	{
		switchScreen(statusScreen);
	}

	attachInterrupt(1, check_radio, FALLING);
	radio.startListening();

}

void switchScreen(Screen &screen)
{
	currentScreen = &screen;
	currentScreen->clear();
}

void initKeys()
{
	pinMode(MODE_PIN, INPUT_PULLUP);
}

void initTFT()
{
	tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
	currentScreen->clear();
	tft.setTextColor(ST7735_WHITE);
}

boolean initRadio()
{
	tft.println("Initializing radio\n");
	radio.begin();
	radio.setDataRate(RF24_1MBPS);
	radio.setPALevel(RF24_PA_HIGH);
	radio.setChannel(RF24_CHANNEL);
	radio.setCRCLength(RF24_CRC_16);
	radio.enableDynamicPayloads();
	radio.setAutoAck(true);
	radio.powerUp();

	printf("opening pipes\n");
	radio.openReadingPipe(1, display_addr);
//	radio.openReadingPipe(1, 0xF0F0F0F0E1LL);
	//radio.openWritingPipe(hub_addr);

	printFreeRam();
	//
	// Dump the configuration of the rf unit for debugging
	//
	printf("+++ RADIO DETAILS +++\n");
	radio.printDetails();
	printf("--- END ---\n");

	//RF24 interrupt
	pinMode(3, INPUT);

	return (radio.getChannel() == RF24_CHANNEL);
}

ZoneInfo obtainZoneInfo()
{
	ZoneInfo *zi = new ZoneInfo();
	int nzones = 12;
	zi->setNumZones(nzones);
	debug("obtainZoneInfo nzones=")
	debug(zi->getNumZones())
	zi->zones = (struct Zone*) malloc(sizeof(struct Zone) * nzones);
	zi->initZone(0, "H1", 0, 0, 0, 0, 0, true);
	zi->initZone(1, "MB", 40, 5, 40, 22, WALL_TOP, false);
	zi->initZone(2, "FB", 5, 35, 35, 35, WALL_BOTTOM, false);
	zi->initZone(3, "BB", 5, 5, 35, 30, WALL_TOP, false);
	zi->initZone(4, "SB", 70, 27, 20, 20, WALL_BOTTOM, false);
	zi->initZone(5, "B1", 40, 50, 30, 15, WALL_BOTTOM, false);

	zi->initZone(6, "DR", 5, 85, 35, 30, WALL_TOP, false);
	zi->initZone(7, "KI", 40, 85, 45, 30, WALL_TOP, false);
	zi->initZone(8, "LO", 5, 115, 35, 35, WALL_BOTTOM, false);
	zi->initZone(9, "Hg", 0, 0, 0, 0, 0, true);
	zi->initZone(10, "Bg", 55, 115, 20, 20, 0, false);
	zi->initZone(11, "GA", 70, 135, 15, 15, WALL_BOTTOM, false);
	return *zi;
}

void loop()
{
	static unsigned long lastStatusPrint = millis();
	handleKeyInput();
//	getRadioInput();
	if (lastStatusPrint + 10000 < millis())
	{
		//radio.print_status();
		zoneInfo.markDirty(true);
		lastStatusPrint = millis();
		printf("nMsgsReceived=%d\n", nMsgsReceived);
		radio.print_status();
	}
	updateDisplay();
	delay(1000);
}

void toggleScreen()
{
	Screen *s;
	if (currentScreen == &statusScreen)
		s = &monitorScreen;
	else
		s = &statusScreen;
	switchScreen(*s);
}
/**
 * input from user action or radio.
 */
void handleKeyInput()
{
	debug("getInput()")
	int modePinPressed = (digitalRead(MODE_PIN) == 0);
	debug("modePinPressed=")
	debug(modePinPressed);debug("\n");
	if (modePinPressed)
		toggleScreen();
	delay(50); //debounce
}

volatile char message[32];

void dataReadyHandler(const uint8_t status)
{
	//printf("\t\tdataReadyHandler(%02x)\n", status);
	printFreeRam();
	//printf("Received something\n\r");
	char receivePayload[32];
	bool isFIFOEmpty = false; // or we wouldn't be here
	do
	{
		uint8_t len = 0;
		nMsgsReceived++;
		len = radio.getDynamicPayloadSize();
		if (len != 7)
		{
			//printf("bad packet (len!=7)\n");
			radio.flush_rx();
			message[0]=0;
			return;
		}
		else
		{
			isFIFOEmpty = radio.read(receivePayload, len);
			receivePayload[len] = 0; //terminate
			//printf("read from  %" PRIu8 " '%s'\n", pipe_num, receivePayload);
			//printf("\t\t\tread something, len=%hhd\n", len);
			strcpy((char*)message,receivePayload);
			//printf("\t\tisFIFOEmpty=%d\n", isFIFOEmpty);
			//printf("read '%s'\n", receivePayload);
//			printf("%c\n", receivePayload[0]);
//			printf("%c\n", receivePayload[1]);
//			printf("%c\n", receivePayload[2]);
//			printf("%x\n", receivePayload[3]);
//			printf("%c\n",receivePayload[4]);
//			printf("%c\n",receivePayload[5]);
//			printf("%c\n",receivePayload[6]);

			// decipher message
			if (receivePayload[0] == 'S')
			{
				uint8_t zone_num, sensor_num, state;
				char sensor_type;
//				sscanf(receivePayload, "S%d %c%d %01d", &zone_num,
//						&sensor_type, &sensor_num, &state);
				zone_num=receivePayload[1]-'0';
				sensor_type=receivePayload[3];
				sensor_num=receivePayload[4]-'0';
				state=receivePayload[6]-'0';
				printf("setzone %d %c%d %01d\n", zone_num, sensor_type, sensor_num, state);
//				zoneInfo.setZoneStatus(zone_num, sensor_type, sensor_num,
//						state);
			}
		}
	} while (!isFIFOEmpty);

}
void dataSentHandler(const uint8_t status)
{
	printf("\t\tdataSentHandler(%02x)\n", status);
}
void maxRetransmitsHandler(const uint8_t status)
{
	printf("\t\tmaxRetransmitsHandler(%02x)\n", status);
}
void check_radio(void)
{
	printf("\tin check_radio\n");

	radio.dispatchAndClearInterrupts(dataReadyHandler, dataSentHandler,
			maxRetransmitsHandler);
}

void updateDisplay()
{

	currentScreen->refresh(&zoneInfo, &status);

}
