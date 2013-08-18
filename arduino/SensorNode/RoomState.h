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


class RoomState {

public:
	RoomState();
	RoomState(char* serialized_state);
	virtual ~RoomState();
	char* toString(char* buf);
	bool isAlert();
    static uint8_t getCurrentProtocolVersion();
	uint8_t protocol_version;
	uint8_t sensor_id;
	bool contact1_alert;
	bool contact2_alert;
	bool contact3_alert;
	bool pir_alert;
};

#endif /* ROOMSTATE_H_ */
