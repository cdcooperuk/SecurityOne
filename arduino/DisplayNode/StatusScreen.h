/*
 * MonitorScreen.h
 *
 *  Created on: 10 Sep 2013
 *      Author: colin
 */

#ifndef __STATUSSCREEN_H__
#define __STATUSSCREEN_H__

#include "Screen.h"

#define COLOUR_TEXT   0xFFFF
#define COLOUR_BG   0x0000

class StatusScreen: public Screen {
public:
	StatusScreen(TFT *tft);
	virtual ~StatusScreen();
	const char* getName();

	void refresh( ZoneInfo *zoneInfo, const struct Status *status);
private:
	 void printZones( ZoneInfo *zoneInfo);
};

#endif /* __STATUSSCREEN_H__ */
