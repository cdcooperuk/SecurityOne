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

#include <SOcommon.h>
#include "Screen.h"
#include "MonitorScreen.h"
#include "StatusScreen.h"
#include "Status.h"
#include "ZoneInfo.h"
#undef DO_DEBUG
#define DO_TIMING
#include "debug.h"

// Option 1: use any pins but a little slower
//Adafruit_ST7735 tft = Adafruit_ST7735(cs, dc, mosi, sclk, rst);

// Option 2: must use the hardware SPI pins
// (for UNO thats sclk = 13 and sid = 11) and pin 10 must be
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
TFT tft = TFT(TFT_CS_PIN, TFT_DC_PIN, TFT_RST_PIN);

RF24 radio(8, 6);
bool radio_interrupt = false;

ZoneInfo zoneInfo;

void initKeys();
void initTFT();
boolean initRadio();
ZoneInfo obtainZoneInfo();

void switchScreen(Screen &screen);
void handleKeyInput();
void getRadioInput();
void updateDisplay();

// Interrupt handler, check the radio because we got an IRQ
void check_radio(void);

struct Status status;

MonitorScreen monitorScreen(&tft);
StatusScreen statusScreen(&tft);

Screen *currentScreen = &monitorScreen;
volatile char heartbeatSymbol = '-';

