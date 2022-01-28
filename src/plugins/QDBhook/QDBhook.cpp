
#include "gh3\GH3Keys.h"
#include "gh3\GlobalMap.h"
#include "gh3\QbScript.h"
#include "gh3\QbValueType.h"
#include "core\Patcher.h"
#include "QDBhook.h"
#include "gh3\malloc.h"
#include <stdint.h>
#include <stdio.h>
#include <Windows.h>
#include "gh3\GH3Functions.h"

static GH3P::Patcher g_patcher = GH3P::Patcher(__FILE__);


///// NOTE: FOR USE WITH QDB:
///// https://github.com/donnaken15/QDB


char testInd[10];
// implement this to log what's called in what order
QbScript**ScriptStack;
DWORD*ScriptBaseStack;
DWORD ScriptStackCursor = 0;
#define SCR_STACK_MAX 1536
#define CurrentScript ScriptStack[ScriptStackCursor-1]
#define CurrentScriptBase ScriptBaseStack[ScriptStackCursor-1]

HANDLE DebugDataF;
BYTE*  DebugData;
#define MMFsz 0x10000

void ScriptStack_Push(QbScript*scr)
{
	ScriptStack[ScriptStackCursor] = scr;
	ScriptBaseStack[ScriptStackCursor] = (DWORD)scr->instructionPointer;
	ScriptStackCursor++;
}
void ScriptStack_Pop ()
{
	ScriptStack[ScriptStackCursor] = 0;
	ScriptBaseStack[ScriptStackCursor] = 0;
	ScriptStackCursor--;
}

enum GameState : byte // byte 0 of MMF
{
	Running	= 0x00,
	Pause	= 0xF0,
	Step	= 0xF2,
	StepOver= 0xF2 | 1
};

//static DWORD *currentScriptPointer = (DWORD*)0x00B54524;
QbScript*_test;

// FUNCS SUPER BIG ULTRA WEIIIIIIIIRDDDDDDDDDDDDDDDDDDD
// but it jus werx

int  MapStructDepth = 0;

void MapStruct(QbStruct*qs, DWORD*entryAddr, DWORD*countAddr, DWORD*dynavalAddr);

