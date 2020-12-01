// GH3Hack.cpp : Defines the exported functions for the DLL application.
//

//#define FASTGH3

#ifdef FASTGH3
#include <Windows.h>
#endif

#include "noteLimitFix.h"
#include "core\Patcher.h"
#include <stdint.h>

uint32_t MAX_NOTES = 0x110000; //?1,048,576?
const uint32_t GH3_MAX_PLAYERS = 2;

void * const SIZEOP_NOTE_ALLOCATION = (void *)0x0041AA78;
									
void * const ADDROP_SUSTAINARRAY_1 =  (void *)0x0041EE33;
void * const ADDROP_SUSTAINARRAY_2 =  (void *)0x00423CD4;
void * const ADDROP_SUSTAINARRAY_3 =  (void *)0x00423D02;
void * const ADDROP_FCARRAY =		  (void *)0x00423D14;
void * const ADDROP_NOTEOFFSETARRAY = (void *)0x00423D22;

static float *fixedSustainArray = nullptr;
static float *fixedFcArray = nullptr;
static uint32_t *fixedOffsetArray = nullptr;

static GH3P::Patcher g_patcher = GH3P::Patcher(__FILE__);

// Writes to the code segment of GH3, overwriting the operands responsible for creating or pointing
// towards the fixed 4000 size note arrays with its own allocated arrays
void FixNoteLimit()
{
#ifdef FASTGH3
	WCHAR pwd[MAX_PATH];
	GetCurrentDirectory(MAX_PATH,pwd);
	//SetCurrentDirectory
	MAX_NOTES = GetPrivateProfileIntA("Player", "MaxNotes", 4000, "settings.ini");

	std::wofstream log;
	log.open("log.txt");
	log << L"Loading plugins...\n";
	log.close();
#endif

	if (fixedSustainArray == nullptr)
		fixedSustainArray = new float[MAX_NOTES * GH3_MAX_PLAYERS];

	if (fixedFcArray == nullptr)
		fixedFcArray = new float[MAX_NOTES * GH3_MAX_PLAYERS];

	if (fixedOffsetArray == nullptr)
		fixedOffsetArray =  new uint32_t[MAX_NOTES * GH3_MAX_PLAYERS];

	// Fix dynamic allocations
	g_patcher.WriteInt32(SIZEOP_NOTE_ALLOCATION, MAX_NOTES);

	// Fix static references
	g_patcher.WriteInt32(ADDROP_SUSTAINARRAY_1, reinterpret_cast<uint32_t>(fixedSustainArray));
	g_patcher.WriteInt32(ADDROP_SUSTAINARRAY_2, reinterpret_cast<uint32_t>(fixedSustainArray));
	g_patcher.WriteInt32(ADDROP_SUSTAINARRAY_3, reinterpret_cast<uint32_t>(fixedSustainArray));
	g_patcher.WriteInt32(ADDROP_FCARRAY,		reinterpret_cast<uint32_t>(fixedFcArray));
	g_patcher.WriteInt32(ADDROP_NOTEOFFSETARRAY, reinterpret_cast<uint32_t>(fixedOffsetArray));
}

