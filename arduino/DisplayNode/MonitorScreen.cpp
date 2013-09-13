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
		drawRoom(i, z, COLOUR_OUTLINE);
	}
}
void MonitorScreen::drawRoom(int i, Zone zone, uint16_t color) {
	if (zone.nodisplay)
		return;

	// temp
	uint16_t fillcolor;

	switch (random(50)) {
	case 1:
		fillcolor = COLOUR_WARNING;
		break;
	case 2:
		fillcolor = COLOUR_ALERT;
		break;
	default:
		fillcolor = COLOUR_ACTIVE;
	}

	m_tft->fillRect(zone.x + 1, zone.y + 1, zone.w - 2, zone.h - 2, fillcolor);
	m_tft->drawRect(zone.x, zone.y, zone.w, zone.h, color);

	m_tft->setCursor(zone.x + zone.w / 2, zone.y + zone.h / 2);
	m_tft->setTextColor(ST7735_YELLOW, ST7735_BLACK);
	m_tft->print(i + 1, 10);
}

void MonitorScreen::drawFluff() {
	m_tft->fillRoundRect(113, 5, 10, 50, 3, ST7735_MAGENTA);
	m_tft->fillRoundRect(113, 105, 10, 50, 3, ST7735_BLUE);

}

void MonitorScreen::refresh(ZoneInfo zoneInfo, struct Status status) {

	drawRooms(zoneInfo);

	drawFluff();
}
