/*
 * Screen.cpp
 *
 *  Created on: 10 Sep 2013
 *      Author: colin
 */

#include "Screen.h"

Screen::Screen( TFT *tft) :m_tft(tft) {

}

Screen::~Screen() {
}

void Screen::clear(){
	m_tft->background(ST7735_BLACK);
}
