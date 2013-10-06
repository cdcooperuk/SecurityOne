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

#include <stdio.h>
#include "RF24.h"
#include "../SOcommon/SOcommon.h"
#include "../SOcommon/RoomState.h"
#include <time.h>

using namespace std;

const int MAX_MSGS = 3;
const int OUT_MSG_LEN = 32;

enum mode
{
	operational = 0, test, manual
};

// CE and CSN pins On header using GPIO numbering (not pin numbers)
RF24 radio("/dev/spidev0.0", 8000000, 25);  // Setup for GPIO 25 CE

bool sendToDisplay(char buf[][OUT_MSG_LEN], const int nMsgs)
{
	bool allSentOk = true;

	time_t clk = time(NULL);
	char *hr_clk = ctime(&clk);
	// remove trailing \n
	int l = strlen(hr_clk);
	hr_clk[l - 1] = '\0';

	radio.stopListening();

	for (int i = 0; i < nMsgs; i++)
	{
		int len = strlen(buf[i]);
		printf("%d %s '%s' [%d]", (int) clk, hr_clk, buf[i], len);

		bool thisMsgOk = radio.write(buf[i], len);
		allSentOk &= thisMsgOk;
		if (thisMsgOk)
		{
			printf("ok\n\r");
		}
		else
		{
			printf("send error\n\r");
		}
	}

	radio.startListening();

	return allSentOk;
}
/**
 * degenerate case for single message
 */
bool sendToDisplay(char s[])
{
	char buf[1][OUT_MSG_LEN];
	strcpy(buf[0], s);

	return sendToDisplay(buf, 1);
}
void prepareSensorStateMessage(char *buf, const uint8_t node_id,
		const char sensor_id, const bool alerted)
{
	sprintf(buf, "S%d %c %1d", node_id, sensor_id, alerted);
}

void setup(void)
{
	printf("PROTOCOL_VERSION: %i\n\r", PROTOCOL_VERSION);

//
// Refer to RF24.h or nRF24L01 DS for settings
	radio.begin();
	radio.enableDynamicPayloads();
	radio.setAutoAck(1);
	radio.setRetries(3, 15);
	radio.setDataRate(CFG_RF24_DATA_RATE);
	radio.setPALevel(RF24_PA_MAX);
	radio.setChannel(CFG_RF24_CHANNEL);
	radio.setCRCLength(CFG_RF24_CRC_LENGTH);

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

	printf("+++ RADIO DETAILS +++\n\r");
	radio.printDetails();
	printf("---------------------\n\r");
	printf("\n\rOutput below : \n\r");
	usleep(1000);
}

void send_test_messages()
{
	char msg[OUT_MSG_LEN];
	char sensor_types[] =
	{ 'C', 'W', 'P' };
	for (int zoneno = 0; zoneno < NUM_ZONES; zoneno++)
	{
		for (int sensor_type_index = 0; sensor_type_index < 3;
				sensor_type_index++)
		{
			char c = sensor_types[sensor_type_index];
			bool sentok = false;
			while (!sentok)
			{
				prepareSensorStateMessage(msg, zoneno, c, true);
				sentok = sendToDisplay(msg);
			}

			usleep(1000000);
			sentok = false;
			while (!sentok)
			{
				prepareSensorStateMessage(msg, zoneno, c, false);
				sentok = sendToDisplay(msg);
			}
		}
	}
}

void read_and_send_message()
{
	printf("Enter message: ");
	size_t buflen = 80;
	char *msg = (char *) malloc(buflen);
	if (getline(&msg, &buflen, stdin) != -1)
	{
		bool sentok = false;
		while (!sentok)
		{
			sentok = sendToDisplay(msg);
		}
	}

}

void accept_data_and_send_to_display()
{
	char receivePayload[32];
	uint8_t pipe = 0;

//IF_SERIAL_DEBUG(printf("in loop\n"));
	while (radio.available(&pipe))
	{

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

//		printf("%d %s node=%d A=%1d B=%1d C=%1d P=%1d\n", clk, hr_clk,
//				rs.node_id, rs.contact1_alert, rs.contact2_alert,
//				rs.contact3_alert, rs.pir_alert);

		char buf[MAX_MSGS][OUT_MSG_LEN];
		int bufIndex = 0;
		prepareSensorStateMessage(buf[bufIndex++], rs.node_id, 'C',
				rs.contact_alert);
		prepareSensorStateMessage(buf[bufIndex++], rs.node_id, 'W',
				rs.window_broken);
		prepareSensorStateMessage(buf[bufIndex++], rs.node_id, 'P',
				rs.pir_alert);
		sendToDisplay(buf, bufIndex);

		if (rs.isAlert())
		{
			IF_SERIAL_DEBUG(printf("\a***ALERT***\n\r"));
		}

		usleep(20);
	}

	static int heartbeat_counter;
	char msg[32];
	sprintf(msg, "HHeartbeat %d", heartbeat_counter++);
	sendToDisplay(msg);
	//radio.print_status();

}

void loop(enum mode mymode)
{

	switch (mymode)
	{
	case (test):
		send_test_messages();
		break;
	case (manual):
		read_and_send_message();
		break;
	default:
		accept_data_and_send_to_display();
	}


}
int main(int argc, char** argv)
{
	enum mode mymode = operational;
	int c;
	while ((c = getopt(argc, argv, "tm")) != -1)
		switch (c)
		{
		case 't':
			mymode = test;
			break;
		case 'm':
			mymode = manual;
			break;
		default:
			abort();
		}
	setup();
	while (1)
	{
		loop(mymode);
		sleep(1);
	}

	return 0;
}

