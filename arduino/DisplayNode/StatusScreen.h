/*
 * MonitorScreen.h
 *
 *  Created on: 10 Sep 2013
 *      Author: colin
 */

#ifndef __STATUSSCREEN_H__
#define __STATUSSCREEN_H__

#include "Screen.h"

#define COLOUR_TEXT   0x0000
#define COLOUR_BG   0xFFFF

class StatusScreen: public Screen {
public:
	StatusScreen(TFT *tft);
	virtual ~StatusScreen();

	void refresh(ZoneInfo zoneInfo, struct Status status);
private:
};

#endif /* __STATUSSCREEN_H__ */