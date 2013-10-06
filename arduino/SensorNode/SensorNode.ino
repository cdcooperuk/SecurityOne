#include <RoomState.h>
#include <SOcommon.h>

#include <SPI.h>

#include <RF24.h>
#include <RF24_config.h>
#include <nRF24L01.h>
#include <EEPROM.h>

// Do not remove the include below
#include "SensorNode.h"
#include "printf.h"

// id of this node is burned into eeprom- this way same prog can be used on multiple avrs.
const int NODE_ID_EEPROM_ADDRESS = 0;

const int MAX_CONTACTS = 3;
const int pin_contacts[] =
{ 2, NOT_A_PIN, NOT_A_PIN };
const int pin_window_broken = 7;

const int pin_pir = 3;

RF24 radio(8, 6);

uint8_t myid = EEPROM.read(NODE_ID_EEPROM_ADDRESS);
RoomState roomState(myid);

//my address = base + my id
uint64_t my_addr = CFG_BASE_ADDR + myid;

// message counter
static int msgId;

bool isPin(const uint8_t p)
{
	return p != NOT_A_PIN;
}

void init_input_pullup(const uint8_t pin)
{
	if (!isPin(pin))
		return;

	pinMode(pin, INPUT_PULLUP);
	digitalWrite(pin, HIGH);
}

void setup()
{
	Serial.begin(115200);
	printf_begin();

	for (int i = 0; i < MAX_CONTACTS; i++)
		init_input_pullup(pin_contacts[i]);
	init_input_pullup(pin_pir);
	init_input_pullup(pin_pir);

	printf("Sensor: %i\n\r", roomState.node_id);
	printf("PROTOCOL_VERSION: %i\n\r", PROTOCOL_VERSION);

	Serial.println("Initializing radio");
	radio.begin();
	radio.setRetries(15, 15);
	radio.setDataRate(CFG_RF24_DATA_RATE);
	radio.setPALevel(RF24_PA_MAX);
	radio.setChannel(CFG_RF24_CHANNEL);
	radio.setCRCLength(CFG_RF24_CRC_LENGTH);
	radio.enableDynamicPayloads();

	radio.openWritingPipe(my_addr);
//	radio.openWritingPipe(display_addr);

//
// Dump the configuration of the rf unit for debugging
//
	radio.printDetails();

}

void loop()
{

	// check inputs contact -- rely on NOT_A_PIN returning 0
	roomState.contact_alert = false;
	for (int i = 0; i < MAX_CONTACTS; i++)
		if (isPin(pin_contacts[i]) && digitalRead(pin_contacts[i]))
		{
			roomState.contact_alert = true;
		}

	roomState.window_broken = isPin(pin_window_broken)
			&& !digitalRead(pin_window_broken);

	roomState.pir_alert = isPin(pin_pir) && digitalRead(pin_pir);

	roomState.msgId = msgId++;

	char s[30];
	roomState.toString(s);
	printf("Now sending RoomState '%s' (%d) ...", s, strlen(s));
//	radio.startWrite(&s, strlen(s), true);
	bool ok = radio.write(&s, strlen(s), false);

	if (ok)
		printf("ok...\n\r");
	else
		printf("failed.\n\r");

	radio.print_status();

	// Try again later
	delay(1000);

}

