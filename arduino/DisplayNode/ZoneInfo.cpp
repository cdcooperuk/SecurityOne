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
//	char buf[32];
//	Serial.println("in setZoneStatus");
//	sprintf(buf, "setting %hhd %c%hhd to %hhd\n", zone_num, sensor_type,
//			sensor_num, status);
//	Serial.println(buf);
	if (zone_num < 0 || zone_num >= m_numzones)
		return;
//	printf("\tin setZoneStatus num=%hhd %c%hhd, %d\n", zone_num, sensor_type,
//			sensor_num, status);
	bool prev = false;
	struct Zone *z = &zones[zone_num];
	switch (sensor_type) {
	case 'c':
		prev = z->contact_alert[sensor_num - 1];
		z->contact_alert[sensor_num - 1] = status ? true : false;
		if (status) {
			printf("\tsetting zone %hhd contact %d to %d\n", zone_num,
					sensor_num, status);
			printf("\t\tc1 %d %d \n", z->contact_alert[sensor_num - 1],
					zones[zone_num].contact_alert[sensor_num - 1]);
		}
		break;
		break;
	case 'p':
		prev = z->pir_alert;
		z->pir_alert = status ? true : false;
		if (status) {
			printf("\tsetting zone %hhd pir to %d\n", zone_num, status);
			printf("\t\tz %d %d \n", z->pir_alert, zones[zone_num].pir_alert);
		}
		break;
	}

	bool changed = (prev != status);
	m_changed |= changed;
}

bool ZoneInfo::is_dirty() {
	return m_changed;
}

void ZoneInfo::markDirty(bool b) {
	m_changed = b;
}
