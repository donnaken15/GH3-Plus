#pragma once

// somehow compilation fails on FastGH3 now >:/

#include "core\Patcher.h"
#include "gh3\QbArray.h"

#define OPEN_NOTEFX 1
#define OVERLAPPING_STARPOWER 1

// dont use, broken
// also i wont give out the required zones lol
// actually figured out animated textures so, no need for this hack
// and https://donnaken15.com/FastGH3/zones.html
//#define RAINBOW 1

///Determines which tapping gem sprite to use under starpower since we don't have a starpower texture (we do now >:)
//Set this to 0 to use the green note texture
//            1 to use the red note texture
//            2 to use the yellow note texture
//            3 to use the blue note texture
//            4 to use the orange note texture
//            5 is WHATEVER I WANT BOYYYYYYYY
#define SP_TAP_NOTE_GEM_INDEX 5

#define SYSTEX(t) t "_" t

#define PUSH_ALL_BUT_EDX \
__asm push esp \
__asm push ebp \
__asm push eax \
__asm push ebx \
__asm push ecx \
__asm push esi \
__asm push edi \

#define POP_ALL_BUT_EDX \
__asm pop edi \
__asm pop esi \
__asm pop ecx \
__asm pop ebx \
__asm pop eax \
__asm pop ebp \
__asm pop esp \

enum HopoFlag : uint32_t
{
	STRUMMED = 0,
	HOPO = 1,
	TAPPING = 2
};

struct Note
{
	uint32_t noteTime;
	uint32_t fretLength[5];
	HopoFlag hopoFlag;
	uint32_t nextNoteTime;
	uint32_t noteLength;
};


enum FretMask : uint32_t
{
	ORANGE = 0x00001,
	BLUE = 0x00010,
	YELLOW = 0x00100,
	RED = 0x01000,
	GREEN = 0x10000,
	OPEN = 0x22222,
};

enum QbFretMask : uint32_t
{
	QbGreen = 0x1,
	QbRed = 0x2,
	QbYellow = 0x4,
	QbBlue = 0x8,
	QbOrange = 0x10,
	QbHopoflip = 0x20,
	QbTapping = 0x40,
	QbOpen = 0x80
};

bool __fastcall IsSingleNote(FretMask fretMask);
FretMask __fastcall GetFretmaskFromTrackArray(GH3::QbArray *trackArray, int currentNote);
FretMask __fastcall GetFretmaskFromNoteQbArray(GH3::QbArray *noteArr);

constexpr GH3::QbKey RawOpenTextureKey =				"images\\highway\\gem2d_open.img";
constexpr GH3::QbKey RawOpenHammerTextureKey =			"images\\highway\\gem2d_open_hammer.img";
constexpr GH3::QbKey RawOpenTapTextureKey =					"images\\highway\\gem2d_open_tap.img";
constexpr GH3::QbKey RawOpenAnimTextureKey =				"images\\highway\\gem2d_open_anim.img";
constexpr GH3::QbKey RawOpenAnimHammerTextureKey =			"images\\highway\\gem2d_open_hammer_anim.img";
constexpr GH3::QbKey RawOpenStarTextureKey =			"images\\highway\\star2d_open.img";
constexpr GH3::QbKey RawOpenStarHammerTextureKey =		"images\\highway\\star2d_open_hammer.img";
constexpr GH3::QbKey RawOpenStarTapTextureKey =				"images\\highway\\star2d_open_tap.img";
constexpr GH3::QbKey RawOpenStarpowerTextureKey =		"images\\highway\\gem2d_open_starpower.img";
constexpr GH3::QbKey RawOpenStarpowerHammerTextureKey = "images\\highway\\gem2d_open_hammer_starpower.img";
constexpr GH3::QbKey RawOpenStarpowerTapTextureKey =		"images\\highway\\gem2d_open_tap_starpower.img";
constexpr GH3::QbKey RawOpenWhammyTextureKey =			"images\\highway\\whammy2d_open.img";
constexpr GH3::QbKey RawOpenWhammyStarTextureKey =		"images\\highway\\whammy2d_open_star.img";
constexpr GH3::QbKey RawOpenWhammyDeadTextureKey =		"images\\highway\\whammy2d_open_dead.img";
constexpr GH3::QbKey RawOpenHitFx1TextureKey =			"images\\highway\\openfx1.img";
constexpr GH3::QbKey RawOpenHitFx2TextureKey =			"images\\highway\\openfx2.img";
constexpr GH3::QbKey RawOpenAnimStarpowerTextureKey =		"images\\highway\\gem2d_open_starpower_anim.img";
constexpr GH3::QbKey RawOpenAnimStarpowerHammerTextureKey = "images\\highway\\gem2d_open_hammer_starpower_anim.img";

