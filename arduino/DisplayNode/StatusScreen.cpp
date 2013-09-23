/*
 * MonitorScreen.cpp
 *
 *  Created on: 10 Sep 2013
 *      Author: colin
 */

#include "StatusScreen.h"
#include "ZoneInfo.h"
#include "debug.h"

StatusScreen::StatusScreen(TFT *tft) :
		Screen(tft) {
	// TODO Auto-generated constructor stub

}

StatusScreen::~StatusScreen() {
	// TODO Auto-generated destructor stub
}

void StatusScreen::refresh(ZoneInfo *zoneInfo, struct Status *status) {
	m_tft->setCursor(0, 0);
	m_tft->print("Status=");
	if (status->ok)
		m_tft->setTextColor(m_tft->Color565(0, 255, 0));
	else
		m_tft->setTextColor(m_tft->Color565(255, 0, 0));
	m_tft->println(status->error);
}
