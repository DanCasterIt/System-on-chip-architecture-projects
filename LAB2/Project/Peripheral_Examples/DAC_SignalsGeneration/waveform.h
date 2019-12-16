#include <stdint.h>

//#define SQUARE //comment for sinewave

#ifdef SQUARE
extern const uint16_t Square12bit[1024];
#else
extern const uint16_t Sine12bit[1024];
#endif
