
#ifndef __DEBUG_H_
#define __DEBUG_H_

#ifdef DO_DEBUG
#define debug(s) {Serial.println(s);}
#else
#define debug(x)
#endif

#ifdef SERIAL_DEBUG
#define IF_SERIAL_DEBUG(x) ({x;})
#else
#define IF_SERIAL_DEBUG(x)
#endif


// optionally print timing info.  Id is for variable uniqueness - a meaningful token name.
#ifdef DO_TIMING
#define TIMEIT(id,clause) \
	unsigned long t##id = millis(); \
	{clause ;}\
	printf(#id"() took %ld ms\n\r",millis() - t##id );
#else
#define TIMEIT(id,clause) clause
#endif



#endif /* __DEBUG_H_*/
