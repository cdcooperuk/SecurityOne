

#ifndef __DEBUG_H_
#define __DEBUG_H_

#undef DO_DEBUG
#ifdef DO_DEBUG
#define debug(s) Serial.println({s;})
#else
#define debug(x)
#endif


#endif /* __DEBUG_H_*/
