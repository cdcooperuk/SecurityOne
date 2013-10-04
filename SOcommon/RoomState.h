/*
 * RoomState.h
 *
 *  Created on: 9 Aug 2013
 *      Author: colin
 */

#ifndef ROOMSTATE_H_
#define ROOMSTATE_H_
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define PROTOCOL_VERSION 4

class RoomState {

public:
	RoomState(const int nodeId);
	RoomState(char* serialized_state);
	virtual ~RoomState();
	char* toString(char* buf);
	bool isAlert();
    static uint8_t getCurrentProtocolVersion();
	uint8_t protocol_version;
	uint8_t node_id;
	bool contact_alert;
	bool window_broken;
	bool pir_alert;
	int msgId;
};

#endif /* ROOMSTATE_H_ */