#ifdef OVERLAPPING_STARPOWER
constexpr GH3::QbKey RawStarStarTextureKey =			"images\\highway\\star2d_star.img";
constexpr GH3::QbKey RawStarHammerStarTextureKey =		"images\\highway\\star2d_star_hammer.img";
constexpr GH3::QbKey RawStarTapStarTextureKey =			"images\\highway\\star2d_star_tap.img";
constexpr GH3::QbKey RawOpenStarStarTextureKey =		"images\\highway\\star2d_open_star.img";
constexpr GH3::QbKey RawOpenStarHammerStarTextureKey =	"images\\highway\\star2d_open_hammer_star.img";
#endif

constexpr GH3::QbKey OpenTextureKey =					SYSTEX("sys_gem2d_open");
constexpr GH3::QbKey OpenHammerTextureKey =				SYSTEX("sys_gem2d_open_hammer");
constexpr GH3::QbKey OpenStarTextureKey =				SYSTEX("sys_star2d_open");
constexpr GH3::QbKey OpenStarHammerTextureKey =			SYSTEX("sys_star2d_open_hammer");
constexpr GH3::QbKey OpenStarpowerTextureKey =			SYSTEX("sys_gem2d_open_starpower");
constexpr GH3::QbKey OpenStarpowerHammerTextureKey =	SYSTEX("sys_gem2d_open_starpower_hammer");
constexpr GH3::QbKey OpenWhammyTextureKey =				SYSTEX("sys_whammy2d_open");
constexpr GH3::QbKey OpenWhammyStarTextureKey =			SYSTEX("sys_whammy2d_open_star");
constexpr GH3::QbKey OpenWhammyDeadTextureKey =			SYSTEX("sys_whammy2d_open_dead");
constexpr GH3::QbKey OpenHitFx1TextureKey =				SYSTEX("sys_openfx1");
constexpr GH3::QbKey OpenHitFx2TextureKey =				SYSTEX("sys_openfx2");

#ifdef OVERLAPPING_STARPOWER
constexpr GH3::QbKey StarStarTextureKey =				SYSTEX("sys_star2d_star");
constexpr GH3::QbKey StarHammerStarTextureKey =			SYSTEX("sys_star2d_hammer");
constexpr GH3::QbKey StarTapStarTextureKey =			SYSTEX("sys_star2d_tap");
constexpr GH3::QbKey OpenStarStarTextureKey =			SYSTEX("sys_star2d_open_star_star");
constexpr GH3::QbKey OpenStarHammerStarTextureKey =		SYSTEX("sys_star2d_open_star_hammer");
#endif

constexpr GH3::QbKey OpenTapTextureKey =				SYSTEX("sys_gem2d_open_tap");
constexpr GH3::QbKey OpenAnimTextureKey =				SYSTEX("sys_gem2d_open_anim");
constexpr GH3::QbKey OpenAnimHammerTextureKey =			SYSTEX("sys_gem2d_open_hammer_anim");

constexpr GH3::QbKey OpenStarTapTextureKey =			SYSTEX("sys_star2d_open_tap");

