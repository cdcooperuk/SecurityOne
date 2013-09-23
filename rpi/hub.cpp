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
#include "../SOcommon/RoomState.h"
#include <time.h>

using namespace std;

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t display_addr = 0xF0F0F0F087LL;
// First pipe is for display, rest are for sensors
const uint64_t pipes[6] = { display_addr, 0xF0F0F0F001LL, 0xF0F0F0F002LL,
		0xF0F0F0F003LL, 0xF0F0F0F004, 0xF0F0F0F005 };


// CE and CSN pins On header using GPIO numbering (not pin numbers)
RF24 radio("/dev/spidev0.0", 8000000, 25);  // Setup for GPIO 25 CE

bool sendToDisplay(const char *buf) {
	bool sentOk;

	radio.stopListening();
	sentOk = radio.write(buf, strlen(buf));
	radio.startListening();

	return sentOk;
}
void notifySensorState(const time_t clk, const char *hr_clk,
		const uint8_t node_id, const char* sensor_id, const bool alerted) {

	char buf[32];

	sprintf(buf, "S%d %s %1d", node_id, sensor_id, alerted);
	printf("%d %s %s ", (int) clk, hr_clk, buf);

	if (sendToDisplay(buf)) {
		printf("ok\n");
	} else {
		printf("send error\n");
	}

}
void setup(void) {
	printf("PROTOCOL_VERSION: %i\n\r", PROTOCOL_VERSION);

	//
	// Refer to RF24.h or nRF24L01 DS for settings
	radio.begin();
	radio.enableDynamicPayloads();
	radio.setAutoAck(1);
	radio.setRetries(15, 15);
	radio.setDataRate(RF24_1MBPS);
	radio.setPALevel(RF24_PA_LOW);
	radio.setChannel(0x4c);
	radio.setCRCLength(RF24_CRC_16);

	// Open 6 pipes for readings ( 5 plus pipe0, also can be used for reading )
	radio.openWritingPipe(display_addr);
	radio.openReadingPipe(1, pipes[1]);
	radio.openReadingPipe(2, pipes[2]);
	radio.openReadingPipe(3, pipes[3]);
	radio.openReadingPipe(4, pipes[4]);
	radio.openReadingPipe(5, pipes[5]);

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
	char receivePayload[32];
	uint8_t pipe = 0;

	//IF_SERIAL_DEBUG(printf("in loop\n"));
	while (radio.available(&pipe)) {

		uint8_t len = radio.getDynamicPayloadSize();
		// printf("avail %d data ",len);
		radio.read(receivePayload, len);
		//printf("read %d data \n",len);
		//printf("read %c %c ...\n",receivePayload[0],receivePayload[1]);
		receivePayload[len] = 0; //terminate
		RoomState rs(receivePayload);

		// Display it on screen
		IF_SERIAL_DEBUG(
				char buf[80];
				printf("Recv: size=%i payload=%s pipe=%i",len,rs.toString(buf),pipe));

		time_t clk = time(NULL);
		char *hr_clk = ctime(&clk);
		// remove trailing \n
		int l = strlen(hr_clk);
		hr_clk[l - 1] = '\0';
//		printf("%d %s node=%d A=%1d B=%1d C=%1d P=%1d\n", clk, hr_clk,
//				rs.node_id, rs.contact1_alert, rs.contact2_alert,
//				rs.contact3_alert, rs.pir_alert);
		notifySensorState(clk, hr_clk, rs.node_id, "c1", rs.contact_alert[0]);
		notifySensorState(clk, hr_clk, rs.node_id, "c2", rs.contact_alert[1]);
		notifySensorState(clk, hr_clk, rs.node_id, "c3", rs.contact_alert[2]);
		notifySensorState(clk, hr_clk, rs.node_id, "p1", rs.pir_alert);

		if (rs.isAlert()) {
			IF_SERIAL_DEBUG(printf("\a***ALERT***\n"));
		}

		usleep(20);
	}

	sendToDisplay("Hnothing to report");
	radio.print_status();

}

int main(int argc, char** argv) {
	setup();
	while (1) {
		loop();
		sleep(1);
	}

	return 0;
}

