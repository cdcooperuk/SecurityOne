/*
 * ZoneInfo.h
 *
 *  Created on: 10 Sep 2013
 *      Author: colin
 */

#ifndef ZONEINFO_H_
#define ZONEINFO_H_

#include <stdint.h>

struct Zone {
	char *name;
	uint8_t x, y, w, h;
	bool fill;
};

class ZoneInfo {
public:
	ZoneInfo();
	virtual ~ZoneInfo();

	uint8_t getNumZones();
	void setNumZones(uint8_t n);
	struct Zone *zones;
private:
	uint8_t m_numzones;
	char* m_name;

};

#endif /* ZONEINFO_H_ */