constexpr GH3::QbKey OpenStarpowerTapTextureKey =		SYSTEX("sys_gem2d_open_starpower_tap");
constexpr GH3::QbKey OpenAnimStarpowerTextureKey =		SYSTEX("sys_gem2d_open_starpower_anim");
constexpr GH3::QbKey OpenAnimStarpowerHammerTextureKey = SYSTEX("sys_gem2d_open_starpower_hammer_anim");

constexpr GH3::QbKey YellowTextureKey =					SYSTEX("sys_gem2d_yellow");
constexpr GH3::QbKey YellowHammerTextureKey =			SYSTEX("sys_gem2d_yellow_hammer");
constexpr GH3::QbKey YellowStarTextureKey =				SYSTEX("sys_star2d_yellow");
constexpr GH3::QbKey YellowStarHammerTextureKey =		SYSTEX("sys_star2d_yellow_hammer");
constexpr GH3::QbKey YellowStarpowerTextureKey =		SYSTEX("sys_gem2d_starpower");
constexpr GH3::QbKey YellowStarpowerHammerTextureKey =	SYSTEX("sys_gem2d_starpower_hammer");
constexpr GH3::QbKey YellowWhammyTextureKey =			SYSTEX("sys_whammy2d_yellow");
constexpr GH3::QbKey YellowWhammyStarTextureKey =		SYSTEX("sys_whammy2d_yellow_star");
constexpr GH3::QbKey YellowWhammyDeadTextureKey =		SYSTEX("sys_whammy2d_yellow_dead");

constexpr GH3::QbKey RawGreenAnimTextureKey =			"images\\highway\\gem2d_green_anim.img";
constexpr GH3::QbKey RawGreenHammerAnimTextureKey =		"images\\highway\\gem2d_green_hammer_anim.img";
constexpr GH3::QbKey RawRedAnimTextureKey =				"images\\highway\\gem2d_red_anim.img";
constexpr GH3::QbKey RawRedHammerAnimTextureKey =		"images\\highway\\gem2d_red_hammer_anim.img";
constexpr GH3::QbKey RawYellowAnimTextureKey =			"images\\highway\\gem2d_yellow_anim.img";
constexpr GH3::QbKey RawYellowHammerAnimTextureKey =	"images\\highway\\gem2d_yellow_hammer_anim.img";
constexpr GH3::QbKey RawBlueAnimTextureKey =			"images\\highway\\gem2d_blue_anim.img";
constexpr GH3::QbKey RawBlueHammerAnimTextureKey =		"images\\highway\\gem2d_blue_hammer_anim.img";
constexpr GH3::QbKey RawOrangeAnimTextureKey =			"images\\highway\\gem2d_orange_anim.img";
constexpr GH3::QbKey RawOrangeHammerAnimTextureKey =	"images\\highway\\gem2d_orange_hammer_anim.img";
constexpr GH3::QbKey RawStarpowerAnimTextureKey =		"images\\highway\\gem2d_starpower_anim.img";
constexpr GH3::QbKey RawStarpowerHammerAnimTextureKey = "images\\highway\\gem2d_starpower_hammer_anim.img";

constexpr GH3::QbKey GreenTextureKey =					SYSTEX("sys_gem2d_green");
constexpr GH3::QbKey RedTextureKey =					SYSTEX("sys_gem2d_red");
//constexpr GH3::QbKey YellowTextureKey =					SYSTEX("sys_gem2d_yellow");
constexpr GH3::QbKey BlueTextureKey =					SYSTEX("sys_gem2d_yellow");
constexpr GH3::QbKey OrangeTextureKey =					SYSTEX("sys_gem2d_green");
constexpr GH3::QbKey StarpowerTextureKey =				SYSTEX("sys_gem2d_starpower");

constexpr GH3::QbKey GreenHammerTextureKey =			SYSTEX("sys_gem2d_green_hammer");
constexpr GH3::QbKey RedHammerTextureKey =				SYSTEX("sys_gem2d_red_hammer");
////constexpr GH3::QbKey YellowHammerTextureKey =			SYSTEX("sys_gem2d_yellow_hammer");
constexpr GH3::QbKey BlueHammerTextureKey =				SYSTEX("sys_gem2d_blue_hammer");
constexpr GH3::QbKey OrangeHammerTextureKey =			SYSTEX("sys_gem2d_orange_hammer");
constexpr GH3::QbKey StarpowerHammerTextureKey =		SYSTEX("sys_gem2d_starpower_hammer");

