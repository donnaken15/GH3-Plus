#define ESWAP(i) (((i & 0xFF) << 24) | ((i & 0xFF00) << 8) | ((i & 0xFF0000) >> 8) | ((i & 0xFF000000) >> 24))
typedef unsigned int uint;

#define ACCURATETIME 1

#if ACCURATETIME
enum bitrate_index {
	M_free = 0b0000,
	M_32b = 0b0001,
	M_40b = 0b0010,
	M_48b = 0b0011,
	M_56b = 0b0100,
	M_64b = 0b0101,
	M_80b = 0b0110,
	M_96b = 0b0111,
	M_112b = 0b1000,
	M_128b = 0b1001,
	M_160b = 0b1010,
	M_192b = 0b1011,
	M_224b = 0b1100,
	M_256b = 0b1101,
	M_320b = 0b1110,
	M_bad = 0b1111
};
enum samprate_index {
	M_r44100 = 0b00,
	M_r48000 = 0b01,
	M_r32000 = 0b10,
	M_rreserved = 0b11
};
unsigned short bitrates(unsigned char index);
extern unsigned short samprates[];

typedef uint MHDR;
typedef struct {
	uint hdr;
	unsigned char* data;
} MFRAME_TAG, * MFRAME;

// samples per frame
#define SPF 1152
static const int spf = SPF;

uint MFRAME_SIZE(uint hdr);

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
#endif