void setup(void)
{
	Serial.begin(115200);
	printf_begin();

	//make sure arduino never thinks its a slave
	pinMode(SS, OUTPUT);
	digitalWrite(SS, HIGH);

	pinMode(TFT_CS_PIN, OUTPUT);

	initTFT();

	initKeys();

	tft.print(F("Initializing system\n"));
	tft.println(F(__DATE__));
	tft.println(F(__TIME__));
	tft.println();
	delay(2000);
	status.ok = true;
	strncpy((char*) status.status_text, "OK", 2);
	if (!initRadio())
	{
		status.ok = false;
		strcpy_P((char*) status.status_text, PSTR("radio failed"));
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

//	tft.background(ST7735_BLACK);
//	for (uint8_t r = 0xFF; r >=0; r-=0xf)
//	{
//		for (uint8_t g = 0; g < 0xFF; g+=0xf)
//		{
//			for (uint8_t b = 0; b < 0xFF; b+=0xf)
//			{
//				uint16_t c = tft.newColor(r, g, b);
//				tft.setCursor(0, 0);
//				tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
//				tft.print(r);tft.println("  ");
//				tft.print(g);tft.println("  ");
//				tft.print(b);tft.println("  ");
//				tft.print(c);
//				tft.fillCircle(60, 80, 50, c);
//			}
//		}
//	}

}

void switchScreen(Screen &screen)
{
	currentScreen = &screen;
	currentScreen->clear();
	zoneInfo.markDirty(true);
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
	radio.setDataRate(CFG_RF24_DATA_RATE);
	radio.setPALevel(RF24_PA_HIGH);
	radio.setChannel(CFG_RF24_CHANNEL);
	radio.setCRCLength(CFG_RF24_CRC_LENGTH);
	radio.enableDynamicPayloads();
	radio.setAutoAck(true);
	radio.powerUp();

	radio.openReadingPipe(1, display_addr);
//	radio.openReadingPipe(1, 0xF0F0F0F0E1LL);
	//radio.openWritingPipe(hub_addr);

	//
	// Dump the configuration of the rf unit for debugging
	//
	radio.printDetails();

	//RF24 interrupt
	pinMode(3, INPUT);

	return (radio.getChannel() == CFG_RF24_CHANNEL);
}

ZoneInfo obtainZoneInfo()
{
	ZoneInfo *zi = new ZoneInfo();
	zi->setNumZones(NUM_ZONES);
	debug("obtainZoneInfo nzones=")
	debug(zi->getNumZones())
	zi->initZone(0, "H1", 0, 0, 0, 0, 0, true);
	zi->initZone(1, "BB", 5, 5, 35, 30, WALL_TOP, false);
	zi->initZone(2, "MB", 40, 5, 40, 22, WALL_TOP, false);
	zi->initZone(3, "FB", 5, 35, 35, 35, WALL_BOTTOM, false);
	zi->initZone(4, "SB", 70, 27, 20, 20, WALL_RIGHT, false); // TEMP
	zi->initZone(5, "B1", 40, 50, 30, 15, WALL_BOTTOM, false);

	zi->initZone(6, "DR", 5, 85, 35, 30, WALL_TOP, false);
	zi->initZone(7, "KI", 40, 85, 45, 30, WALL_TOP, false);
	zi->initZone(8, "LO", 5, 115, 35, 35, WALL_LEFT, false); // TEMP
	zi->initZone(9, "Hg", 0, 0, 0, 0, 0, true);
	zi->initZone(10, "Bg", 55, 115, 15, 20, 0, false);
	zi->initZone(11, "GA", 70, 115, 15, 35, WALL_BOTTOM, false);
	return *zi;
}
void periodic_stuff()
{

}

void toggleScreen()
{
	Screen *s;
	if (currentScreen == &statusScreen)
	{
		s = &monitorScreen;
	}
	else
	{
		s = &statusScreen;
	}
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
	debug(modePinPressed);debug("\n\r");
	if (modePinPressed)
	{
		toggleScreen();
		delay(50); //debounce
	}
}

void handleHeartBeat(char *message)
{
	if (heartbeatSymbol == 'x')
		heartbeatSymbol = '+';
	else
		heartbeatSymbol = 'x';
}

void dataReadyHandler(const uint8_t rf24_status)
{
	char receivePayload[32];
	bool isFIFOEmpty = false; // or we wouldn't be here
	do
	{
		uint8_t len = 0;
		status.nMsgsReceived++;
		len = radio.getDynamicPayloadSize();
		if (len > 32)
		{
			status.nMsgsDiscarded++;
			radio.flush_rx();
			receivePayload[0] = 0;
			return;
		}
		else
		{
			isFIFOEmpty = radio.read(receivePayload, len);
			receivePayload[len] = 0; //terminate

			// decipher message
			if (receivePayload[0] == 'S')
			{
				uint8_t zone_num, state;
				char sensor_type;
				sscanf(receivePayload, "S%hhd %c %hhd", &zone_num, &sensor_type,
						&state);
//				zone_num = receivePayload[1] - '0';
//				sensor_type = receivePayload[3];
//				sensor_num = receivePayload[4] - '0';
//				state = receivePayload[6] - '0';
				zoneInfo.setZoneStatus(zone_num, sensor_type, state);
			}
			else if (receivePayload[0] == 'H')
			{
				handleHeartBeat(receivePayload);
			}
		}
	} while (!isFIFOEmpty);

}
void dataSentHandler(const uint8_t rf24_status)
{
	printf("TODO: dataSentHandler(%02x)\n\r", rf24_status);
}
void maxRetransmitsHandler(const uint8_t rf24_status)
{
	printf("TODO: maxRetransmitsHandler(%02x)\n\r", rf24_status);
}
void check_radio(void)
{
	radio_interrupt = true;
}

void getRadioInput()
{
	if (radio_interrupt)
	{
		radio.dispatchAndClearInterrupts(dataReadyHandler, dataSentHandler,
				maxRetransmitsHandler);

		radio_interrupt = false;
	}
}
void updateDisplay()
{
//noInterrupts();
	currentScreen->refresh(&zoneInfo, &status);
	tft.setCursor(0, 0);
	tft.setTextColor(ST7735_YELLOW, ST7735_BLACK);
	tft.print(heartbeatSymbol);
//interrupts();

}

void loop()
{
	static unsigned long lastStatusPrint = millis();
	TIMEIT(handleKeyInput, handleKeyInput());
	TIMEIT(getRadioInput, getRadioInput());
	if (lastStatusPrint + 1000 < millis())
	{
		periodic_stuff();
		lastStatusPrint = millis();
		zoneInfo.markInactiveDirty();
	}
	TIMEIT(updateDisplay, updateDisplay());

}
