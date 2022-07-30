#include "gh3\GH3Keys.h"
#include "core\Patcher.h"
#include "Modifiers.h"
#include <stdint.h>
#include <stdio.h>
#include <Windows.h>

static GH3P::Patcher g_patcher = GH3P::Patcher(__FILE__);

using namespace GH3;

static char inipath[MAX_PATH];
static BYTE flags = 0;
LPCSTR modnames[] = {
	"AllStrums",
	"AllTaps",
	"AllDoubles",
	"Hopos2Taps",
	"Mirror",
	"ColorShuffle",
};
enum modtypes {
	AllStrums    = 1 << 0,
	AllTaps      = 1 << 1,
	AllDoubles   = 1 << 2,
	Hopos2Taps   = 1 << 3,
	Mirror       = 1 << 4,
	ColorShuffle = 1 << 5
};

#pragma region COLOR SHUFFLE

#include <random>
#include "gh3\QbArray.h"

static uint32_t g_randomIndex;

#ifdef opennote_mod
static uint32_t openNoteIndex = 5;
#endif

void randomizeIndex()
{
	g_randomIndex = rand() % 5;
}

static void* const fruityDetour = (void*)0x00432887; //0x004327D2; // 0x00432733;

__declspec (naked) void fruityNaked()
{
	static const uint32_t returnAddress = 0x0043288F;

	__asm
	{
		test[flags], ColorShuffle
		jz      COLORSHUFFLE_SKIP

#ifdef opennote_mod
		cmp		eax, openNoteIndex;
		je		EXIT;
#endif
		pushad;
		call	randomizeIndex;
		popad;
		mov		eax, g_randomIndex;
#ifdef opennote_mod
		EXIT :
#endif

	COLORSHUFFLE_SKIP:
		test    ecx, ecx
		movss[esp + 34h], xmm0

		jmp     returnAddress;
	}
}

#pragma endregion

#pragma region MIRROR MODE

#define ENABLE_MIRROR 1

// conflicts with TapHopoChord when correcting HOPO flags
// for GetFretmaskFromNoteQbArray and double notes

#if ENABLE_MIRROR
enum class FretMask : uint32_t
{
	ORANGE = 0x00001,
	BLUE   = 0x00010,
	YELLOW = 0x00100,
	RED    = 0x01000,
	GREEN  = 0x10000,
};

// Reverse the parameter order for the CreateNote call
static void* const createNote4ReverseDetour = (void*)0x0041D41B;

__declspec(naked) void createNoteReversedNaked()
{
	static void* const returnAddress = (void*)0x0041D449;
	__asm
	{
#if ENABLE_MIRROR
		test [flags],Mirror
		jz      MIRROR_SKIP

		//mov     edx, [esp + 20h]; // [esp+48h+hopoRaw]
		mov     ecx, [esp + 18h]; // [esp+48h+redSustainLength]
		mov     esi, [esp + 30h]; // [esp+48h+currentStartTime]
		push    eax; //      nextNoteTime
		mov     eax, [esp + 28h]; // [esp+4Ch+greenSustainLength]
		push    edx; //      hopoFlag
		mov     edx, [esp + 18h]; // [esp+50h+yellowSustainLength]
		push    eax; //      greenLength
		mov     eax, [esp + 20h]; // [esp+54h+blueSustainLength]
		push    ecx; //      redLength
		mov     ecx, [esp + 2Ch]; // [esp+58h+orangeSustainLength]
		push    edx; //      yellowLength
		mov     edx, [esp + 40h]; // [esp+5Ch+noteTime2]
		push    eax; //      blueLength
		mov     eax, [esp + 4Ch]; // [esp+60h+noteArray] ; noteArray
		push    ecx; //      greenLength
		push    edx; //      noteTime
		mov     ecx, esi; // noteIndex
		jmp		returnAddress;

	MIRROR_SKIP:
#endif
		//mov     edx, [esp + 20h];
		mov     ecx, [esp + 14h];
		mov     esi, [esp + 30h];
		push    eax;
		mov     eax, [esp + 20h];
		push    edx;
		mov     edx, [esp + 18h];
		push    eax;
		mov     eax, [esp + 24h];
		push    ecx;
		mov     ecx, [esp + 34h];
		push    edx;
		mov     edx, [esp + 40h];
		push    eax;
		mov     eax, [esp + 4Ch];
		push    ecx;
		push    edx;
		mov     ecx, esi;
		jmp		returnAddress;
	}
}

static void* const GetFretmaskFromNoteQbArrayDetour = (void*)0x0041B090;

uint32_t __fastcall GetMirroredFretmaskFromNoteQbArray(QbArray* qbArr)
{
	uint32_t size = qbArr->size;
	uint32_t* frets = qbArr->arr;
	uint32_t fretMask = 0;
	uint32_t fret = reinterpret_cast<uint32_t>(frets); //Please don't ask me the reasoning behind this insanity. The neversoft devs are mad

	static FretMask maskOrder[] = {
		FretMask::GREEN,
		FretMask::RED,
		FretMask::YELLOW,
		FretMask::BLUE,
		FretMask::ORANGE
	};
	static FretMask maskOrderMirror[] = {
		FretMask::ORANGE,
		FretMask::BLUE,
		FretMask::YELLOW,
		FretMask::RED,
		FretMask::GREEN
	};
	FretMask* currentMask = maskOrder;
	if (flags & Mirror) // wtf
		currentMask = maskOrderMirror;
	else if (flags & ~Mirror)
		currentMask = maskOrder;

	if (size != 1)
		fret = frets[1];
	if (fret > 0)
		fretMask |= (uint32_t)currentMask[0];

	if (size != 1)
		fret = frets[2];
	if (fret > 0)
		fretMask |= (uint32_t)currentMask[1];

	if (size != 1)
		fret = frets[3];
	if (fret > 0)
		fretMask |= (uint32_t)currentMask[2];

	if (size != 1)
		fret = frets[4];
	if (fret > 0)
		fretMask |= (uint32_t)currentMask[3];

	if (size != 1)
		fret = frets[5];
	if (fret > 0)
		fretMask |= (uint32_t)currentMask[4];

	return fretMask;
}
#endif

