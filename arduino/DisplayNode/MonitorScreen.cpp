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

void indicateContact(TFT tft, int16_t x, int16_t y, int16_t w, int16_t h)
{
	tft.fillRect(x, y, w, h, COLOUR_CONTACT);
	tft.drawRect(x, y, w, h, COLOUR_OUTLINE);
}

void indicateBrokenWindow(TFT tft, int16_t x0, int16_t y0, int16_t x1,
		int16_t y1, int16_t x2, int16_t y2)
{
	tft.fillTriangle(x0, y0, x1, y1, x2, y2, COLOUR_WINDOW_BROKEN);
	tft.drawTriangle(x0, y0, x1, y1, x2, y2, COLOUR_OUTLINE);
}

void MonitorScreen::drawZone(int i, Zone zone, uint16_t color)
{
	if (zone.nodisplay)
	{
		return;
	}

	// temp
	uint16_t fillcolor = COLOUR_ACTIVE;

	if (millis() - INACTIVE_TIMEOUT_MS > zone.lastUpdatedMs)
		fillcolor = COLOUR_INACTIVE;

	if (zone.pir_alert)
	{
		fillcolor = COLOUR_PIR;
	}

	m_tft->fillRect(zone.x + 1, zone.y + 1, zone.w - 2, zone.h - 2, fillcolor);
	m_tft->drawRect(zone.x, zone.y, zone.w, zone.h, color);

	// overlay contact alert on appropriate wall
	if (zone.contact_alert)
	{
		bool oneOrMoreWallsFound = false;
		const uint8_t alertWallThickness = 8;
		int x, y, w, h;

		// highlight contact wall(s)
		if (zone.contact_walls & WALL_TOP)
		{
			oneOrMoreWallsFound = true;
			x = zone.x;
			y = zone.y;
			w = zone.w;
			h = alertWallThickness;
		}
		if (zone.contact_walls & WALL_BOTTOM)
		{
			oneOrMoreWallsFound = true;
			x = zone.x;
			y = zone.y + zone.h - alertWallThickness;
			w = zone.w;
			h = alertWallThickness;
		}
		if (zone.contact_walls & WALL_LEFT)
		{
			oneOrMoreWallsFound = true;
			x = zone.x;
			y = zone.y;
			w = alertWallThickness;
			h = zone.h;
		}
		if (zone.contact_walls & WALL_RIGHT)
		{
			oneOrMoreWallsFound = true;
			x = zone.x + zone.w - alertWallThickness;
			y = zone.y;
			w = alertWallThickness;
			h = zone.h;
		}
		if (oneOrMoreWallsFound)
			indicateContact(*m_tft, x, y, w, h);
	}

	// window broken: draw an outward point triangle centred on the wall.
	if (zone.window_broken)
	{
		int x0, y0, x1, y1, x2, y2;
		bool oneOrMoreWallsFound = false;

		// highlight contact wall(s)
		if ((zone.contact_walls & WALL_TOP)
				|| (zone.contact_walls & WALL_BOTTOM))
		{
			oneOrMoreWallsFound = true;
			x0 = zone.x;
			x1 = x0 + zone.w;
			x2 = zone.x + (zone.w / 2);
			if (zone.contact_walls & WALL_TOP)
			{
				y0 = y1 = zone.y + (zone.h / 2);
				y2 = zone.y;
			}
			if (zone.contact_walls & WALL_BOTTOM)
			{
				y0 = y1 = zone.y + (zone.h / 2);
				y2 = zone.y + zone.h - 1;
			}
		}
		if ((zone.contact_walls & WALL_LEFT)
				|| (zone.contact_walls & WALL_RIGHT))
		{
			oneOrMoreWallsFound = true;
			y0 = zone.y;
			y1 = y0 + zone.h - 1;
			y2 = zone.y + (zone.h / 2);

			if (zone.contact_walls & WALL_LEFT)
			{
				x0 = x1 = zone.x + (zone.w / 2);
				x2 = zone.x;
			}
			if (zone.contact_walls & WALL_RIGHT)
			{
				x0 = x1 = zone.x + (zone.w / 2);
				x2 = zone.x + zone.w - 1;
			}
		}
		if (oneOrMoreWallsFound)
			indicateBrokenWindow(*m_tft, x0, y0, x1, y1, x2, y2);

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
	m_tft->fillCircle(118, 105, 5, ST7735_BLUE);
	m_tft->fillCircle(118, 125, 5, ST7735_BLUE);
	m_tft->fillCircle(118, 145, 5, ST7735_BLUE);

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

