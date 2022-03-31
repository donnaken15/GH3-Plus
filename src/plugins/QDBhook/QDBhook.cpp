
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
// most likely won't work with async scripts
QbScript**ScriptStack;
DWORD*ScriptBaseStack;
DWORD ScriptStackCursor = 0;
#define SCR_STACK_MAX 1536
#define CurrentScript ScriptStack[ScriptStackCursor-1]
#define CurrentScriptBase ScriptBaseStack[ScriptStackCursor-1]

HANDLE DebugDataF;
BYTE*  DebugData;
#define MMFsz 0x8000
DWORD entryAlign = 0xC;

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

// FUNCS SUPER BIG ULTRA WEIIIIIIIIRDDDDDDDDDDDDDDDDDDDD
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
		(*(DWORD*)(DebugData + *countAddr))++;
		*entryAddr += entryAlign;
		break;
	default:
		*(DWORD*)(DebugData + *entryAddr + 4) = qsi->Type();
		(*(DWORD*)(DebugData + *countAddr))++;
		*entryAddr += entryAlign;
		break;
	case TypeQbStruct:
		*(DWORD*)(DebugData + *entryAddr + 4) = 0;
		(*(DWORD*)(DebugData + *countAddr))++;
		*entryAddr += entryAlign;
		MapStruct((QbStruct*)qsi->value, entryAddr, countAddr, dynavalAddr);
		*entryAddr += entryAlign;
		break;
	case TypeQbArray:
		arr = (QbArray*)qsi->value;
		*(DWORD*)(DebugData + *entryAddr + 4) = arr->Length();
		*(BYTE *)(DebugData + *entryAddr + 9) = MapStructDepth;
		*(BYTE *)(DebugData + *entryAddr + 10) = arr->Type();
		(*(DWORD*)(DebugData + *countAddr))++;
		*entryAddr += entryAlign;
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
			//if (i != arr->Length())
				*entryAddr += entryAlign;
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
		(*(DWORD*)(DebugData + *countAddr))++;
		*entryAddr += entryAlign;
		break;
	case TypeWString:
		strw = (LPCWSTR)qsi->value;
		strl = wcslen(strw) << 1;
		*(WORD *)(DebugData + *entryAddr + 4) = strl;
		*(WORD *)(DebugData + *entryAddr + 6) = *dynavalAddr;
		memcpy(DebugData + *dynavalAddr, strw, strl);
		*dynavalAddr += strl;
		(*(DWORD*)(DebugData + *countAddr))++;
		*entryAddr += entryAlign;
		break;
	case TypePair:
		pair = (float*)qsi->value;
		*(DWORD*)(DebugData + *entryAddr + 4) = 0;
		*(BYTE *)(DebugData + *entryAddr + 8) = TypeQbArray;
		*(BYTE *)(DebugData + *entryAddr + 9) = MapStructDepth;
		(*(DWORD*)(DebugData + *countAddr))++;
		*entryAddr += entryAlign;
		MapStructDepth++;
		*(DWORD*)(DebugData + *entryAddr) = 0;
		*(DWORD*)(DebugData + *entryAddr + 4) = pair[0];
		*(BYTE *)(DebugData + *entryAddr + 8) = TypeFloat;
		*(BYTE *)(DebugData + *entryAddr + 9) = MapStructDepth;
		(*(DWORD*)(DebugData + *countAddr))++;
		*entryAddr += entryAlign;
		*(DWORD*)(DebugData + *entryAddr) = 1;
		*(DWORD*)(DebugData + *entryAddr + 4) = pair[1];
		*(BYTE *)(DebugData + *entryAddr + 8) = TypeFloat;
		*(BYTE *)(DebugData + *entryAddr + 9) = MapStructDepth;
		(*(DWORD*)(DebugData + *countAddr))++;
		MapStructDepth--;
		*entryAddr += entryAlign;
		break;
	case TypeVector:
		vec = (float*)qsi->value;
		*(DWORD*)(DebugData + *entryAddr + 4) = 0;
		*(BYTE *)(DebugData + *entryAddr + 8) = TypeQbArray;
		*(BYTE *)(DebugData + *entryAddr + 9) = MapStructDepth;
		(*(DWORD*)(DebugData + *countAddr))++;
		*entryAddr += entryAlign;
		MapStructDepth++;
		*(DWORD*)(DebugData + *entryAddr) = 0;
		*(DWORD*)(DebugData + *entryAddr + 4) = vec[0];
		*(BYTE *)(DebugData + *entryAddr + 8) = TypeFloat;
		*(BYTE *)(DebugData + *entryAddr + 9) = MapStructDepth;
		(*(DWORD*)(DebugData + *countAddr))++;
		*entryAddr += entryAlign;
		*(DWORD*)(DebugData + *entryAddr) = 1;
		*(DWORD*)(DebugData + *entryAddr + 4) = vec[1];
		*(BYTE *)(DebugData + *entryAddr + 8) = TypeFloat;
		*(BYTE *)(DebugData + *entryAddr + 9) = MapStructDepth;
		(*(DWORD*)(DebugData + *countAddr))++;
		*entryAddr += entryAlign;
		*(DWORD*)(DebugData + *entryAddr) = 2;
		*(DWORD*)(DebugData + *entryAddr + 4) = vec[2];
		*(BYTE *)(DebugData + *entryAddr + 8) = TypeFloat;
		*(BYTE *)(DebugData + *entryAddr + 9) = MapStructDepth;
		(*(DWORD*)(DebugData + *countAddr))++;
		MapStructDepth--;
		*entryAddr += entryAlign;
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
					//(*(DWORD*)(DebugData + *countAddr))++;
					//*entryAddr += 0x10;
					qsi = qsi->next;
				}
				MapStructDepth--;
			}
		}
	}
}
DWORD IIIIIIIIIIIIIIIIIIIIIIII;