constexpr GH3::QbKey GreenAnimTextureKey =				SYSTEX("sys_Gem2D_green_anim");
constexpr GH3::QbKey GreenHammerAnimTextureKey =		SYSTEX("sys_Gem2D_green_hammer_anim");
constexpr GH3::QbKey RedAnimTextureKey =				SYSTEX("sys_Gem2D_red_anim");
constexpr GH3::QbKey RedHammerAnimTextureKey =			SYSTEX("sys_Gem2D_red_hammer_anim");
constexpr GH3::QbKey YellowAnimTextureKey =				SYSTEX("sys_Gem2D_yellow_anim");
constexpr GH3::QbKey YellowHammerAnimTextureKey =		SYSTEX("sys_Gem2D_yellow_hammer_anim");
constexpr GH3::QbKey BlueAnimTextureKey =				SYSTEX("sys_Gem2D_blue_anim");
constexpr GH3::QbKey BlueHammerAnimTextureKey =			SYSTEX("sys_Gem2D_blue_hammer_anim");
constexpr GH3::QbKey OrangeAnimTextureKey =				SYSTEX("sys_Gem2D_orange_anim");
constexpr GH3::QbKey OrangeHammerAnimTextureKey =		SYSTEX("sys_Gem2D_orange_hammer_anim");
constexpr GH3::QbKey StarpowerAnimTextureKey =			SYSTEX("sys_Gem2D_starpower_anim");
constexpr GH3::QbKey StarpowerHammerAnimTextureKey =	SYSTEX("sys_Gem2D_starpower_hammer_anim");

// (old comment)what's the naming convention of the original global.tex note/fret textures
// figured it out, check FastGH3/SOURCE/ZONES/highway/__config.ini
constexpr GH3::QbKey RawGreenTapTextureKey =			"images\\highway\\tap2d_green.img";
constexpr GH3::QbKey RawRedTapTextureKey =				"images\\highway\\tap2d_red.img";
constexpr GH3::QbKey RawYellowTapTextureKey =			"images\\highway\\tap2d_yellow.img";
constexpr GH3::QbKey RawBlueTapTextureKey =				"images\\highway\\tap2d_blue.img";
constexpr GH3::QbKey RawOrangeTapTextureKey =			"images\\highway\\tap2d_orange.img";
constexpr GH3::QbKey RawGreenTapStarTextureKey =		"images\\highway\\tap2d_green_star.img";
constexpr GH3::QbKey RawRedTapStarTextureKey =			"images\\highway\\tap2d_red_star.img";
constexpr GH3::QbKey RawYellowTapStarTextureKey =		"images\\highway\\tap2d_yellow_star.img";
constexpr GH3::QbKey RawBlueTapStarTextureKey =			"images\\highway\\tap2d_blue_star.img";
constexpr GH3::QbKey RawOrangeTapStarTextureKey =		"images\\highway\\tap2d_orange_star.img";

constexpr GH3::QbKey GreenTapTextureKey =				SYSTEX("sys_tap2d_green");
constexpr GH3::QbKey RedTapTextureKey =					SYSTEX("sys_tap2d_red");
constexpr GH3::QbKey YellowTapTextureKey =				SYSTEX("sys_tap2d_yellow");
constexpr GH3::QbKey BlueTapTextureKey =				SYSTEX("sys_tap2d_blue");
constexpr GH3::QbKey OrangeTapTextureKey =				SYSTEX("sys_tap2d_orange");
constexpr GH3::QbKey GreenTapStarTextureKey =			SYSTEX("sys_tap2d_green_star");
constexpr GH3::QbKey RedTapStarTextureKey =				SYSTEX("sys_tap2d_red_star");
constexpr GH3::QbKey YellowTapStarTextureKey =			SYSTEX("sys_tap2d_yellow_star");
constexpr GH3::QbKey BlueTapStarTextureKey =			SYSTEX("sys_tap2d_blue_star");
constexpr GH3::QbKey OrangeTapStarTextureKey =			SYSTEX("sys_tap2d_orange_star");

