#define ESWAP(i) (((i & 0xFF) << 24) | ((i & 0xFF00) << 8) | ((i & 0xFF0000) >> 8) | ((i & 0xFF000000) >> 24))
typedef unsigned int uint;

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
const int spf = SPF;

uint MFRAME_SIZE(uint hdr);