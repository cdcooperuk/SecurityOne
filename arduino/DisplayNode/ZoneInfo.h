/*
 * ZoneInfo.h
 *
 *  Created on: 10 Sep 2013
 *      Author: colin
 */

#ifndef __ZONEINFO_H__
#define __ZONEINFO_H__

#include <stdint.h>

#define WALL_TOP 1
#define WALL_BOTTOM 2
#define WALL_LEFT 4
#define WALL_RIGHT 8

#define INACTIVE_TIMEOUT_MS 60000

struct Zone
{
	char name[3];
	uint8_t x, y, w, h;
	bool nodisplay;
	volatile bool contact_alert;
	volatile bool window_broken;
	volatile bool pir_alert;
	volatile bool dirty;
	// bits indicating which walls have contact sensors
	uint8_t contact_walls;
	long lastUpdatedMs;
};

class ZoneInfo
{
public:
	ZoneInfo();
	virtual ~ZoneInfo();

	uint8_t getNumZones();
	void setNumZones(uint8_t n);
	void initZone(int zoneNum, const char *name, uint8_t x, uint8_t y,
			uint8_t w, uint8_t h, uint8_t contact_walls, bool nodisplay);

	void setZoneStatus(uint8_t zone_num, char sensor_type,
			uint8_t state);

	bool is_dirty();
	void markDirty(bool d);

	struct Zone *zones;
private:
	uint8_t m_numzones;
	volatile bool m_dirty;

};

#endif /* __ZONEINFO_H__ */
