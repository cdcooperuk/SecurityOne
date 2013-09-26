/*
 * RoomState.cpp
 *
 *  Created on: 9 Aug 2013
 *      Author: colin
 */

#include "RoomState.h"
#include <inttypes.h>


static const char* FMT="[V%02d #%02d A%1d B%1d C%1d P%1d #%d]";

RoomState::RoomState(const int nodeId) :
		protocol_version(PROTOCOL_VERSION), node_id(nodeId) {
	contact_alert[0]=false;
	contact_alert[1]=false;
	contact_alert[2]=false;
	pir_alert=false;
}
RoomState::RoomState(char* serialized_state) {
	int pvInt;
	int nidInt;
	int c1, c2, c3, p;
	int id;
	sscanf(serialized_state, FMT, &pvInt, &nidInt, &c1,
			&c2, &c3, &p, &id);

	protocol_version = (uint8_t) pvInt;
	node_id = (uint8_t) nidInt;
	contact_alert[0] = c1;
	contact_alert[1] = c2;
	contact_alert[2] = c3;
	pir_alert = p;
	msgId=id;
}

RoomState::~RoomState() {
	// TODO Auto-generated destructor stub
}

char* RoomState::toString(char* buf) {
	sprintf(buf, FMT, protocol_version, node_id,
			contact_alert[0], contact_alert[1], contact_alert[2], pir_alert, msgId);
	return buf;

}

bool RoomState::isAlert() {
	return contact_alert[0] || contact_alert[1] || contact_alert[2] || pir_alert;
}