constexpr GH3::QbKey RawStarpowerTapTextureKey =		"images\\highway\\tap2d_starpower.img";
constexpr GH3::QbKey StarpowerTapTextureKey =			SYSTEX("sys_tap2d_starpower");

#ifdef RAINBOW
constexpr GH3::QbKey RawRainbow1TextureKey =			0x00005001;
constexpr GH3::QbKey RawRainbow2TextureKey =			0x00005002;
constexpr GH3::QbKey RawRainbow3TextureKey =			0x00005003;
constexpr GH3::QbKey RawRainbow4TextureKey =			0x00005004;
constexpr GH3::QbKey RawRainbow5TextureKey =			0x00005005;
constexpr GH3::QbKey RawRainbow1HammerTextureKey =		0x00005006;
constexpr GH3::QbKey RawRainbow2HammerTextureKey =		0x00005007;
constexpr GH3::QbKey RawRainbow3HammerTextureKey =		0x00005008;
constexpr GH3::QbKey RawRainbow4HammerTextureKey =		0x00005009;
constexpr GH3::QbKey RawRainbow5HammerTextureKey =		0x0000500A;
constexpr GH3::QbKey RawRainbow1StarTextureKey =		0x0000500B;
constexpr GH3::QbKey RawRainbow2StarTextureKey =		0x0000500C;
constexpr GH3::QbKey RawRainbow3StarTextureKey =		0x0000500D;
constexpr GH3::QbKey RawRainbow4StarTextureKey =		0x0000500E;
constexpr GH3::QbKey RawRainbow5StarTextureKey =		0x0000500F;
constexpr GH3::QbKey RawRainbow1StarHammerTextureKey =	0x00005010;
constexpr GH3::QbKey RawRainbow2StarHammerTextureKey =	0x00005011;
constexpr GH3::QbKey RawRainbow3StarHammerTextureKey =	0x00005012;
constexpr GH3::QbKey RawRainbow4StarHammerTextureKey =	0x00005013;
constexpr GH3::QbKey RawRainbow5StarHammerTextureKey =	0x00005014; // not overriding original textures :(

constexpr GH3::QbKey Rainbow1TextureKey =				SYSTEX("sys_gem2d_rainbow1");
constexpr GH3::QbKey Rainbow2TextureKey =				SYSTEX("sys_gem2d_rainbow2");
constexpr GH3::QbKey Rainbow3TextureKey =				SYSTEX("sys_gem2d_rainbow3");
constexpr GH3::QbKey Rainbow4TextureKey =				SYSTEX("sys_gem2d_rainbow4");
constexpr GH3::QbKey Rainbow5TextureKey =				SYSTEX("sys_gem2d_rainbow5");
constexpr GH3::QbKey Rainbow1HammerTextureKey =			SYSTEX("sys_gem2d_rainbow1_hammer");
constexpr GH3::QbKey Rainbow2HammerTextureKey =			SYSTEX("sys_gem2d_rainbow2_hammer");
constexpr GH3::QbKey Rainbow3HammerTextureKey =			SYSTEX("sys_gem2d_rainbow3_hammer");
constexpr GH3::QbKey Rainbow4HammerTextureKey =			SYSTEX("sys_gem2d_rainbow4_hammer");
constexpr GH3::QbKey Rainbow5HammerTextureKey =			SYSTEX("sys_gem2d_rainbow5_hammer");
constexpr GH3::QbKey Rainbow1StarTextureKey =			SYSTEX("sys_star2d_rainbow1");
constexpr GH3::QbKey Rainbow2StarTextureKey =			SYSTEX("sys_star2d_rainbow2");
constexpr GH3::QbKey Rainbow3StarTextureKey =			SYSTEX("sys_star2d_rainbow3");
constexpr GH3::QbKey Rainbow4StarTextureKey =			SYSTEX("sys_star2d_rainbow4");
constexpr GH3::QbKey Rainbow5StarTextureKey =			SYSTEX("sys_star2d_rainbow5");
constexpr GH3::QbKey Rainbow1StarHammerTextureKey =		SYSTEX("sys_star2d_rainbow1_hammer");
constexpr GH3::QbKey Rainbow2StarHammerTextureKey =		SYSTEX("sys_star2d_rainbow2_hammer");
constexpr GH3::QbKey Rainbow3StarHammerTextureKey =		SYSTEX("sys_star2d_rainbow3_hammer");
constexpr GH3::QbKey Rainbow4StarHammerTextureKey =		SYSTEX("sys_star2d_rainbow4_hammer");
constexpr GH3::QbKey Rainbow5StarHammerTextureKey =		SYSTEX("sys_star2d_rainbow5_hammer");
#endif


