/*
 * RoomState.cpp
 *
 *  Created on: 9 Aug 2013
 *      Author: colin
 */

#include "RoomState.h"
#include <inttypes.h>

static const char* FMT = "[V%02d #%02d C%1d W%1d P%1d #%d]";

RoomState::RoomState(const int nodeId) :
		protocol_version(PROTOCOL_VERSION), node_id(nodeId)
{
	contact_alert = false;
	window_broken = false;
	pir_alert = false;
}
RoomState::RoomState(char* serialized_state)
{
	int pvInt;
	int nidInt;
	int c, w, p;
	int id;
	sscanf(serialized_state, FMT, &pvInt, &nidInt, &c, &w, &p, &id);

	protocol_version = (uint8_t) pvInt;
	node_id = (uint8_t) nidInt;
	contact_alert = c;
	window_broken = w;
	pir_alert = p;
	msgId = id;
}

RoomState::~RoomState()
{
	// TODO Auto-generated destructor stub
}

char* RoomState::toString(char* buf)
{
	sprintf(buf, FMT, protocol_version, node_id, contact_alert,
			window_broken, pir_alert, msgId);
	return buf;

}

bool RoomState::isAlert()
{
	return contact_alert|| window_broken || pir_alert;
}

