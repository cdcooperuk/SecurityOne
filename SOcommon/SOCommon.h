#ifndef SOCOMMON_H_
#define SOCOMMON_H_


const uint64_t base_addr =  0xCDC12CDC00LL;

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t display_addr = base_addr + 87LL;

// First pipe is for display, rest are for sensors
const uint64_t pipes[6] = { display_addr, base_addr + 1LL, base_addr +2LL,
		base_addr +3LL, base_addr +4LL, base_addr +5LL };

//const uint64_t hub_addr = 0xF0F0F0F0D2LL;

unsigned char RF24_CHANNEL = 0x64;

#endif
