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

void MonitorScreen::drawZones(ZoneInfo zoneInfo) {
	for (int i = 0; i < zoneInfo.getNumZones(); i++) {
		struct Zone z = zoneInfo.zones[i];
		if (z.dirty) {
			IF_SERIAL_DEBUG(printf("\t%d dirty ", i))
			TIMEIT(drawZone, drawZone(i, z, COLOUR_OUTLINE);)
		}
	}
}
void MonitorScreen::drawZone(int i, Zone zone, uint16_t color) {
	//printf("in %d (p=%d) (c1=%d)", i, zone.pir_alert, zone.contact_alert[0]);
	if (zone.nodisplay) {
		IF_SERIAL_DEBUG(printf("nodisplay\n"))
		return;
	}

	// temp
	uint16_t fillcolor = COLOUR_ACTIVE;
	if (zone.pir_alert) {
		fillcolor = COLOUR_ALERT;
		printf("\t*** pir alert %s\n", zone.name);
	}

	m_tft->fillRect(zone.x + 1, zone.y + 1, zone.w - 2, zone.h - 2, fillcolor);
	m_tft->drawRect(zone.x, zone.y, zone.w, zone.h, color);
	if (zone.contact_alert[0] || zone.contact_alert[1]
			|| zone.contact_alert[2]) {
		printf("\t*** contact alert %s\n", zone.name);
		const uint8_t alertWallThickness = 5;
		// highlight contact wall(s)
		if (zone.contact_walls & WALL_TOP) {
			m_tft->fillRect(zone.x, zone.y, zone.w, alertWallThickness,
			COLOUR_WARNING);
		}
		if (zone.contact_walls & WALL_BOTTOM) {
			m_tft->fillRect(zone.x, zone.y + zone.h - alertWallThickness,
					zone.w, alertWallThickness, COLOUR_WARNING);
		}
		if (zone.contact_walls & WALL_LEFT) {
			m_tft->fillRect(zone.x, zone.y, alertWallThickness, zone.h,
			COLOUR_WARNING);
		}
		if (zone.contact_walls & WALL_RIGHT) {
			m_tft->fillRect(zone.x + zone.w - alertWallThickness, zone.y,
					alertWallThickness, zone.h, COLOUR_WARNING);
		}
	}

	m_tft->setCursor(zone.x + zone.w / 2 - 6, zone.y + zone.h / 2 - 4);
	m_tft->setTextColor(ST7735_BLACK);
	//m_tft->print(i, 10);
	m_tft->print(zone.name);
	zone.dirty = false;
}

void MonitorScreen::drawFluff() {
	m_tft->fillRoundRect(113, 5, 10, 50, 3, ST7735_YELLOW);
	m_tft->fillTriangle(113, 75, 123, 80, 113, 85, ST7735_BLACK);
	m_tft->drawTriangle(113, 75, 123, 80, 113, 85, ST7735_WHITE);
	m_tft->fillRoundRect(113, 105, 10, 50, 3, ST7735_BLUE);

}

void MonitorScreen::refresh(ZoneInfo *zoneInfo, struct Status *status) {
	if (zoneInfo->is_dirty()) {
		printf("screen is dirty\n");
		drawZones(*zoneInfo);

		drawFluff();
		zoneInfo->markDirty(false);
	}
}
