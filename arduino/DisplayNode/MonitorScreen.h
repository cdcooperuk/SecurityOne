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

#define COLOUR_OUTLINE   0x7BEF
#define COLOUR_NYI   0xFFFF
#define COLOUR_ACTIVE   0x07E0
#define COLOUR_INACTIVE   0xBDF7
#define COLOUR_CONTACT    ST7735_YELLOW
#define COLOUR_WINDOW_BROKEN   ST7735_RED
#define COLOUR_PIR   ST7735_MAGENTA

class MonitorScreen: public Screen
{
public:
	MonitorScreen(TFT *tft);
	virtual ~MonitorScreen();
	const char* getName();

	void refresh(ZoneInfo *zoneInfo, const struct Status *status);
private:
	void drawZones(ZoneInfo zoneInfo);
	void drawZone(int i, Zone zone, uint16_t color);
	void drawFluff();
};

#endif /* __MONITORSCREEN_H__ */
