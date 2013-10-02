/* 
 *
 *  Listen to all sensor nodes, and record their status.
 *  
 *  CE is connected to GPIO25
 *  CSN is connected to GPIO8 
 *
 *  Refer to RPi docs for GPIO numbers
 *
 *  Author : Stanley Seow
 *  e-mail : stanleyseow@gmail.com
 *  date   : 4th Apr 2013
 *
 */

#include <cstdlib>
#include <iostream>
#include "RF24.h"
#include "../SOcommon/SOcommon.h"

using namespace std;


bool listen = true;

// CE and CSN pins On header using GPIO numbering (not pin numbers)
RF24 radio("/dev/spidev0.0", 8000000, 25);  // Setup for GPIO 25 CE

bool sendToDisplay(const char *buf) {
	bool sentOk;

	printf("sending '%s' ... ", buf);
	radio.stopListening();
	sentOk = radio.write(buf, strlen(buf));
	radio.startListening();

	return sentOk;
}

void setup(void) {

	//
	// Refer to RF24.h or nRF24L01 DS for settings
	radio.begin();
	radio.enableDynamicPayloads();
	radio.setAutoAck(1);
	radio.setRetries(15, 15);
	radio.setDataRate(CFG_RF24_DATA_RATE);
	radio.setPALevel(RF24_PA_MAX);
	radio.setChannel(CFG_RF24_CHANNEL);
	radio.setCRCLength(CFG_RF24_CRC_LENGTH);

	// Open 6 pipes for readings ( 5 plus pipe0, also can be used for reading )
	radio.openWritingPipe(display_addr);
	radio.openReadingPipe(1, pipes[1]);
	//
	// Dump the configuration of the rf unit for debugging
	//

	// Start Listening
	radio.startListening();

	printf("+++ RADIO DETAILS +++\n\r");
	radio.printDetails();
	printf("---------------------\n\r");
	printf("\n\rOutput below : \n\r");
	usleep(1000);
}

void loop(void) {
	if (listen) {
		char receivePayload[32];
		uint8_t pipe = 0;

		//IF_SERIAL_DEBUG(printf("in loop\n"));
		while (radio.available(&pipe)) {

			uint8_t len = radio.getDynamicPayloadSize();
			// printf("avail %d data ",len);
			radio.read(receivePayload, len);
			receivePayload[len] = 0;
			receivePayload[len] = 0; //terminate
			printf("read on pipe %d '%s'\n\r", pipe, receivePayload);
		}
		sleep(1);
	}

	static int iter = 0;
	char msg[32];
	sprintf(msg, "test %d", iter++);
	bool ok = sendToDisplay(msg);
	printf(ok ? "ok\n\r" : "error\n\r");
}

int main(int argc, char** argv) {
	if (argc > 1)
		if (strcmp("nolisten", argv[1]) == 0)
			listen = false;
	if (listen)
		printf("listening\n\r");
	else
		printf("not listening\n\r");

	setup();
	while (1) {
		loop();
		sleep(1);
	}

	return 0;
}

