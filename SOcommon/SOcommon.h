#ifndef SOCOMMON_H_
#define SOCOMMON_H_

#include "RF24.h"

const uint64_t CFG_BASE_ADDR =  0xCDC12CDC00LL;

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t display_addr = CFG_BASE_ADDR + 87LL;

// First pipe is for display, rest are for sensors
const uint64_t pipes[6] = { display_addr, CFG_BASE_ADDR + 1LL, CFG_BASE_ADDR +2LL,
		CFG_BASE_ADDR +3LL, CFG_BASE_ADDR +4LL, CFG_BASE_ADDR +5LL };

//const uint64_t hub_addr = 0xF0F0F0F0D2LL;

unsigned char CFG_RF24_CHANNEL = 0x64;

rf24_datarate_e CFG_RF24_DATA_RATE = RF24_1MBPS;
rf24_crclength_e CFG_RF24_CRC_LENGTH = RF24_CRC_16;

#endif