void MapStructItem(QbStructItem*qsi, DWORD*entryAddr, DWORD*countAddr, DWORD*dynavalAddr)
{
	if (!qsi)
		return;
	BYTE qt = qsi->Type();
	*(DWORD*)(DebugData + *entryAddr) = qsi->key;
	*(BYTE *)(DebugData + *entryAddr + 8) = qsi->Type();
	*(BYTE *)(DebugData + *entryAddr + 9) = MapStructDepth;
	QbArray*arr; // stupid
	char*str;
	LPCWSTR strw;
	int strl;
	float*pair;
	float*vec;
	// CAUSING ERRORS WHEN THESE ARE INSIDE CASES
	// WORST PROGRAMMING LANGUAGE EVER
	switch (qt)
	{
	case TypeInt:
	case TypeFloat:
	case TypeQbKey:
	case TypeStringPointer:
		*(DWORD*)(DebugData + *entryAddr + 4) = qsi->value;
		break;
	default:
		*(DWORD*)(DebugData + *entryAddr + 4) = qsi->Type();
		break;
	case TypeQbStruct:
		*(DWORD*)(DebugData + *entryAddr + 4) = 0;
		(*(DWORD*)(DebugData + *countAddr))++;
		*entryAddr += 0x10;
		MapStruct((QbStruct*)qsi->value, entryAddr, countAddr, dynavalAddr);
		*entryAddr += 0x10;
		break;
	case TypeQbArray:
		arr = (QbArray*)qsi->value;
		*(DWORD*)(DebugData + *entryAddr + 4) = arr->Length();
		*(BYTE *)(DebugData + *entryAddr + 9) = MapStructDepth;
		*(BYTE *)(DebugData + *entryAddr + 10) = arr->Type();
		(*(DWORD*)(DebugData + *countAddr))++;
		*entryAddr += 0x10;
		*(DWORD*)(DebugData + *entryAddr + 4) = arr->Length();
		*(BYTE *)(DebugData + *entryAddr + 9) = MapStructDepth;
		*(BYTE *)(DebugData + *entryAddr + 10) = arr->Type();
		MapStructDepth++;
		for (unsigned int i = 0; i < arr->Length(); i++)
		{
			*(DWORD*)(DebugData + *entryAddr) = i;
			*(DWORD*)(DebugData + *entryAddr + 4) = arr->Get(i);
			*(BYTE *)(DebugData + *entryAddr + 8) = arr->Type();
			*(BYTE *)(DebugData + *entryAddr + 9) = MapStructDepth;
			(*(DWORD*)(DebugData + *countAddr))++;
			*entryAddr += 0x10;
		}
		MapStructDepth--;
		break;
	case TypeCString:
		str = (char*)qsi->value;
		strl = strlen(str);
		*(WORD *)(DebugData + *entryAddr + 4) = strl;
		*(WORD *)(DebugData + *entryAddr + 6) = *dynavalAddr;
		memcpy(DebugData + *dynavalAddr, str, strl);
		*dynavalAddr += strl;
		break;
	case TypeWString:
		strw = (LPCWSTR)qsi->value;
		strl = wcslen(strw) << 1;
		*(WORD *)(DebugData + *entryAddr + 4) = strl;
		*(WORD *)(DebugData + *entryAddr + 6) = *dynavalAddr;
		memcpy(DebugData + *dynavalAddr, strw, strl);
		*dynavalAddr += strl;
		break;
	case TypePair:
		pair = (float*)qsi->value;
		*(DWORD*)(DebugData + *entryAddr + 4) = 0;
		*(BYTE *)(DebugData + *entryAddr + 8) = TypeQbArray;
		*(BYTE *)(DebugData + *entryAddr + 9) = MapStructDepth;
		(*(DWORD*)(DebugData + *countAddr))++;
		*entryAddr += 0x10;
		MapStructDepth++;
		*(DWORD*)(DebugData + *entryAddr + 4) = pair[0];
		*(BYTE *)(DebugData + *entryAddr + 8) = TypeFloat;
		*(BYTE *)(DebugData + *entryAddr + 9) = MapStructDepth;
		(*(DWORD*)(DebugData + *countAddr))++;
		*entryAddr += 0x10;
		*(DWORD*)(DebugData + *entryAddr + 4) = pair[1];
		*(BYTE *)(DebugData + *entryAddr + 8) = TypeFloat;
		*(BYTE *)(DebugData + *entryAddr + 9) = MapStructDepth;
		MapStructDepth--;
		break;
	case TypeVector:
		pair = (float*)qsi->value;
		*(DWORD*)(DebugData + *entryAddr + 4) = 0;
		*(BYTE *)(DebugData + *entryAddr + 8) = TypeQbArray;
		*(BYTE *)(DebugData + *entryAddr + 9) = MapStructDepth;
		(*(DWORD*)(DebugData + *countAddr))++;
		*entryAddr += 0x10;
		MapStructDepth++;
		*(DWORD*)(DebugData + *entryAddr + 4) = pair[0];
		*(BYTE *)(DebugData + *entryAddr + 8) = TypeFloat;
		*(BYTE *)(DebugData + *entryAddr + 9) = MapStructDepth;
		(*(DWORD*)(DebugData + *countAddr))++;
		*entryAddr += 0x10;
		*(DWORD*)(DebugData + *entryAddr + 4) = pair[1];
		*(BYTE *)(DebugData + *entryAddr + 8) = TypeFloat;
		*(BYTE *)(DebugData + *entryAddr + 9) = MapStructDepth;
		(*(DWORD*)(DebugData + *countAddr))++;
		*entryAddr += 0x10;
		*(DWORD*)(DebugData + *entryAddr + 4) = pair[2];
		*(BYTE *)(DebugData + *entryAddr + 8) = TypeFloat;
		*(BYTE *)(DebugData + *entryAddr + 9) = MapStructDepth;
		MapStructDepth--;
		break;
	}
}

