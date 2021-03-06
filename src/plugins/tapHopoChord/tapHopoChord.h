#pragma once

#include "core\Patcher.h"
#include "gh3\QbArray.h"

//#define EXE HATRED
#define HATRED
//#define BATTERY
// EXE KEYWORDS:
// BATTERY - 37MB - DEFAULT
// HATRED - 6MB

///Determines which tapping gem sprite to use under starpower since we don't have a starpower texture
//Set this to 0 to use the green note texture
//            1 to use the red note texture
//            2 to use the yellow note texture
//            3 to use the blue note texture
//            4 to use the orange note texture
#define SP_TAP_NOTE_GEM_INDEX 3

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
constexpr GH3::QbKey RawOpenTapTextureKey =				"images\\highway\\gem2d_open_tap.img";
constexpr GH3::QbKey RawOpenAnimTextureKey =			"images\\highway\\gem2d_open_anim.img";
constexpr GH3::QbKey RawOpenAnimHammerTextureKey =		"images\\highway\\gem2d_open_hammer_anim.img";

constexpr GH3::QbKey RawOpenStarTextureKey =			"images\\highway\\star2d_open.img";
constexpr GH3::QbKey RawOpenStarHammerTextureKey =		"images\\highway\\star2d_open_hammer.img";
constexpr GH3::QbKey RawOpenStarTapTextureKey =			"images\\highway\\star2d_open_tap.img";
constexpr GH3::QbKey RawOpenStarpowerTextureKey =		"images\\highway\\gem2d_open_starpower.img";
constexpr GH3::QbKey RawOpenStarpowerHammerTextureKey = "images\\highway\\gem2d_open_hammer_starpower.img";
constexpr GH3::QbKey RawOpenStarpowerTapTextureKey =	"images\\highway\\gem2d_open_tap_starpower.img";
constexpr GH3::QbKey RawOpenAnimStarpowerTextureKey =		"images\\highway\\gem2d_open_starpower_anim.img";
constexpr GH3::QbKey RawOpenAnimStarpowerHammerTextureKey = "images\\highway\\gem2d_open_hammer_starpower_anim.img";

constexpr GH3::QbKey OpenTextureKey =					"sys_gem2d_open_sys_gem2d_open";
constexpr GH3::QbKey OpenHammerTextureKey =				"sys_gem2d_open_hammer_sys_gem2d_open_hammer";
constexpr GH3::QbKey OpenTapTextureKey =				"sys_gem2d_open_tap_sys_gem2d_open_tap";
constexpr GH3::QbKey OpenAnimTextureKey =				"sys_gem2d_open_anim_sys_gem2d_open_anim";
constexpr GH3::QbKey OpenAnimHammerTextureKey =			"sys_gem2d_open_hammer_anim_sys_gem2d_open_hammer_anim";

constexpr GH3::QbKey OpenStarTextureKey =				"sys_star2d_open_sys_gem2d_open";
constexpr GH3::QbKey OpenStarHammerTextureKey =			"sys_star2d_open_hammer_sys_gem2d_open_hammer";
constexpr GH3::QbKey OpenStarTapTextureKey =			"sys_star2d_open_tap_sys_star2d_open_tap";
constexpr GH3::QbKey OpenStarpowerTextureKey =			"sys_gem2d_open_starpower_sys_gem2d_open_starpower";
constexpr GH3::QbKey OpenStarpowerHammerTextureKey =	"sys_gem2d_open_starpower_hammer_sys_gem2d_open_starpower_hammer";
constexpr GH3::QbKey OpenStarpowerTapTextureKey =		"sys_gem2d_open_starpower_tap_sys_gem2d_open_starpower_tap";
constexpr GH3::QbKey OpenAnimStarpowerTextureKey =		"sys_gem2d_open_starpower_anim_sys_gem2d_open_starpower_anim";
constexpr GH3::QbKey OpenAnimStarpowerHammerTextureKey = "sys_gem2d_open_starpower_hammer_anim_sys_gem2d_open_starpower_hammer_anim";

