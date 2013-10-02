/*
 * Screen.h
 *
 *  Created on: 10 Sep 2013
 *      Author: colin
 */

#ifndef __SCREEN_H__
#define __SCREEN_H__
#include <TFT.h>
#include "ZoneInfo.h"
#include "Status.h"

class Screen {
public:
	Screen( TFT *tft);
	virtual ~Screen();

	void clear();

	virtual void refresh( ZoneInfo *zoneInfo, const struct Status *status) = 0;
	virtual const char * getName() =0;

protected:
	 TFT *m_tft;
};

#endif /* __SCREEN_H__ */
