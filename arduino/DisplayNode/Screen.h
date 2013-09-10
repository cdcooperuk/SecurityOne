/*
 * Screen.h
 *
 *  Created on: 10 Sep 2013
 *      Author: colin
 */

#ifndef SCREEN_H_
#define SCREEN_H_
#include <Adafruit_ST7735.h>
#include "ZoneInfo.h"

class Screen {
public:
	Screen( Adafruit_ST7735 *tft);
	virtual ~Screen();

	void clear();

	virtual void refresh(ZoneInfo zoneInfo) = 0;

protected:
	 Adafruit_ST7735 *m_tft;
};

#endif /* SCREEN_H_ */
