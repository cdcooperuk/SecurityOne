/*
 * ZoneInfo.cpp
 *
 *  Created on: 10 Sep 2013
 *      Author: colin
 */

#include "ZoneInfo.h"
#include <stdio.h>
#include <string.h>
#include <HardwareSerial.h>

ZoneInfo::ZoneInfo() {
	// TODO Auto-generated constructor stub

}

ZoneInfo::~ZoneInfo() {
	// TODO Auto-generated destructor stub
}

uint8_t ZoneInfo::getNumZones() {
	return m_numzones;
}

void ZoneInfo::setNumZones(uint8_t n) {
	m_numzones = n;
}
void ZoneInfo::initZone(int zoneNum, const char *name, uint8_t x, uint8_t y,
		uint8_t w, uint8_t h, bool nodisplay) {

	struct Zone *z = &zones[zoneNum];
	strcpy(z->name, name);
	z->x = x;
	z->y = y;
	z->w = w;
	z->h = h;
	z->contact_alert[0] = false;
	z->contact_alert[1] = false;
	z->contact_alert[2] = false;
	z->pir_alert = false;
	z->nodisplay = nodisplay;
}

void ZoneInfo::setZoneStatus(uint8_t zone_num, char sensor_type,
		uint8_t sensor_num, uint8_t status) {

	if (zone_num < 0 || zone_num >= m_numzones)
		return;

	bool prev = false;
	struct Zone *z = &zones[zone_num];
	switch (sensor_type) {
	case 'c':
		prev = z->contact_alert[sensor_num - 1];
		z->contact_alert[sensor_num - 1] = status ? true : false;
		if (status) {
			printf("\tsetting zone %hhd contact %d to %d\n", zone_num,
					sensor_num, status);
		}
		break;
	case 'p':
		prev = z->pir_alert;
		z->pir_alert = status ? true : false;
		if (status) {
			printf("\tsetting zone %hhd pir to %d\n", zone_num, status);
		}
		break;
	}

	bool changed = (prev != status);
	z->dirty |= changed;
	m_dirty |= changed;
}

bool ZoneInfo::is_dirty() {
	return m_dirty;
}

void ZoneInfo::markDirty(bool b) {
	m_dirty = b;
	for (int i = 0; i < m_numzones; i++) {
		zones[i].dirty = b;
	}
}
