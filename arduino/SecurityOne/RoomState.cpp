/*
 * RoomState.cpp
 *
 *  Created on: 9 Aug 2013
 *      Author: colin
 */

#include "RoomState.h"
#include <inttypes.h>

RoomState::RoomState() : protocol_version(1), sensor_id(1), 
			contact1_alert(false),
			contact2_alert(false),
			contact3_alert(false),
			pir_alert(false)
			{}
RoomState::RoomState(char* serialized_state) {
	int pvInt;
	int sidInt;
	scanf("(%02d%02d----)",	&pvInt, &sidInt);
	protocol_version = (uint8_t)pvInt;
	sensor_id = (uint8_t)sidInt;
}

RoomState::~RoomState() {
	// TODO Auto-generated destructor stub
}

char* RoomState::toString(char* buf) {
	sprintf(buf,"(%02d%02d----)",protocol_version, sensor_id);
	return buf;

}
