/*
 * MonitorScreen.cpp
 *
 *  Created on: 10 Sep 2013
 *      Author: colin
 */

#include "MonitorScreen.h"
#include "ZoneInfo.h"
#undef DO_TIMING
#include "debug.h"

MonitorScreen::MonitorScreen(TFT *tft) :
		Screen(tft) {
	// TODO Auto-generated constructor stub

}

MonitorScreen::~MonitorScreen() {
	// TODO Auto-generated destructor stub
}

void MonitorScreen::drawRooms(ZoneInfo zoneInfo) {
	for (int i = 0; i < zoneInfo.getNumZones(); i++) {
		struct Zone z = zoneInfo.zones[i];

		TIMEIT(drawRoom, drawRoom(i, z, COLOUR_OUTLINE);)
	}
}
void MonitorScreen::drawRoom(int i, Zone zone, uint16_t color) {
	printf("in %d (p=%d) (c1=%d)", i, zone.pir_alert, zone.contact_alert[0]);
	if (zone.nodisplay) {
		printf("returning\n");
		return;
	}

	// temp
	uint16_t fillcolor = COLOUR_ACTIVE;
	if (zone.contact_alert[0] || zone.contact_alert[1]
			|| zone.contact_alert[2]) {
		fillcolor = COLOUR_WARNING;
		printf("\t*** contact alert %s\n", zone.name);
	}
	if (zone.pir_alert) {
		fillcolor = COLOUR_ALERT;
		printf("\t*** pir alert %s\n", zone.name);
	}

	m_tft->fillRect(zone.x + 1, zone.y + 1, zone.w - 2, zone.h - 2, fillcolor);
	m_tft->drawRect(zone.x, zone.y, zone.w, zone.h, color);

	m_tft->setCursor(zone.x + zone.w / 2 - 4, zone.y + zone.h / 2 - 4);
	m_tft->setTextColor(ST7735_YELLOW, ST7735_BLACK);
	m_tft->print(i, 10);
	printf("\n");
}

void MonitorScreen::drawFluff() {
	m_tft->fillRoundRect(113, 5, 10, 50, 3, ST7735_MAGENTA);
	m_tft->fillRoundRect(113, 105, 10, 50, 3, ST7735_BLUE);

}

void MonitorScreen::refresh(ZoneInfo *zoneInfo, struct Status *status) {
	if (zoneInfo->is_dirty()) {
		printf("screen is dirty\n");
		drawRooms(*zoneInfo);

		drawFluff();
		zoneInfo->markDirty(false);
	}
}
