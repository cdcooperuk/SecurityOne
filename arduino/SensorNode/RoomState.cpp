/*
 * RoomState.cpp
 *
 *  Created on: 9 Aug 2013
 *      Author: colin
 */

#include "RoomState.h"
#include <inttypes.h>

RoomState::RoomState() :
		protocol_version(1), sensor_id(1), contact1_alert(false), contact2_alert(
				false), contact3_alert(false), pir_alert(false) {
}
RoomState::RoomState(char* serialized_state) :
		contact1_alert(false), contact2_alert(false), contact3_alert(false), pir_alert(
				false) {
	int pvInt;
	int sidInt;
	int c1, c2, c3, p;
	printf("constructor %s.\n", serialized_state);
	sscanf(serialized_state, "[%02d%02d%1d%1d%1d%1d]", &pvInt, &sidInt, &c1,
			&c2, &c3, &p);

	printf("scan completed\n");
	protocol_version = (uint8_t) pvInt;
	sensor_id = (uint8_t) sidInt;
	contact1_alert=c1;
	contact2_alert=c2;
	contact3_alert=c3;
	pir_alert=p;
}

RoomState::~RoomState() {
	// TODO Auto-generated destructor stub
}

char* RoomState::toString(char* buf) {
	sprintf(buf, "[%02d%02d%1d%1d%1d%1d]", protocol_version, sensor_id,
			contact1_alert, contact2_alert, contact3_alert, pir_alert);
	return buf;

}

bool RoomState::isAlert() {
	return contact1_alert || contact2_alert || contact3_alert || pir_alert;
}
