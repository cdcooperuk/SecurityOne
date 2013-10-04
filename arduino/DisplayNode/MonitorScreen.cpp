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
		Screen(tft)
{

}

MonitorScreen::~MonitorScreen()
{
}

void MonitorScreen::drawZones(ZoneInfo zoneInfo)
{
	for (int i = 0; i < zoneInfo.getNumZones(); i++)
	{
		struct Zone z = zoneInfo.zones[i];
		if (z.dirty)
		{
			TIMEIT(drawZone, drawZone(i, z, COLOUR_OUTLINE);)
		}
	}
}
void MonitorScreen::drawZone(int i, Zone zone, uint16_t color)
{
	if (zone.nodisplay)
	{
		return;
	}

	// temp
	uint16_t fillcolor = COLOUR_ACTIVE;

	if (zone.pir_alert)
	{
		fillcolor = COLOUR_PIR;
	}

	m_tft->fillRect(zone.x + 1, zone.y + 1, zone.w - 2, zone.h - 2, fillcolor);
	m_tft->drawRect(zone.x, zone.y, zone.w, zone.h, color);

	// overlay contact alert on appropriate wall
	if (zone.contact_alert)
	{
		const uint8_t alertWallThickness = 5;
		int x, y, w, h;

		// highlight contact wall(s)
		if (zone.contact_walls & WALL_TOP)
		{
			x = zone.x;
			y = zone.y;
			w = zone.w;
			h = alertWallThickness;
		}
		if (zone.contact_walls & WALL_BOTTOM)
		{
			x = zone.x;
			y = zone.y + zone.h - alertWallThickness;
			w = zone.w;
			h = alertWallThickness;
		}
		if (zone.contact_walls & WALL_LEFT)
		{
			x = zone.x;
			y = zone.y;
			w = alertWallThickness;
			h = zone.h;
		}
		if (zone.contact_walls & WALL_RIGHT)
		{
			x = zone.x + zone.w - alertWallThickness;
			y = zone.y;
			w = alertWallThickness;
			h = zone.h;
		}
		m_tft->fillRect(x, y, w, h, COLOUR_CONTACT);
	}

	// window broken: draw an inward point triangle centred on the wall.
	if (zone.window_broken)
	{
		int x1, y1, x2, y2, x3, y3;

		// highlight contact wall(s)
		if (zone.contact_walls & WALL_TOP)
		{
			x1 = zone.x;
			x2 = x1 + zone.w;
			x3 = zone.x + (zone.w / 2);

			y1 = y2 = zone.y;
			y3 = zone.y + (zone.h / 2);
		}
		if (zone.contact_walls & WALL_BOTTOM)
		{
			x1 = zone.x;
			x2 = x1 + zone.w;
			x3 = zone.x + (zone.w / 2);

			y1 = y2 = zone.y + zone.h - 1;
			y3 = zone.y + (zone.h / 2);
		}
		if (zone.contact_walls & WALL_LEFT)
		{
			x1 = x2 = zone.x;
			x3 = zone.x + (zone.w / 2);

			y1 = zone.y;
			y2 = y1 + zone.h - 1;
			y3 = zone.y + (zone.h / 2);
		}
		if (zone.contact_walls & WALL_RIGHT)
		{
			x1 = x2 = zone.x + zone.w - 1;
			x3 = zone.x + (zone.w / 2);

			y1 = zone.y + (zone.h / 2);
			y2 = y1 + zone.h - 1;
			y3 = zone.y + (zone.h / 2);
		}

		m_tft->fillTriangle(x1, y1, x2, y2, x3, y3, COLOUR_WINDOW_BROKEN);
		m_tft->drawTriangle(x1, y1, x2, y2, x3, y3, COLOUR_OUTLINE);
	}

	m_tft->setCursor(zone.x + zone.w / 2 - 6, zone.y + zone.h / 2 - 4);
	m_tft->setTextColor(ST7735_BLACK);
	m_tft->print(zone.name);
	zone.dirty = false;
}

void MonitorScreen::drawFluff()
{
	m_tft->fillRoundRect(113, 5, 10, 50, 3, ST7735_YELLOW);
	m_tft->fillTriangle(113, 75, 123, 80, 113, 85, ST7735_BLACK);
	m_tft->drawTriangle(113, 75, 123, 80, 113, 85, ST7735_WHITE);
	m_tft->fillRoundRect(113, 105, 10, 50, 3, ST7735_BLUE);

}

void MonitorScreen::refresh(ZoneInfo *zoneInfo, const struct Status *status)
{
	if (zoneInfo->is_dirty())
	{
		drawZones(*zoneInfo);

		drawFluff();
		zoneInfo->markDirty(false);
	}
}
const char* MonitorScreen::getName()
{
	return "monitor";
}

