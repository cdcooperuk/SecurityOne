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
		Screen(tft)
{
	// TODO Auto-generated constructor stub

}

StatusScreen::~StatusScreen()
{
	// TODO Auto-generated destructor stub
}

void StatusScreen::printZones(ZoneInfo *zoneInfo)
{

	for (int i = 0; i < zoneInfo->getNumZones(); i++)
	{
		struct Zone z = zoneInfo->zones[i];
//		m_tft->setCursor(0, startY + (i * 8));

		if (z.dirty)
		{
			m_tft->setTextColor(ST7735_WHITE, ST7735_BLACK);
			m_tft->print(z.name);
			m_tft->print(" ");
			m_tft->print(z.contact_alert[0] ? "1" : " ");
			m_tft->print(z.contact_alert[1] ? "2" : " ");
			m_tft->print(z.contact_alert[2] ? "3" : " ");
			m_tft->print(z.pir_alert ? "P" : " ");
		}
		m_tft->println();
	}

}
void StatusScreen::refresh(ZoneInfo *zoneInfo, const struct Status *status)
{
	const char * header = "STATUS";
	short xoffsetToCenter = (m_tft->width() - 6 * (strlen(header))) / 2;
	m_tft->setCursor(xoffsetToCenter, 0);
	m_tft->setTextColor(ST7735_WHITE);
	m_tft->println("STATUS");
	m_tft->println();
	m_tft->print("Status=");
	if (status->ok)
		m_tft->setTextColor(m_tft->Color565(0, 255, 0));
	else
		m_tft->setTextColor(m_tft->Color565(255, 0, 0));
	m_tft->println(status->status_text);

	m_tft->setTextColor(ST7735_WHITE, ST7735_BLACK);
	char buf[32];
	sprintf(buf, "recvd %d bad %d", status->nMsgsReceived,
			status->nMsgsDiscarded);
	m_tft->println(buf);
	m_tft->println();

	if (zoneInfo->is_dirty())
	{
		printZones(zoneInfo);
		zoneInfo->markDirty(false);
	}
}

const char* StatusScreen::getName()
{
	return "status";
}

