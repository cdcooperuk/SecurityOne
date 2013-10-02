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

}

MonitorScreen::~MonitorScreen() {
}

void MonitorScreen::drawZones(ZoneInfo zoneInfo) {
	for (int i = 0; i < zoneInfo.getNumZones(); i++) {
		struct Zone z = zoneInfo.zones[i];
		if (z.dirty) {
			TIMEIT(drawZone, drawZone(i, z, COLOUR_OUTLINE);)
		}
	}
}
void MonitorScreen::drawZone(int i, Zone zone, uint16_t color) {
	if (zone.nodisplay) {
		return;
	}

	// temp
	uint16_t fillcolor = COLOUR_ACTIVE;
	if (zone.pir_alert) {
		fillcolor = COLOUR_ALERT;
	}

	m_tft->fillRect(zone.x + 1, zone.y + 1, zone.w - 2, zone.h - 2, fillcolor);
	m_tft->drawRect(zone.x, zone.y, zone.w, zone.h, color);
	if (zone.contact_alert[0] || zone.contact_alert[1]
			|| zone.contact_alert[2]) {
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
	m_tft->print(zone.name);
	zone.dirty = false;
}

void MonitorScreen::drawFluff() {
	m_tft->fillRoundRect(113, 5, 10, 50, 3, ST7735_YELLOW);
	m_tft->fillTriangle(113, 75, 123, 80, 113, 85, ST7735_BLACK);
	m_tft->drawTriangle(113, 75, 123, 80, 113, 85, ST7735_WHITE);
	m_tft->fillRoundRect(113, 105, 10, 50, 3, ST7735_BLUE);

}

void MonitorScreen::refresh(ZoneInfo *zoneInfo, const struct Status *status) {
	if (zoneInfo->is_dirty()) {
		drawZones(*zoneInfo);

		drawFluff();
		zoneInfo->markDirty(false);
	}
}
const char* MonitorScreen::getName()
{
	return "monitor";
}

