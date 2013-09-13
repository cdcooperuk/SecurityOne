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
#define COLOUR_WARNING   0xFFE0
#define COLOUR_ALERT   0xF800

class MonitorScreen: public Screen {
public:
	MonitorScreen(TFT *tft);
	virtual ~MonitorScreen();

	void refresh(ZoneInfo zoneInfo, struct Status status);
private:
	void drawRooms(ZoneInfo zoneInfo);
	void drawRoom(int i,Zone zone, uint16_t color);
	void drawFluff();
};

#endif /* __MONITORSCREEN_H__ */