constexpr GH3::QbKey YellowStarTextureKey =				"sys_star2d_yellow_sys_star2d_yellow";
constexpr GH3::QbKey YellowStarHammerTextureKey =		"sys_star2d_yellow_hammer_sys_star2d_yellow_hammer";
constexpr GH3::QbKey YellowStarpowerTextureKey =		"sys_gem2d_starpower_sys_gem2d_starpower";
constexpr GH3::QbKey YellowStarpowerHammerTextureKey =	"sys_gem2d_starpower_hammer_sys_gem2d_starpower_hammer";

constexpr GH3::QbKey GreenTextureKey = "sys_gem2d_green_sys_gem2d_green";
constexpr GH3::QbKey RedTextureKey = "sys_gem2d_red_sys_gem2d_red";
constexpr GH3::QbKey YellowTextureKey = "sys_gem2d_yellow_sys_gem2d_yellow";
constexpr GH3::QbKey BlueTextureKey = "sys_gem2d_yellow_sys_gem2d_yellow";
constexpr GH3::QbKey OrangeTextureKey = "sys_gem2d_green_sys_gem2d_green";
constexpr GH3::QbKey StarpowerTextureKey = "sys_gem2d_starpower_sys_gem2d_starpower";

constexpr GH3::QbKey GreenHammerTextureKey = "sys_gem2d_green_hammer_sys_gem2d_green_hammer";
constexpr GH3::QbKey RedHammerTextureKey = "sys_gem2d_red_hammer_sys_gem2d_red_hammer";
constexpr GH3::QbKey YellowHammerTextureKey = "sys_gem2d_yellow_hammer_sys_gem2d_yellow_hammer";
constexpr GH3::QbKey BlueHammerTextureKey = "sys_gem2d_blue_hammer_sys_gem2d_blue_hammer";
constexpr GH3::QbKey OrangeHammerTextureKey = "sys_gem2d_orange_hammer_sys_gem2d_orange_hammer";
constexpr GH3::QbKey StarpowerHammerTextureKey = "sys_gem2d_starpower_hammer_sys_gem2d_starpower_hammer";

constexpr GH3::QbKey RawGreenAnimTextureKey = "images\\highway\\gem2d_green_anim.img";
constexpr GH3::QbKey RawGreenHammerAnimTextureKey = "images\\highway\\gem2d_green_hammer_anim.img";
constexpr GH3::QbKey RawRedAnimTextureKey = "images\\highway\\gem2d_red_anim.img";
constexpr GH3::QbKey RawRedHammerAnimTextureKey = "images\\highway\\gem2d_red_hammer_anim.img";
constexpr GH3::QbKey RawYellowAnimTextureKey = "images\\highway\\gem2d_yellow_anim.img";
constexpr GH3::QbKey RawYellowHammerAnimTextureKey = "images\\highway\\gem2d_yellow_hammer_anim.img";
constexpr GH3::QbKey RawBlueAnimTextureKey = "images\\highway\\gem2d_blue_anim.img";
constexpr GH3::QbKey RawBlueHammerAnimTextureKey = "images\\highway\\gem2d_blue_hammer_anim.img";
constexpr GH3::QbKey RawOrangeAnimTextureKey = "images\\highway\\gem2d_orange_anim.img";
constexpr GH3::QbKey RawOrangeHammerAnimTextureKey = "images\\highway\\gem2d_orange_hammer_anim.img";
constexpr GH3::QbKey RawStarpowerAnimTextureKey = "images\\highway\\gem2d_starpower_anim.img";
constexpr GH3::QbKey RawStarpowerHammerAnimTextureKey = "images\\highway\\gem2d_starpower_hammer_anim.img";