// name lol
void MapStruct(QbStruct*qs, DWORD*entryAddr, DWORD*countAddr, DWORD*dynavalAddr /* © */)
{
	if (qs)
	{
		if (qs->first)
		{
			// how much space is limited should satisfy enough original game data to be logged I HOPE
			// 0xFFFF should literally be more than enough
			// should check game memory or something to see how much is taken up
			*(DWORD*)(DebugData + *countAddr) = 0;
			QbStructItem*qsi = qs->first;
			//memset(DebugData,0,dynaval);
			if (qsi)
			{
				MapStructDepth++;
				while (1)
				{
					if (!qsi->next) // <--- ??? put in while ??? didnt work i think
						break;
					MapStructItem(qsi, entryAddr, countAddr, dynavalAddr);
					(*(DWORD*)(DebugData + *countAddr))++;
					*entryAddr += 0x10;
					qsi = qsi->next;
				}
				MapStructDepth--;
			}
		}
	}
}

void WriteCSD() // fake x86dbg CPU display like usage whatever
{
	*(DWORD*)(DebugData + 0x10) = CurrentScript->type;
	*(DWORD*)(DebugData + 0x14) = (int)CurrentScript->instructionPointer - (int)CurrentScriptBase;
	DWORD _14dynaval = 0x800;
	DWORD _1Cdynaval = 0x2800;
	DWORD _14entry = 0x20;
	DWORD _1Centry = 0x2000;
	DWORD _14count = 0x1C;
	DWORD _1Ccount = 0x1FFC;
	MapStruct(CurrentScript->qbStruct14, &_14entry, &_14count, &_14dynaval);
	MapStruct(CurrentScript->qbStruct1C, &_1Centry, &_1Ccount, &_1Cdynaval);
	*(DWORD*)(DebugData + 0xA000 - 4) = ScriptStackCursor;
	*(DWORD*)(DebugData + 0x500) = CurrentScript->dword18; // OPTIMIZE????
	*(DWORD*)(DebugData + 0x504) = CurrentScript->dwordA0;
	*(DWORD*)(DebugData + 0x508) = CurrentScript->dwordA4;
	*(DWORD*)(DebugData + 0x50C) = CurrentScript->unkStructPtrA8;
	*(DWORD*)(DebugData + 0x510) = CurrentScript->dwordAC;
	*(DWORD*)(DebugData + 0x514) = CurrentScript->dwordB4;
	*(DWORD*)(DebugData + 0x518) = CurrentScript->dwordB8;
	*(DWORD*)(DebugData + 0x51C) = CurrentScript->dwordC8;
	*(DWORD*)(DebugData + 0x520) = CurrentScript->dwordCC;
	*(DWORD*)(DebugData + 0x524) = CurrentScript->dwordD0;
	*(DWORD*)(DebugData + 0x528) = CurrentScript->dwordD4;
	*(DWORD*)(DebugData + 0x52C) = CurrentScript->unkB0;
	*(DWORD*)(DebugData + 0x530) = CurrentScript->unkBC;
	*(DWORD*)(DebugData + 0x534) = CurrentScript->unkBD;
	*(DWORD*)(DebugData + 0x538) = CurrentScript->unkBE;
	*(DWORD*)(DebugData + 0x53C) = CurrentScript->unkBF;
	*(DWORD*)(DebugData + 0x540) = CurrentScript->unkC4;
	*(DWORD*)(DebugData + 0x544) = CurrentScript->unkC5;
	*(DWORD*)(DebugData + 0x548) = CurrentScript->unkC6;
	*(DWORD*)(DebugData + 0x54C) = CurrentScript->unkC7;
}

using namespace GH3;

QbStruct *nullParams;

void BreakCond()
{
	WriteCSD();
	if (DebugData[0] == Pause)
	{
		DebugData[1] = 1; // ping debugger
						  // ping the booger
		// try using this so game doesn't have to catch up with song somehow
		/*if (DebugData[2] == 1)
		{
			//ExecuteScript2(QbKey("pausegh3"), nullParams, QbKey((uint32_t)0), 0, 0, 0, 0, 0, 0, 0);
			DebugData[2] = 0;
		}*/
	}
	while (DebugData[0] == Pause)
	{
		Sleep(10);
		//if (DebugData[0] == Running)
		{
			//ExecuteScript2(QbKey("unpausegh3"), nullParams, QbKey((uint32_t)0), 0, 0, 0, 0, 0, 0, 0);
		}
	}
	if (DebugData[0] == Step)
	{
		DebugData[0] = Pause;
	}
	if (DebugData[0] == StepOver)
	{
		DebugData[0] = Pause;
	}
	// IMPLEMENT EXIT STATE
	// LAZY
}

