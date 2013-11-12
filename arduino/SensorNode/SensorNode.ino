// Do not remove the include below
#include "SensorNode.h"
#include "printf.h"

#include <RoomState.h>
#include <SOcommon.h>

#include <SPI.h>

#include <RF24.h>
#include <RF24_config.h>
#include <nRF24L01.h>
#include <EEPROM.h>
#include "LowPower.h" 
#define DEBUG true

const int MAX_CONTACTS = 3;

// id of this node is burned into eeprom- this way same prog can be used on multiple avrs.
const int NODE_ID_EEPROM_ADDRESS = 0;
const int PIR_PIN_EEPROM_ADDRESS = 1;
const int CONTACT_1_EEPROM_ADDRESS = 2;
const int CONTACT_2_EEPROM_ADDRESS = 3;
const int CONTACT_3_EEPROM_ADDRESS = 4;
const int WINDOW_BREAK_PIN_EEPROM_ADDRESS = 5;

const int pin_contacts[] =
{ EEPROM.read(CONTACT_1_EEPROM_ADDRESS), EEPROM.read(CONTACT_2_EEPROM_ADDRESS),
		EEPROM.read(CONTACT_3_EEPROM_ADDRESS) };
const int pin_window_broken = EEPROM.read(WINDOW_BREAK_PIN_EEPROM_ADDRESS);

const int pin_pir = EEPROM.read(PIR_PIN_EEPROM_ADDRESS);

RF24 radio(8, 6);

uint8_t myid = EEPROM.read(NODE_ID_EEPROM_ADDRESS);
RoomState roomState(myid);
volatile bool window_break_detected = false;

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

/*
 * voltmeter. see https://code.google.com/p/tinkerit/wiki/SecretVoltmeter
 */
int readVccMv()
{
	long result;
	// Read 1.1V reference against AVcc
	ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
	delay(2); // Wait for Vref to settle
	ADCSRA |= _BV(ADSC); // Convert
	while (bit_is_set(ADCSRA,ADSC))
		;
	result = ADCL;
	result |= ADCH << 8;
	result = 1126400L / result; // Back-calculate AVcc in mV
	return result;
}

void printSensorDetails()
{
	printf("contact pins");
	for (int i = 0; i < MAX_CONTACTS; i++)
	{
		printf(" %i", pin_contacts[i]);
	}
	printf("\n");
	printf("pir pin %i\n", pin_pir);
	printf("window break pin %i\n", pin_window_broken);
}

void windowBreakInterrupt()
{
	//Serial.println("*B*");
	window_break_detected = true;
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

	printSensorDetails();

	attachInterrupt(0, windowBreakInterrupt, RISING);

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

	roomState.window_broken = isPin(pin_window_broken) && window_break_detected;
	window_break_detected = false;

	roomState.pir_alert = isPin(pin_pir) && digitalRead(pin_pir);

	roomState.msgId = msgId++;

	roomState.batteryMv = readVccMv();

	char s[30];
	roomState.toString(s);
	if (DEBUG)
	{
		printf("Sending RoomState '%s' (%d) ...", s, strlen(s));
	}
	//	radio.startWrite(&s, strlen(s), true);
	bool ok = radio.write(&s, strlen(s), false);

	if (DEBUG)
	{
		if (ok)
			printf("ok...\n");
		else
			printf("failed.\n");
	}

	//	radio.print_status();

	// Try again later
	//delay(1000);
	//	LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF,
	//			SPI_OFF, USART0_OFF, TWI_OFF);
	if (DEBUG)
		printf("sleeping...\n");
	Serial.flush();
	radio.powerDown();
	LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF);
	if (DEBUG)
		printf("awake.\n");
	radio.powerUp();
	//delay(1000);
}

