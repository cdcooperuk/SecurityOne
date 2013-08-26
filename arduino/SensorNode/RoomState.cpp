/*
 * RoomState.cpp
 *
 *  Created on: 9 Aug 2013
 *      Author: colin
 */

#include "RoomState.h"
#include <inttypes.h>


static const char* FMT="[V%02d #%02d A%1d B%1d C%1d P%1d]";
static uint8_t PROTOCOL_VERSION=2;

RoomState::RoomState(const int nodeId) :
		protocol_version(PROTOCOL_VERSION), sensor_id(nodeId), contact1_alert(false), contact2_alert(
				false), contact3_alert(false), pir_alert(false) {
}
RoomState::RoomState(char* serialized_state) :
		contact1_alert(false), contact2_alert(false), contact3_alert(false), pir_alert(
				false) {
	int pvInt;
	int sidInt;
	int c1, c2, c3, p;
	sscanf(serialized_state, FMT, &pvInt, &sidInt, &c1,
			&c2, &c3, &p);

	protocol_version = (uint8_t) pvInt;
	sensor_id = (uint8_t) sidInt;
	contact1_alert = c1;
	contact2_alert = c2;
	contact3_alert = c3;
	pir_alert = p;
}

RoomState::~RoomState() {
	// TODO Auto-generated destructor stub
}

char* RoomState::toString(char* buf) {
	sprintf(buf, FMT, protocol_version, sensor_id,
			contact1_alert, contact2_alert, contact3_alert, pir_alert);
	return buf;

}

bool RoomState::isAlert() {
	return contact1_alert || contact2_alert || contact3_alert || pir_alert;
}


uint8_t RoomState::getCurrentProtocolVersion(){
	return PROTOCOL_VERSION;
}
