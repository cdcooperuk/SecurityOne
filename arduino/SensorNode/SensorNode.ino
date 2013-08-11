#include <SPI.h>

#include <RF24.h>
#include <RF24_config.h>
#include <nRF24L01.h>

// Do not remove the include below
#include "SensorNode.h"
#include "printf.h"

// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
const int pin_led = 13;
const int pin_contact1 = 2;

RF24 radio(8, 7);

RoomState roomState;

const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

// the setup routine runs once when you press reset:
void setup() {
	Serial.begin(9600);
	printf_begin();

	pinMode(pin_led, OUTPUT);
	pinMode(pin_contact1, INPUT_PULLUP);
	digitalWrite(pin_contact1, HIGH);

	printf("Sensor: %i\n\r", roomState.sensor_id);
	printf("*** PRESS 'T' to begin transmitting to the other node\n\r");

	Serial.println("Initializing radio");
	radio.begin();
	radio.setRetries(15, 15);
	radio.setDataRate(RF24_1MBPS);
	radio.setPALevel(RF24_PA_LOW);
	radio.setChannel(76);
	radio.setCRCLength(RF24_CRC_16);
	radio.enableDynamicPayloads();

	radio.openWritingPipe(pipes[0]);
	radio.openReadingPipe(1, pipes[1]);
	radio.startListening();

	//
	// Dump the configuration of the rf unit for debugging
	//

	radio.printDetails();
}

// the loop routine runs over and over again forever:
void loop() {
	//roomState.contact1_alert = !roomState.contact1_alert;
	digitalWrite(pin_led, HIGH);

	roomState.contact1_alert = digitalRead(pin_contact1);

	// First, stop listening so we can talk.
	radio.stopListening();
	char s[10];
	printf("Now sending RoomState %s...", roomState.toString(s));
	bool ok = radio.write(&s, strlen(s));

	if (ok)
		printf("ok...");
	else
		printf("failed.\n\r");

	// Now, continue listening
	radio.startListening();

	// Wait here until we get a response, or timeout (250ms)
	unsigned long started_waiting_at = millis();
	bool timeout = false;
	while (!radio.available() && !timeout)
		if (millis() - started_waiting_at > 200)
			timeout = true;

	// Describe the results
	if (timeout) {
		printf("Failed, response timed out.\n\r");
	} else {
		// Grab the response, compare, and send to debugging spew
		char ack;
		radio.read(&ack, sizeof(char));

		// Spew it
		printf("Got response %c \n\r", ack);
	}
	digitalWrite(pin_led, LOW);

	// Try again 1s later
	delay(1000);

}

