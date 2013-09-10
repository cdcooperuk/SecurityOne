/*
 * Screen.cpp
 *
 *  Created on: 10 Sep 2013
 *      Author: colin
 */

#include "Screen.h"

Screen::Screen( Adafruit_ST7735 *tft) :m_tft(tft) {

}

Screen::~Screen() {
	// TODO Auto-generated destructor stub
}

void Screen::clear(){
	m_tft->fillScreen(ST7735_CYAN);
}
