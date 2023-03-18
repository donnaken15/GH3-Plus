#include "mp3seek.h"

#if ACCURATETIME
// VERSION 1, LAYER 3
// only format we're using (i'm pretty sure)
unsigned short bitrates(unsigned char index) {
	if (!index) return 0; // free (to parse for special data by program)
	if (index < 15)
		// lfg so based
		return (32 + ((--index & 3) << 3) << (index >> 2));
	return -1; // bad
}
unsigned short samprates[] = {
	44100,
	48000,
	32000,
	-1 // bad
};

uint MFRAME_SIZE(uint hdr)
{
	return 144 * (bitrates(MFRAME_GET_BTIDX(hdr)) * 1000) / samprates[MFRAME_GET_FRIDX(hdr)];
}
#endif