DWORD IIIIIIIIIIIIIIIIIIIIIIII;
static void *QBRunDetour1 = (void *)0x00495A9A;
__declspec(naked) void* DebugScriptStart()
{
	static const uint32_t returnAddress = 0x00495AA1;
	__asm {
		inc byte ptr[esi + 0BCh];
		mov[_test], esi;
	};
	ScriptStack_Push(_test);
	for (IIIIIIIIIIIIIIIIIIIIIIII = 0; IIIIIIIIIIIIIIIIIIIIIIII < *(DWORD*)(DebugData + 0xC000 - 4) - 1; IIIIIIIIIIIIIIIIIIIIIIII++)
	{
		if (_test->type == *(DWORD*)(DebugData + 0xC000 + (IIIIIIIIIIIIIIIIIIIIIIII * 4)))
		{
			WriteCSD();
			DebugData[1] = 1;
			DebugData[0] = Pause;
			break;
		}
	}

	__asm {
		mov esi, [_test];
		jmp returnAddress;
	}
}
static void *QBRunDetour2 = (void *)0x00495AC0;
__declspec(naked) void* DebugScript()
{
	static const uint32_t returnAddress = 0x00495AC7;
	__asm {
		// mov     dword_B54524, esi
		mov[_test], esi;
	};
	BreakCond();
	{
		// lol
		if (DebugData[0xE9FB])
		{
			ExecuteScript2(*(DWORD*)(DebugData + 0xE9FC), nullParams, QbKey((uint32_t)0), 0, 0, 0, 0, 0, 0, 0);
			DebugData[0xE9FB] = 0;
		}
	}
	__asm {
		mov esi, [_test];
		test byte ptr[esi + 0C4h], 1;
		jmp returnAddress;
	};
}
static void *QBRunDetour3 = (void *)0x00495B99;
static void *QBRunDetour4 = (void *)0x00495C42;
__declspec(naked) void* DebugScriptStop()
{
	static const uint32_t returnAddress = (uint32_t)(QBRunDetour3) + 7;//0x00495C49;
	__asm {
		dec byte ptr[esi + 0BCh];
		mov[_test], esi;
	};
	ScriptStack_Pop();
	//BreakCond();
	__asm {
		mov esi, [_test];
		jmp returnAddress;
	}
}
__declspec(naked) void* DebugScriptStop2()
{
	static const uint32_t returnAddress = (uint32_t)(QBRunDetour4) + 7;
	__asm {
		dec byte ptr[esi + 0BCh];
		mov[_test], esi;
	};
	ScriptStack_Pop();
	//BreakCond();
	__asm {
		mov esi, [_test];
		jmp returnAddress;
	}
}

void ApplyHack()
{
	DebugDataF = OpenFileMappingA(FILE_MAP_ALL_ACCESS, 1, "GH3_QDebug_DTA");
	DebugData  = (BYTE*)MapViewOfFile(DebugDataF, FILE_MAP_WRITE | FILE_MAP_READ, 0, 0, MMFsz);

	ScriptStack = (QbScript**)&DebugData[0xA000];
	ScriptBaseStack = (DWORD*)&DebugData[0xB000];

	g_patcher.WriteJmp(QBRunDetour1, DebugScriptStart);
	g_patcher.WriteJmp(QBRunDetour2, DebugScript);
	g_patcher.WriteJmp(QBRunDetour3, DebugScriptStop);
	g_patcher.WriteJmp(QBRunDetour4, DebugScriptStop2);

	nullParams = (QbStruct *)qbMalloc(sizeof(QbStruct), 1);
	memset(nullParams, 0, sizeof(QbStruct));
	//nullParams->InsertIntItem(QbKey("player"), 2);
}