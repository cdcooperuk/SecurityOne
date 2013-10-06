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

#include "Arduino.h"

ZoneInfo::ZoneInfo()
{
	m_numzones = 0;
	m_dirty = false;
	zones = NULL;
}

ZoneInfo::~ZoneInfo()
{
	// TODO Auto-generated destructor stub
}

uint8_t ZoneInfo::getNumZones()
{
	return m_numzones;
}

void ZoneInfo::setNumZones(uint8_t n)
{
	m_numzones = n;
	zones = (struct Zone*) malloc(sizeof(struct Zone) * n);
}
void ZoneInfo::initZone(int zoneNum, const char *name, uint8_t x, uint8_t y,
		uint8_t w, uint8_t h, uint8_t contact_walls, bool nodisplay)
{

	struct Zone *z = &zones[zoneNum];
	strcpy(z->name, name);
	z->x = x;
	z->y = y;
	z->w = w;
	z->h = h;
	z->contact_alert = false;
	z->window_broken = false;
	z->pir_alert = false;
	z->nodisplay = nodisplay;
	z->contact_walls = contact_walls;
	z->lastUpdatedMs=millis();
}

void ZoneInfo::setZoneStatus(uint8_t zone_num, char sensor_type, uint8_t status)
{

	if (zone_num < 0 || zone_num >= m_numzones)
		return;

	bool prev = false;
	struct Zone *z = &zones[zone_num];
	switch (sensor_type)
	{
	case 'C':
		prev = z->contact_alert;
		z->contact_alert = status ? true : false;
		break;
	case 'W':
		prev = z->window_broken;
		z->window_broken = status ? true : false;
		break;
	case 'P':
		prev = z->pir_alert;
		z->pir_alert = status ? true : false;
		break;
	}

	bool changed = (prev != status);
	z->dirty |= changed;
	m_dirty |= changed;
	z->lastUpdatedMs = millis();
}

bool ZoneInfo::is_dirty()
{
	return m_dirty;
}

void ZoneInfo::markDirty(bool b)
{
	m_dirty = b;
	for (int i = 0; i < m_numzones; i++)
	{
		zones[i].dirty = b;
	}
}
void ZoneInfo::markInactiveDirty()
{
	for (int i = 0; i < m_numzones; i++)
	{
		if (zones[i].lastUpdatedMs - millis() > INACTIVE_TIMEOUT_MS)
			zones[i].dirty = true;
		m_dirty = true;
	}
}
