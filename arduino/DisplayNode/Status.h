#ifndef __STATUS_H__
#define __STATUS_H__

struct Status
{
	volatile bool ok;
	char status_text[32];
	volatile int nMsgsReceived;
	volatile int nMsgsDiscarded;
};

#endif /* __STATUS_H__ */
