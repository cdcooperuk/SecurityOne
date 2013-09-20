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

using namespace std;

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t display_addr = 0xF0F0F0F087LL;
// First pipe is for display, rest are for sensors
const uint64_t pipes[6] = { display_addr, 0xF0F0F0F001LL, 0xF0F0F0F002LL,
		0xF0F0F0F003LL, 0xF0F0F0F004, 0xF0F0F0F005 };

bool listen = true;

// CE and CSN pins On header using GPIO numbering (not pin numbers)
RF24 radio("/dev/spidev0.0", 8000000, 25);  // Setup for GPIO 25 CE

bool sendToDisplay(const char *buf) {
	bool sentOk;

	printf("send '%s' ... ", buf);
	radio.stopListening();
	usleep(500000);
	sentOk = radio.write(buf, strlen(buf));
	usleep(500000);
	radio.startListening();
	usleep(500000);

	return sentOk;
}

void setup(void) {

	//
	// Refer to RF24.h or nRF24L01 DS for settings
	radio.begin();
	radio.enableDynamicPayloads();
	radio.setAutoAck(1);
	radio.setRetries(15, 15);
	radio.setDataRate(RF24_1MBPS);
	radio.setPALevel(RF24_PA_MAX);
	radio.setChannel(0x4c);
	radio.setCRCLength(RF24_CRC_16);

	// Open 6 pipes for readings ( 5 plus pipe0, also can be used for reading )
	radio.openWritingPipe(display_addr);
	radio.openReadingPipe(1, pipes[1]);
	//
	// Dump the configuration of the rf unit for debugging
	//

	// Start Listening
	radio.startListening();

	printf("+++ RADIO DETAILS +++\n");
	radio.printDetails();
	printf("---------------------\n");
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
			printf("read on pipe %d '%s'\n", pipe, receivePayload);
		}
		sleep(1);
	}

	static int iter = 0;
	char msg[32];
	sprintf(msg, "test %d", iter++);
	bool ok = sendToDisplay(msg);
	printf(ok ? "ok\n" : "error\n");
}

int main(int argc, char** argv) {
	if (argc > 1)
		if (strcmp("nolisten", argv[1]) == 0)
			listen = false;
	if (listen)
		printf("listening\n");
	else
		printf("not listening\n");

	setup();
	while (1) {
		loop();
		sleep(1);
	}

	return 0;
}

