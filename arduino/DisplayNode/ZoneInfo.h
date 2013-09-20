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
	char name[3];
	uint8_t x, y, w, h;
	bool nodisplay;
	bool contact_alert[3] ;
	bool pir_alert ;
};

class ZoneInfo {
public:
	ZoneInfo();
	virtual ~ZoneInfo();

	uint8_t getNumZones();
	void setNumZones(uint8_t n);
	void initZone(int zoneNum, const char *name, uint8_t x, uint8_t y,
			uint8_t w, uint8_t h, bool nodisplay);

	void setZoneStatus(uint8_t zone_num, char sensor_type, uint8_t sensor_num,
			uint8_t state);

	bool is_dirty();
	void markDirty(bool d);

	struct Zone *zones;
private:
	uint8_t m_numzones;
	char* m_name;
	bool m_changed =false;

};

#endif /* __ZONEINFO_H__ */