/*constexpr GH3::QbKey RawGreenTextureKey =				0x151EE874;
constexpr GH3::QbKey RawRedTextureKey =					0x27dd8dc4;
constexpr GH3::QbKey RawYellowTextureKey =				0x39e35d20;
constexpr GH3::QbKey RawBlueTextureKey =				0xc715d48d;
constexpr GH3::QbKey RawOrangeTextureKey =				0xa5a7f110;
constexpr GH3::QbKey RawGreenHammerTextureKey =			0x59C6A636;
constexpr GH3::QbKey RawRedHammerTextureKey =			0xd51dd55e;
constexpr GH3::QbKey RawYellowHammerTextureKey =		0x9e1b6d3e;
constexpr GH3::QbKey RawBlueHammerTextureKey =			0xd326251a;
constexpr GH3::QbKey RawOrangeHammerTextureKey =		0x0243b96c;*/

extern GH3P::Patcher g_patcher;

extern float g_gemStartPosX[6];
extern float g_gemStartPosY[6];
extern float g_gemEndPosX[6];
extern float g_gemEndPosY[6];

extern float g_gemLeftStartPosX[6];
extern float g_gemLeftStartPosY[6];
extern float g_gemLeftEndPosX[6];
extern float g_gemLeftEndPosY[6];

extern float g_gemLeftAngle[6];
extern float g_gemAngle[6];

extern uint32_t g_gemMatBattle[6];
extern uint32_t g_gemMatHammerBattle[6];

extern uint32_t g_gemMatStar[6];
extern uint32_t g_gemMatHammerStar[6];

extern uint32_t g_gemMatNormal[6];
extern uint32_t g_gemMatHammer[6];

extern uint32_t g_gemMatTap[6];
extern uint32_t g_gemMatTapSp[6];

extern uint32_t g_gemMatSp[6];
extern uint32_t g_gemMatHammerSp[6];

extern uint32_t g_gemMatWhammy[6];
extern uint32_t g_gemMatSpWhammy[6];

extern uint32_t g_gemMatTapStar[6];


#ifdef OVERLAPPING_STARPOWER
extern uint32_t g_gemMatSpSp[6];
extern uint32_t g_gemMatHammerSpSp[6];
extern uint32_t g_gemMatTapSpSp[6];
#endif

extern uint32_t g_gemMatRainbow[6];
extern uint32_t g_gemMatRainbowHammer[6];

extern uint32_t g_gemMatRainbowStar[6];
extern uint32_t g_gemMatRainbowHammerStar[6];

extern bool animNotesEnabled;
extern bool openTapsEnabled;


void ApplyHack();

bool TryApplyNoteLogicPatches();
bool TryApplyNoteLoadingPatches();
bool TryApplyGemLoadingPatches();
bool TryApplyGemMutationPatches();
bool TryApplyGemConstantsPatches();
bool TryApplyGlobalArrayPatches();
bool TryApplyTextureTakeoverPatches();

FretMask getHighFret(FretMask fretMask);