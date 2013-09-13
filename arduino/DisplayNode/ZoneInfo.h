/*
 * ZoneInfo.h
 *
 *  Created on: 10 Sep 2013
 *      Author: colin
 */

#ifndef __ZONEINFO_H__
#define __ZONEINFO_H__

#include <stdint.h>

struct Zone {
	const char *name;
	uint8_t x, y, w, h;
	bool nodisplay;
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

#endif /* __ZONEINFO_H__ */
