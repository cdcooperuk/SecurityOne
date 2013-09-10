/*
 * MonitorScreen.h
 *
 *  Created on: 10 Sep 2013
 *      Author: colin
 */

#ifndef MONITORSCREEN_H_
#define MONITORSCREEN_H_

#include "Screen.h"
#include "ZoneInfo.h"


#define COLOUR_NYI   0xBDF7
#define COLOUR_ACTIVE   0x7E0
#define COLOUR_WARNING   0xFFE0
#define COLOUR_ALERT   0xF800

class MonitorScreen: public Screen {
public:
	MonitorScreen(Adafruit_ST7735 *tft);
	virtual ~MonitorScreen();

	void refresh(ZoneInfo zoneInfo);
private:
	void drawRooms(ZoneInfo zoneInfo);
	void drawRoom(int i,Zone zone, uint16_t color);
};

#endif /* MONITORSCREEN_H_ */
