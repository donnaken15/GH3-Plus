#include "mp3seek.h"

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
// 0xFFFB504C & 0xFFE00000 >> 21 & 0x7FF
#define MFRAME_GET_FSYNC(h) h >> 21
// 0xFFFB504C & 0x00180000 >> 19 & 0x3
#define MFRAME_GET_VERSN(h) h >> 19 & 3
// 0xfffb504c & 0x00060000 >> 17 & 0x3
#define MFRAME_GET_LAYER(h) h >> 17 & 3
// 0xFFFB504C & 0x00010000 >> 16 & 1
#define MFRAME_GET_PRTCT(h) h >> 16 & 1
// 0xFFFB504C & 0x0000F000 >> 12 & 0xF
#define MFRAME_GET_BTIDX(h) h >> 12 & 15
// 0xFFFB504C & 0x00000C00 >> 10 & 3
#define MFRAME_GET_FRIDX(h) h >> 10 & 3
// 0xFFFB504C & 0x00000200 >> 9 & 1
#define MFRAME_GET_PADBT(h) h >> 9 & 1
// 0xFFFB504C & 0x00000100 >> 8 & 1
#define MFRAME_GET_PRVBT(h) h >> 8 & 1
// 0xFFFB504C & 0x000000C0 >> 6 & 1
#define MFRAME_GET_CHNLM(h) h >> 6 & 3
#define MFRAME_GET_MDEXT(h) h >> 4 & 3
// 0xFFFB504C & 0x00000008 >> 3 & 1
#define MFRAME_GET_COPYR(h) h >> 3 & 1
// 0xFFFB504C & 0x00000004 >> 2 & 1
#define MFRAME_GET_ORGNL(h) h >> 2 & 1
#define MFRAME_GET_EMPHS(h) h & 3

uint MFRAME_SIZE(uint hdr)
{
	// MAKING 144 A FLOAT BLOATED THE PROGRAM SUPER HARD WTFFFFF
	// time to use linking with FASM (which i have already for another thing)
	return 144 * (bitrates(MFRAME_GET_BTIDX(hdr)) * 1000) / samprates[MFRAME_GET_FRIDX(hdr)];
}