#pragma endregion

#pragma region DOUBLE NOTES

__declspec(naked) void tinyJump()
{
	static const uint32_t returnAddress = 0x0041D298;
	static const uint32_t jzAddr = 0x0041D3FA;
	__asm
	{
		test[flags], AllDoubles;
		jz  STUPIDASM2;

		mov ecx, ebp;
		jmp returnAddress;

	STUPIDASM2:
		jmp jzAddr;
	}
}

static void* hopoCheckDetour = (void*)0x0041D1D2;
static void* GetFretmaskFromNoteQbArray = (void*)0x0041B090;
static void* GetFretmaskFromTrackArray = (void*)0x0041B100;
__declspec (naked) void hopoCheckNaked()
{
	static const uint32_t returnAddress = 0x0041D1E4;

	__asm
	{
		//Only use the old method if we're literally on the first note.
		//OR if this is disabled
		test[flags], AllDoubles
		jnz     DOUBLENOTES_ROUTINE

		mov     ecx, edi;
		call    GetFretmaskFromNoteQbArray;
		mov     ecx, [esp + 58h];
		mov     ebp, eax;
		call    GetFretmaskFromTrackArray;
		jmp		returnAddress;

	DOUBLENOTES_ROUTINE:

		sub		esi, 3
		cmp     esi, 0
		jge     NEW_METHOD

		//Old method
		add     esi, 3 //correct ESI
		mov     ecx, edi; //this
		call    GetFretmaskFromNoteQbArray;
		mov     ebp, eax;
		jmp		GET_FRETMASK;

	NEW_METHOD:
		mov		ecx, [esp + 58h];
		call    GetFretmaskFromTrackArray
		mov     ebp, eax;
		add		esi, 3 //correct ESI

	GET_FRETMASK:
		mov		ecx, [esp + 58h];
		call	GetFretmaskFromTrackArray;
		mov     ecx, eax;

		jmp		returnAddress;
	}
}

#pragma endregion

#pragma region FORCE STRUMS OR TAPS

static void* setHopoFlagDetour2 = (void*)0x0041D41B;

__declspec(naked) void forceTapFlagNaked2()
{
	static const uint32_t returnAddress = 0x0041D423;
	__asm
	{
		test[flags], AllTaps
		jnz     FORCETAPS_ROUTINE
		test[flags], AllStrums
		jnz     FORCESTRUMS_ROUTINE
		test[flags], Hopos2Taps
		jnz     H2T_ROUTINE

		mov     edx, [esp + 48h - 28h]; // note flag?
		mov     ecx, [esp + 48h - 34h]; // *something
		mov     esi, [esp + 48h - 18h];
#if ENABLE_MIRROR
		jmp     createNoteReversedNaked;
#endif
		jmp		returnAddress;

	H2T_ROUTINE:
		test[esp + 48h - 28h], 1;
		jnz     FORCETAPS_ROUTINE;
		test[esp + 48h - 28h], 2;
		jnz     FORCETAPS_ROUTINE;
		jmp     FORCESTRUMS_ROUTINE;
	FORCESTRUMS_ROUTINE:
		mov     edx, 0;
		jmp     NOTE_FLAG_SET;
	FORCETAPS_ROUTINE:
		mov     edx, 2;
		jmp     NOTE_FLAG_SET;
	NOTE_FLAG_SET:
		mov		ecx, [esp + 14h]; // *
#if ENABLE_MIRROR
		jmp     createNoteReversedNaked;
#endif
		jmp		returnAddress;
	}
}

#pragma endregion

#pragma region

#pragma endregion

// TODO: FIX HOPO CHECKS ON DOUBLE NOTES.
// TAPHOPOCHORD CAUSES A STRAIGHT LINE
// OF DOUBLE NOTES TO HAVE HOPOS,
// AND STILL HAVE ORIGINAL HOPO FLAGS
// OF SINGLE NOTES WITHOUT THE MOD

void ApplyHack()
{
	GetCurrentDirectoryA(MAX_PATH, inipath);
	strcat_s(inipath, MAX_PATH, "\\settings.ini");
	for (int i = 0; i < 8; i++)
		flags |= (GetPrivateProfileIntA("Modifiers", modnames[i], 0, inipath) != 0) << i;
	// DOUBLE NOTES
	g_patcher.WriteJmp((void*)0x0041D288, &tinyJump);
	g_patcher.WriteJmp(hopoCheckDetour, hopoCheckNaked);
	// FORCE STRUMS
	g_patcher.WriteJmp(setHopoFlagDetour2, &forceTapFlagNaked2);
	// COLOR SHUFFLE
	g_patcher.WriteJmp(fruityDetour, &fruityNaked);
	// MIRROR MODE
#if ENABLE_MIRROR
	g_patcher.WriteJmp(GetFretmaskFromNoteQbArrayDetour, GetMirroredFretmaskFromNoteQbArray);
#endif
}