constexpr GH3::QbKey GreenAnimTextureKey = "sys_Gem2D_green_anim_sys_Gem2D_green_anim";
constexpr GH3::QbKey GreenHammerAnimTextureKey = "sys_Gem2D_green_hammer_anim_sys_Gem2D_green_hammer_anim";
constexpr GH3::QbKey RedAnimTextureKey = "sys_Gem2D_red_anim_sys_Gem2D_red_anim";
constexpr GH3::QbKey RedHammerAnimTextureKey = "sys_Gem2D_red_hammer_anim_sys_Gem2D_red_hammer_anim";
constexpr GH3::QbKey YellowAnimTextureKey = "sys_Gem2D_yellow_anim_sys_Gem2D_yellow_anim";
constexpr GH3::QbKey YellowHammerAnimTextureKey = "sys_Gem2D_yellow_hammer_anim_sys_Gem2D_yellow_hammer_anim";
constexpr GH3::QbKey BlueAnimTextureKey = "sys_Gem2D_blue_anim_sys_Gem2D_blue_anim";
constexpr GH3::QbKey BlueHammerAnimTextureKey = "sys_Gem2D_blue_hammer_anim_sys_Gem2D_blue_hammer_anim";
constexpr GH3::QbKey OrangeAnimTextureKey = "sys_Gem2D_orange_anim_sys_Gem2D_orange_anim";
constexpr GH3::QbKey OrangeHammerAnimTextureKey = "sys_Gem2D_orange_hammer_anim_sys_Gem2D_orange_hammer_anim";
constexpr GH3::QbKey StarpowerAnimTextureKey = "sys_Gem2D_starpower_anim_sys_Gem2D_starpower_anim";
constexpr GH3::QbKey StarpowerHammerAnimTextureKey = "sys_Gem2D_starpower_hammer_anim_sys_Gem2D_starpower_hammer_anim";

constexpr GH3::QbKey RawGreenTapTextureKey = "images\\highway\\gem2d_green_tap.img";
constexpr GH3::QbKey RawRedTapTextureKey = "images\\highway\\gem2d_red_tap.img";
constexpr GH3::QbKey RawYellowTapTextureKey = "images\\highway\\gem2d_yellow_tap.img";
constexpr GH3::QbKey RawBlueTapTextureKey = "images\\highway\\gem2d_blue_tap.img";
constexpr GH3::QbKey RawOrangeTapTextureKey = "images\\highway\\gem2d_orange_tap.img";
constexpr GH3::QbKey RawStarpowerTapTextureKey = "images\\highway\\gem2d_starpower_tap.img";

constexpr GH3::QbKey RawGreenTapStarTextureKey = "images\\highway\\star2d_green_tap.img";
constexpr GH3::QbKey RawRedTapStarTextureKey = "images\\highway\\star2d_red_tap.img";
constexpr GH3::QbKey RawYellowTapStarTextureKey = "images\\highway\\star2d_yellow_tap.img";
constexpr GH3::QbKey RawBlueTapStarTextureKey = "images\\highway\\star2d_blue_tap.img";
constexpr GH3::QbKey RawOrangeTapStarTextureKey = "images\\highway\\star2d_orange_tap.img";

constexpr GH3::QbKey GreenTapTextureKey = "sys_Gem2D_green_tap_sys_Gem2D_green_tap";
constexpr GH3::QbKey RedTapTextureKey = "sys_Gem2D_red_tap_sys_Gem2D_red_tap";
constexpr GH3::QbKey YellowTapTextureKey = "sys_Gem2D_yellow_tap_sys_Gem2D_yellow_tap";
constexpr GH3::QbKey BlueTapTextureKey = "sys_Gem2D_blue_tap_sys_Gem2D_blue_tap";
constexpr GH3::QbKey OrangeTapTextureKey = "sys_Gem2D_orange_tap_sys_Gem2D_orange_tap";
constexpr GH3::QbKey StarpowerTapTextureKey = "sys_Gem2D_starpower_tap_sys_Gem2D_starpower_tap";

constexpr GH3::QbKey GreenTapStarTextureKey = "sys_Star2D_green_tap_sys_Star2D_green_tap";
constexpr GH3::QbKey RedTapStarTextureKey = "sys_Star2D_red_tap_sys_Star2D_red_tap";
constexpr GH3::QbKey YellowTapStarTextureKey = "sys_Star2D_yellow_tap_sys_Star2D_yellow_tap";
constexpr GH3::QbKey BlueTapStarTextureKey = "sys_Star2D_blue_tap_sys_Star2D_blue_tap";
constexpr GH3::QbKey OrangeTapStarTextureKey = "sys_Star2D_orange_tap_sys_Star2D_orange_tap";

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

extern uint32_t g_gemMatSp[6];
extern uint32_t g_gemMatHammerSp[6];

extern uint32_t g_gemMatWhammy[6];
extern uint32_t g_gemMatSpWhammy[6];

extern uint32_t g_gemMatTap[6];
extern uint32_t g_gemMatTapStar[6];
extern uint32_t g_gemMatTapSp[6];

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