DWORD _dynaval;
DWORD _4entry;
DWORD _14entry;
DWORD _1Centry;
DWORD _18entry;
DWORD _40entry;
DWORD _74entry;
DWORD _4count;
DWORD _14count;
DWORD _1Ccount;
DWORD _18count;
DWORD _40count;
DWORD _74count;

/*const BYTE CSDstrIds[] = {
	0x14, 0x1C, 0x18, 0x40, 0x74
};
const DWORD CSDstrEntries[sizeof(CSDstrIds)] = {
	0x20,
	0x1000,
	0x3000,
	0x4000,
	0x5000
}; // might as well subtract 4 for counter anyway/*
DWORD CSDstrAddrs[sizeof(CSDstrIds)];
DWORD CSDstrCounters[sizeof(CSDstrIds)];*/

DWORD curScriptOffset = 0x2000;
DWORD watchListOffset = 0x6800;

#include <time.h>

void WriteCSD() // fake x86dbg CPU display like usage whatever
{
	*(DWORD*)(DebugData + 0x10) = CurrentScript->type;
	*(DWORD*)(DebugData + 0x14) = (int)CurrentScript->instructionPointer;// -(int)CurrentScriptBase;
	_dynaval = 0x2100;
	/*memcpy(CSDstrAddrs, CSDstrEntries, sizeof(CSDstrIds) << 2);
	memcpy(CSDstrCounters, CSDstrEntries, sizeof(CSDstrIds) << 2);
	for (int i = 0; i < sizeof(CSDstrIds); i++)
	{
		CSDstrCounters[i] -= 4;
		MapStruct(*(QbStruct**)(&CurrentScript + CSDstrIds[i]),
			&CSDstrAddrs[i],
			&CSDstrCounters[i],
			&_dynaval);
	}*/
	//_4entry = 0x800;
	_14entry = 0x20;
	_1Centry = 0x1000;
	_18entry = 0x3000;
	//_40entry = 0x4000;
	_74entry = 0x5000;
	//_4count = 0x7FC;
	_14count = _14entry - 4;
	_1Ccount = _1Centry - 4;
	_18count = _18entry - 4;
	//_40count = _40entry - 4;
	_74count = _74entry - 4;
	//MapStruct(CurrentScript->qbStruct4, &_4entry, &_4count, &_dynaval);
	// make array l:/
	MapStruct(CurrentScript->qbStruct14, &_14entry, &_14count, &_dynaval);
	MapStruct(CurrentScript->qbStruct1C, &_1Centry, &_1Ccount, &_dynaval);
	MapStruct(CurrentScript->qbStruct18, &_18entry, &_18count, &_dynaval);
	//MapStruct(CurrentScript->qbStruct40, &_40entry, &_40count, &_dynaval);
	MapStruct(CurrentScript->qbStruct74, &_74entry, &_74count, &_dynaval);
	*(DWORD*)(DebugData + 0x33FC - 4) = ScriptStackCursor;
	memcpy(DebugData + curScriptOffset, &CurrentScript->gap, 16); // OPTIMIZE????
	*(DWORD*)(DebugData + curScriptOffset + 0x18) = (int)CurrentScript->qbStruct18;
	memcpy(DebugData + curScriptOffset + 0x20, &CurrentScript->unk20, 16 * 8);
	*(DWORD*)(DebugData + curScriptOffset + 0xA0) = CurrentScript->dwordA0;
	*(DWORD*)(DebugData + curScriptOffset + 0xA4) = CurrentScript->dwordA4;
	*(DWORD*)(DebugData + curScriptOffset + 0xA8) = CurrentScript->unkStructPtrA8;
	*(DWORD*)(DebugData + curScriptOffset + 0xAC) = CurrentScript->dwordAC;
	*(DWORD*)(DebugData + curScriptOffset + 0xB0) = CurrentScript->unkB0;
	*(DWORD*)(DebugData + curScriptOffset + 0xB4) = CurrentScript->dwordB4;
	*(DWORD*)(DebugData + curScriptOffset + 0xB8) = CurrentScript->dwordB8;
	*(BYTE *)(DebugData + curScriptOffset + 0xBC) = CurrentScript->unkBC; // related with a function named GameFrame?
	*(BYTE *)(DebugData + curScriptOffset + 0xBD) = CurrentScript->unkBD;
	*(BYTE *)(DebugData + curScriptOffset + 0xBE) = CurrentScript->unkBE;
	*(BYTE *)(DebugData + curScriptOffset + 0xBF) = CurrentScript->unkBF;
	*(DWORD*)(DebugData + curScriptOffset + 0xC0) = (int)CurrentScript->nextIP;
	*(BYTE *)(DebugData + curScriptOffset + 0xC4) = CurrentScript->unkC4;
	*(BYTE *)(DebugData + curScriptOffset + 0xC5) = CurrentScript->unkC5;
	*(BYTE *)(DebugData + curScriptOffset + 0xC6) = CurrentScript->unkC6;
	*(BYTE *)(DebugData + curScriptOffset + 0xC7) = CurrentScript->unkC7;
	*(DWORD*)(DebugData + curScriptOffset + 0xC8) = CurrentScript->node;
	*(DWORD*)(DebugData + curScriptOffset + 0xCC) = CurrentScript->dwordCC;
	*(DWORD*)(DebugData + curScriptOffset + 0xD0) = CurrentScript->dwordD0;
	*(DWORD*)(DebugData + curScriptOffset + 0xD4) = CurrentScript->dwordD4;
	// is there any data in gaps
	// as if i should wonder about that
	// but also what about data that would
	// match up in thug1 code
}
void WriteWatch()
{
	/*
	for (IIIIIIIIIIIIIIIIIIIIIIII = 0;
	     IIIIIIIIIIIIIIIIIIIIIIII < *(DWORD*)(DebugData + 0x6800 - 4);
		 IIIIIIIIIIIIIIIIIIIIIIII++);
	{
		*(DWORD*)(DebugData + 0x7000 + (IIIIIIIIIIIIIIIIIIIIIIII * 4)) =
			*(DWORD*)(DebugData + 0x6800 + (IIIIIIIIIIIIIIIIIIIIIIII * 4));
	}
	*(DWORD*)(DebugData + 0x7000) =
		//time(0) +
		*(DWORD*)(DebugData + 0x6800 - 4);
	*/
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
		// doesn't appear to happen with a chart without audio when pausing for a long time
		// pause music, and then unpause and pause for 16ms trololol
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

static void *QBRunDetour1 = (void *)0x00495A9A;
__declspec(naked) void* DebugScriptStart()
{
	static const uint32_t returnAddress = 0x00495AA1;
	__asm {
		inc byte ptr[esi + 0BCh];
		mov[_test], esi;
	};
	ScriptStack_Push(_test);
	WriteWatch();
	WriteCSD();
	for (IIIIIIIIIIIIIIIIIIIIIIII = 0; IIIIIIIIIIIIIIIIIIIIIIII < *(DWORD*)(DebugData + 0x6000 - 4); IIIIIIIIIIIIIIIIIIIIIIII++)
	{
		if (_test->type == *(DWORD*)(DebugData + 0x6000 + (IIIIIIIIIIIIIIIIIIIIIIII * 4)))
		{
			//WriteCSD();
			DebugData[1] = 1;
			DebugData[0] = Pause;
			break;
		}
	}
	//BreakCond();

	__asm {
		mov esi, [_test];
		jmp returnAddress;
	}
}
 WORD ACEsoff;
QbKey ACEpkey;
QbStruct *ACEargs;
BYTE  ACEptype;
QbKey nullKey = QbKey((uint32_t)0);
QbStructItem*ACEcurItem;
char _itoatmp[11];
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
		if (DebugData[0x29FB])
		{
			DebugData[0x29FB] = 0;
			memset(ACEargs, 0, sizeof(QbStruct));
			//_itoa_s(*(DWORD*)(DebugData + 0xE9F7), _itoatmp, 11, 10);
			//MessageBoxA(0, _itoatmp, "", 0);
			ACEsoff = 0x2A00;
			//ACEcurItem = (QbStructItem*)qbItemMalloc(sizeof(QbStructItem), 1);
			//ACEargs->first = ACEcurItem;
			if (*(DWORD*)(DebugData + 0x29F7) > 0) // ????
			for (IIIIIIIIIIIIIIIIIIIIIIII = 0; IIIIIIIIIIIIIIIIIIIIIIII < *(DWORD*)(DebugData + 0x29F7); IIIIIIIIIIIIIIIIIIIIIIII++)
			{
				ACEpkey = *(DWORD*)(DebugData + ACEsoff);
				ACEptype = *(DWORD*)(DebugData + ACEsoff + 8);
				// TAHNKS NUDE FUNCTION
				// elseif by force of habit
				// who would've thought
				if (ACEptype == TypeInt)
				{
					ACEargs->InsertIntItem  (ACEpkey, *(int  *)(DebugData + ACEsoff + 4));
				}
				else if (ACEptype == TypeFloat)
				{
					ACEargs->InsertFloatItem(ACEpkey, *(float*)(DebugData + ACEsoff + 4));
				}
				else if (ACEptype == TypeQbKey)
				{
					ACEargs->InsertQbKeyItem(ACEpkey, *(DWORD*)(DebugData + ACEsoff + 4));
				}
				//case TypeCString:
				//case TypeWString:
				ACEsoff += 0x10;
			}
			ExecuteScript2(*(DWORD*)(DebugData + 0x29FC), ACEargs, QbKey((uint32_t)0), 0, 0, 0, 0, 0, 0, 0);
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
	DebugDataF = OpenFileMappingA(FILE_MAP_ALL_ACCESS | FILE_MAP_READ | FILE_MAP_WRITE, 1, "GH3_QDebug_DTA");
	if (DebugDataF == INVALID_HANDLE_VALUE || !DebugDataF)
	{
		MessageBoxA(0, "Cannot be hooked to debugger. Close this game and delete QDBhook.dll if you're not using the debugger.", "QDB Hook", MB_ICONWARNING);
		return;
	}
	DebugData = (BYTE*)MapViewOfFile(DebugDataF, FILE_MAP_WRITE | FILE_MAP_READ | FILE_MAP_ALL_ACCESS, 0, 0, MMFsz);
	if (!DebugData)
	{
		MessageBoxA(0, "Unable to access debug data.", "QDB Hook", MB_ICONWARNING);
		return;
	}

	ScriptStack = (QbScript**)&DebugData[0x3400];
	ScriptBaseStack = (DWORD*)&DebugData[0x3800];

	if (!g_patcher.WriteJmp(QBRunDetour1, DebugScriptStart) ||
		!g_patcher.WriteJmp(QBRunDetour2, DebugScript) ||
		!g_patcher.WriteJmp(QBRunDetour3, DebugScriptStop) ||
		!g_patcher.WriteJmp(QBRunDetour4, DebugScriptStop2))
	{
		MessageBoxA(0, "Failed to successfully patch script execution code.", "QDB Hook", MB_ICONWARNING);
		return;
	}

	nullParams = (QbStruct *)qbMalloc(sizeof(QbStruct), 1);
	memset(nullParams, 0, sizeof(QbStruct));

	ACEargs = (QbStruct *)qbMalloc(sizeof(QbStruct), 1);
	memset(ACEargs, 0, sizeof(QbStruct));
}