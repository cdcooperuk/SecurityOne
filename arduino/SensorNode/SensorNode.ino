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

const int pin_led = 13;
const int pin_contact1 = 2;
const int pin_contact2 = NOT_A_PIN;
const int pin_contact3 = NOT_A_PIN;
const int pin_pir = 3;

RF24 radio(8, 7);

RoomState roomState(EEPROM.read(NODE_ID_EEPROM_ADDRESS));

const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0E1LL };
//const uint64_t pipes[2] = { 0xF0F0F0F0D2LL, 0xF0F0F0F0E1LL };

bool isPin(const uint8_t p) {
	return p != NOT_A_PIN;
}

void init_input_pullup(const uint8_t pin) {
	if (!isPin(pin)) return;

	pinMode(pin, INPUT_PULLUP);
	digitalWrite(pin, HIGH);
}


void setup() {
	Serial.begin(9600);
	printf_begin();

	pinMode(pin_led, OUTPUT);
	init_input_pullup(pin_contact1);
	init_input_pullup(pin_contact2);
	init_input_pullup(pin_contact3);
	init_input_pullup(pin_pir);

	printf("Sensor: %i\n\r", roomState.sensor_id);
	printf("PROTOCOL_VERSION: %i\n\r", RoomState::getCurrentProtocolVersion());

	Serial.println("Initializing radio");
	radio.begin();
	radio.setRetries(15, 15);
	radio.setDataRate(RF24_1MBPS);
	radio.setPALevel(RF24_PA_LOW);
	radio.setChannel(76);
	radio.setCRCLength(RF24_CRC_16);
	radio.enableDynamicPayloads();

	radio.openWritingPipe(pipes[0]);

	//
	// Dump the configuration of the rf unit for debugging
	//
	radio.printDetails();

}

void loop() {
	digitalWrite(pin_led, HIGH);

	// check inputs contact -- rely on NOT_A_PIN returning 0
	roomState.contact1_alert = digitalRead(pin_contact1);
	roomState.contact2_alert = isPin(pin_contact2) && digitalRead(pin_contact2);
	roomState.contact3_alert = isPin(pin_contact3) && digitalRead(pin_contact3);
	roomState.pir_alert = digitalRead(pin_pir);

	char s[30];
	roomState.toString(s);
	printf("Now sending RoomState '%s' (%d) ...", s, strlen(s));
	bool ok = radio.write(&s, strlen(s), false);

	if (ok)
		printf("ok...\n");
	else
		printf("failed.\n\r");

	digitalWrite(pin_led, LOW);

	// Try again 1s later
	delay(1000);

}

