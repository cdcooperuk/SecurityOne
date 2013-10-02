/*
 * MonitorScreen.h
 *
 *  Created on: 10 Sep 2013
 *      Author: colin
 */
#ifndef __MONITORSCREEN_H__
#define __MONITORSCREEN_H__

#include "Screen.h"
#include "ZoneInfo.h"

#define COLOUR_OUTLINE   0xBDF7
#define COLOUR_NYI   0xFFFF
#define COLOUR_ACTIVE   0x7E0
#define COLOUR_WARNING   0xFC00
#define COLOUR_ALERT   0xF800

class MonitorScreen: public Screen
{
public:
	MonitorScreen(TFT *tft);
	virtual ~MonitorScreen();
	const char* getName();

	void refresh( ZoneInfo *zoneInfo, const struct Status *status);
private:
	void drawZones(ZoneInfo zoneInfo);
	void drawZone(int i, Zone zone, uint16_t color);
	void drawFluff();
};

#endif /* __MONITORSCREEN_H__ */
