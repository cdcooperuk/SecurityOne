#include <RoomState.h>
#include <SOCommon.h>

#include <SPI.h>

#include <RF24.h>
#include <RF24_config.h>
#include <nRF24L01.h>
#include <EEPROM.h>

// Do not remove the include below
#include "SensorNode.h"
#include "printf.h"

// id of this node is burned into eeprom- this way same prog can be used on multiple avrs.
const int NODE_ID_EEPROM_ADDRESS=0;

const int pin_contact1 = 2;
const int pin_contact2 = NOT_A_PIN;
const int pin_contact3 = NOT_A_PIN;
const int pin_pir = 3;

RF24 radio(8, 6);

uint8_t myid= EEPROM.read(NODE_ID_EEPROM_ADDRESS);
RoomState roomState(myid);

//my address = base + my id
uint64_t my_addr = base_addr + myid;

// message counter
static int msgId;

bool isPin(const uint8_t p) {
	return p != NOT_A_PIN;
}

void init_input_pullup(const uint8_t pin) {
	if (!isPin(pin)) return;

	pinMode(pin, INPUT_PULLUP);
	digitalWrite(pin, HIGH);
}


void setup() {
	Serial.begin(115200);
	printf_begin();

	init_input_pullup(pin_contact1);
	init_input_pullup(pin_contact2);
	init_input_pullup(pin_contact3);
	init_input_pullup(pin_pir);

	printf("Sensor: %i\n\r", roomState.node_id);
	printf("PROTOCOL_VERSION: %i\n\r", PROTOCOL_VERSION);

	Serial.println("Initializing radio");
	radio.begin();
	radio.setRetries(15, 15);
	radio.setDataRate(RF24_1MBPS);
	radio.setPALevel(RF24_PA_HIGH);
	radio.setChannel(RF24_CHANNEL);
	radio.setCRCLength(RF24_CRC_16);
	radio.enableDynamicPayloads();

	radio.openWritingPipe(my_addr);
//	radio.openWritingPipe(display_addr);

	//
	// Dump the configuration of the rf unit for debugging
	//
	radio.printDetails();

}

void loop() {

	// check inputs contact -- rely on NOT_A_PIN returning 0

	roomState.contact_alert[0] = isPin(pin_contact1) && digitalRead(pin_contact1);
	roomState.contact_alert[1] = isPin(pin_contact2) && digitalRead(pin_contact2);
	roomState.contact_alert[2] = isPin(pin_contact3) && digitalRead(pin_contact3);
	roomState.pir_alert = isPin(pin_pir) && digitalRead(pin_pir);
	roomState.msgId=msgId++;

	char s[30];
	roomState.toString(s);
	printf("Now sending RoomState '%s' (%d) ...", s, strlen(s));
//	radio.startWrite(&s, strlen(s), true);
	bool ok = radio.write(&s, strlen(s), false);

	if (ok)
		printf("ok...\n");
	else
		printf("failed.\n\r");


	radio.print_status();

	// Try again 1s later
	delay(1000);

}

