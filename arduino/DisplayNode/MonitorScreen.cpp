/*
 * MonitorScreen.cpp
 *
 *  Created on: 10 Sep 2013
 *      Author: colin
 */

#include "MonitorScreen.h"
#include "ZoneInfo.h"
#include "debug.h"

MonitorScreen::MonitorScreen(TFT *tft) :
		Screen(tft) {
	// TODO Auto-generated constructor stub

}

MonitorScreen::~MonitorScreen() {
	// TODO Auto-generated destructor stub
}

void MonitorScreen::drawRooms(ZoneInfo zoneInfo) {
	debug("drawRoom()...");
	for (int i = 0; i < zoneInfo.getNumZones(); i++) {
		Zone z = zoneInfo.zones[i];
		drawRoom(i, z, COLOUR_NYI);
	}
}
void MonitorScreen::drawRoom(int i, Zone zone, uint16_t color) {
	if (zone.fill) {
		m_tft->fillRect(zone.x, zone.y, zone.w, zone.h, COLOUR_NYI);
	} else {
		// temp
		switch (random(50)) {
		case 1:
			m_tft->fillRect(zone.x, zone.y, zone.w, zone.h, COLOUR_WARNING);
			break;
		case 2:
			m_tft->fillRect(zone.x, zone.y, zone.w, zone.h, COLOUR_ALERT);
			break;
		default:
			m_tft->fillRect(zone.x, zone.y, zone.w, zone.h, COLOUR_ACTIVE);
		}
	}
	m_tft->drawRect(zone.x, zone.y, zone.w, zone.h, color);

	m_tft->setCursor(zone.x + zone.w / 2, zone.y + zone.h / 2);
	m_tft->setTextColor(ST7735_YELLOW, ST7735_BLACK);
	m_tft->print(i+1, 10);
}
void MonitorScreen::refresh(ZoneInfo zoneInfo) {

	drawRooms(zoneInfo);
}

