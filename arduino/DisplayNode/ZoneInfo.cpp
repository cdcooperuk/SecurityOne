/*
 * ZoneInfo.cpp
 *
 *  Created on: 10 Sep 2013
 *      Author: colin
 */

#include "ZoneInfo.h"

ZoneInfo::ZoneInfo() {
	// TODO Auto-generated constructor stub

}

ZoneInfo::~ZoneInfo() {
	// TODO Auto-generated destructor stub
}

uint8_t ZoneInfo::getNumZones() {
	return m_numzones;
}

void ZoneInfo::setNumZones(uint8_t n) {
	m_numzones=n;
}
