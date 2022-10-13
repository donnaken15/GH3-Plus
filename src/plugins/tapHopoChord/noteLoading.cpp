#include "core\Patcher.h"
#include "gh3\GH3Keys.h"
#include "gh3\GH3GlobalAddresses.h"
#include "tapHopoChord.h"

//GH3 functions
static const void * const QbKvPairListGetByte = (void *)0x0040A9B0;

//Using global variables as a substitution for stack space
uint32_t g_openLength = 0;
uint32_t g_hopoFlag = 0;


// Open notes - increase fret array
// During loading, mask GRYBO bits if open flag is set























bool __fastcall IsSingleNote(FretMask fretMask)
{
	return (fretMask == FretMask::GREEN ||
		fretMask == FretMask::RED ||
		fretMask == FretMask::YELLOW ||
		fretMask == FretMask::BLUE ||
		fretMask == FretMask::ORANGE ||
		fretMask == FretMask::OPEN);
}

FretMask __fastcall GetFretmaskFromTrackArray(GH3::QbArray *trackArray, int currentNote)
{
	uint32_t result = 0; 
	int realFretMask = trackArray->Get(currentNote + 2);

	if (realFretMask & QbFretMask::QbOpen)
		return FretMask::OPEN;

	if (realFretMask & QbFretMask::QbGreen)
		result = FretMask::GREEN;                 
													
	if (realFretMask & QbFretMask::QbRed)
		result |= FretMask::RED;                  
													
	if (realFretMask & QbFretMask::QbYellow)
		result |= FretMask::YELLOW;                           
													
	if (realFretMask & QbFretMask::QbBlue)
		result |= FretMask::BLUE;                              
													 
	if (realFretMask & QbFretMask::QbOrange)
		result |= FretMask::ORANGE;                           
												
	return (FretMask)result;
}

FretMask __fastcall GetFretmaskFromNoteQbArray(GH3::QbArray *noteArr)
{
	if (noteArr->Get(8))
		return FretMask::OPEN;

	uint32_t fretMask = 0;
		
	if (noteArr->Get(1))
		fretMask |= FretMask::GREEN;

	if (noteArr->Get(2))
		fretMask |= FretMask::RED;

	if (noteArr->Get(3))
		fretMask |= FretMask::YELLOW;

	if (noteArr->Get(4))
		fretMask |= FretMask::BLUE;

	if (noteArr->Get(5))
		fretMask |= FretMask::ORANGE;

	return static_cast<FretMask>(fretMask);
}




// Injected into the code loading an individual note
static void * const hopoFlagDetour = (void *)0x0041D22E;
__declspec(naked) void loadTappingFlagNaked()
{
	static const int frameOffset = 0x48 + 0x4 * 8; //push an addition 4 for each offset register

	//Needed stack variables of GuitarInputFrame
	static const int arg_C = 0x10 + frameOffset;
	static const int obj = 0x04 + frameOffset;
	static const int hopoRaw = -0x28 + frameOffset;

	static const unsigned int returnAddress = 0x0041D239;

	//Too fucking lazy to convert this to C
	//It's just loading the tapping flag and setting hopo flag to 2 if it has one
	_asm
	{
		pushad

		mov      eax, [esp + 0x78]; //[esp + arg_C]
		mov      edx, [eax + 4];
		mov      eax, [eax + 8];
		mov      edi, eax;
		mov      ecx, eax;
		cmp      edx, 1;
		jz       branch_a;
		mov      edi, [eax + esi * 4 + 4];
		mov      ecx, [eax + esi * 4 + 8];

		branch_a:
		sar      ecx, 6;
		and		 ecx, 1;
		cmp      ecx, 1;
		jnz      branch_b;
		mov      edi, 2;
		mov		 [esp + 0x40], edi; //[esp + hopoRaw]

		branch_b :

		popad;

		mov     eax, KEY_PLAYER; //Existing code we pushed out of the way to jump to this cave
		push    eax;
		call    QbKvPairListGetByte;

		jmp     returnAddress; //Return to normal execution

	};
}




void __stdcall LoadOpenHopoTappingBits(GH3::QbArray *noteQbArr, GH3::QbArray *placedNotesQbArr, int currentNote, int noteIndex, float hammerOnTolerance)
{
	//noteQbArr has a repeating sequence of [offset in ms, length in ms, fretmask]
	//placedNotesQbArr is an array of QbArrays

	int sustainLength = 0;
	int openLength = 0;
	HopoFlag hopoFlag; //Actual flag that will be set on the note placed on the highway
	HopoFlag hopoRaw = HopoFlag::STRUMMED; //Calculated hopo value
	bool hopoFlip = false;
	bool tappingFlag = false;

	GH3::QbArray *prevNoteQbArray;

	FretMask fretMask;
	FretMask prevFretMask;
	FretMask currentFretMask;

	int noteQbArrSize = noteQbArr->Length();
	int prevNoteTime;
	int noteTime;

	if (noteQbArrSize != 1)
	{
		sustainLength = noteQbArr->Get(currentNote + 1); 
		fretMask = static_cast<FretMask>(noteQbArr->Get(currentNote + 2));

		if (sustainLength * (fretMask & QbFretMask::QbHopoflip) != 0)
			hopoFlip = true;
	
		if (sustainLength * (fretMask & QbFretMask::QbTapping) != 0)
			tappingFlag = true;

		openLength = sustainLength * ((fretMask >> 7) & 1);
	}

	if (noteIndex > 0)
	{
		prevNoteQbArray = reinterpret_cast<GH3::QbArray *>(placedNotesQbArr->Get(noteIndex - 1));
		prevFretMask = GetFretmaskFromNoteQbArray(prevNoteQbArray);
		currentFretMask = GetFretmaskFromTrackArray(noteQbArr, currentNote);

		prevNoteTime = prevNoteQbArray->Get(0);
		noteTime = noteQbArr->Get(currentNote);

		if (hammerOnTolerance >= ((float)noteTime - (float)prevNoteTime)
			&& IsSingleNote(currentFretMask)
			&& currentFretMask != prevFretMask)
		{
			hopoRaw = HopoFlag::HOPO;
		}
	}

	hopoFlag = static_cast<HopoFlag>((hopoFlip ? 1 : 0) ^ hopoRaw);

	if (tappingFlag)
		hopoFlag = HopoFlag::TAPPING;

	g_hopoFlag = hopoFlag;
	g_openLength = openLength;
}

static void * const LoadHopoFlagDetour = (void *)0x0041D14C;
void __declspec(naked) LoadOpenHopoTappingBitsNaked()
{
	static const unsigned int returnAddress = 0x0041D239;

	__asm
	{
		push	[esp + 40h]; // hammeronTolerance
		push	[esp + (30h + 04h)]; // noteIndex
		push	[esp + (54h + 08h)]; // currentNote
		push	[esp + (34h + 0Ch)]; // placedNotesQbArr
		push	[esp + (58h + 10h)]; // noteQbArr
		call	LoadOpenHopoTappingBits;

		mov		eax, g_hopoFlag;
		mov		[esp + 20h], eax;

		mov     ecx, [esp + 4Ch] //list
		mov     eax, KEY_PLAYER; //Existing code we pushed out of the way to jump to this cave
		push    eax;
		call    QbKvPairListGetByte;
		jmp     returnAddress; //Return to normal execution
	}
}





int __fastcall CreateNoteWithOpenImpl(GH3::QbArray *noteArray, int noteIndex, int noteTime, int greenLength, int redLength, int yellowLength, int blueLength, int orangeLength, int hopoFlag, int nextNoteTime)
{
	GH3::QbArray *note = reinterpret_cast<GH3::QbArray *>(noteArray->Get(noteIndex));

	note->Set(0, noteTime);
	note->Set(1, greenLength);
	note->Set(2, redLength);
	note->Set(3, yellowLength);
	note->Set(4, blueLength);
	note->Set(5, orangeLength);
	note->Set(6, hopoFlag);
	note->Set(7, nextNoteTime);
	note->Set(8, g_openLength);

	return 1;
}

void *CreateNote = (void *)0x0041AB60;
int __declspec(naked) CreateNoteWithOpen()
{
	__asm
	{
		mov edx, ecx;
		mov ecx, eax;
		jmp CreateNoteWithOpenImpl;
	}
}




uint32_t*opensus_len = &g_openLength;
void *SetUpSustainsDetour = (void *)0x0041B7C9;
void __declspec(naked) Sustains4Opens()
{
	static const unsigned int returnAddress = 0x0041B7CE;
	__asm
	{
		mov  esi, 5;
		/*CRINGE VC++ MAKING IT A BYTE PTR*/
		cmp  dword ptr[ecx + 1Ch], 1; // inputArray[8] = open length from CreateNote
		jge  _OPEN;
		jmp  NOT_OPEN; // dumb
	_OPEN:
		mov  esi, 1;
		add  ecx, 1Ch;
	NOT_OPEN:
		jmp  returnAddress;
	}
}




char openName[] = "yellow";
char*openName_ = openName;
void* calculateNoteKey = (void*)0x00418AC0;
//int*lastGemKeys = (int*)0x00A5E8F0;
//int* lastWhammyKeys = (int*)0x00A5E918;
#define MAX_NUM_PLAYERS 2
int g_lastOpenKeys[MAX_NUM_PLAYERS] = { 0,0 };
int g_lastOpenWhammyKeys[MAX_NUM_PLAYERS] = { 0,0 };

void*CheckNoteHoldInit_detour = (void *)0x0041EC59;
void __declspec(naked) CheckNoteHoldInit_4Opens()
{
	static const unsigned int returnAddress = 0x0041EC5E;
	static const unsigned int returnAddress2 = 0x0041ECD2;
	__asm
	{
		mov  ebp, 10000h;
		cmp dword ptr[esp + 18h], 33333h;
		jne  _OPEN;
		and dword ptr [esp + 18h], ~0x11111;

		mov  eax, [esp + 34h - 20h]; // arrayEntry

		mov  edx, openName_;
		push eax;
		push ecx;
		mov  eax, 008A6D44h; // 'gem'
		lea  edi, [esp + 3Ch - 14h]; // *key
		call calculateNoteKey;

		mov  eax, [esp + 3Ch - 24h];
		mov  g_lastOpenKeys[eax], ecx;

		mov  ecx, [esp + 3Ch - 20h]; // arrayEntry
		mov  edx, [esp + 3Ch - 24h]; // playerIndex
		push ecx;
		push edx;

		mov  edx, openName_; // gemColor
		mov  eax, 008A6D38h; // 'whammybar'
		lea  edi, [esp + 44h - 04h]; // *key
		call calculateNoteKey;

		mov  eax, [esp + 44h - 24h]; // playerIndex
		mov  g_lastOpenWhammyKeys[eax], ecx;

		mov  edx, dword ptr [esp + 44h - 14h];
		//shr  eax, 2; // blind shot hoping this part works // dont actually need
		// because the other way didnt work
		//mov  g_lastOpenKeys[eax], edx; // not work
		mov  edx, [esp + 44h - 04h];
		add  esp, 10h;
		add  edx, 1;
		//mov  g_lastOpenWhammyKeys[eax], edx;

		// rewrote this part of a function
		// using an independent array
		// how chad

		jmp  returnAddress2;

		_OPEN:
		jmp  returnAddress;
	}
}

int*noteFxRelated1 = (int*)0x00B54440;
void*element_sub_48F8D0 = (void*)0x0048F8D0;
void*CScreenElement__elementBoxMethod = (void*)0x004F63C0;
void*CheckNoteHoldWait_detour = (void*)0x0042B71F;
void __declspec(naked) CheckNoteHoldWait_4Opens()
{
	static const unsigned int returnAddress = 0x0042B724;
	static const unsigned int returnAddress2_success = 0x0042B778;
	static const unsigned int returnAddress3_fail = 0x0042B782;
	__asm
	{
		cmp  ebx, 33333h;
		jne  NOT_OPEN;
		xor  esi, esi;
		mov  ecx, 00B54440h;
		mov  edx, esi; // lea failed to load a zero pointer that isnt accessed
		add  edx, eax; // why is it even that way other than for optimization
		add  edx, eax; // besides doing two adds
		mov  eax, g_lastOpenKeys[edx * 4];
		push 0;
		push eax;
		lea  eax, [esp + 28h - 0Ch];
		push eax;
		call element_sub_48F8D0;
		mov  ecx, [esp + 20h - 0Ch];
		test ecx, ecx;
		mov  edi, [eax];
		jz   l1;
		lea  edx, [esp + 20h - 0Ch];
		push edx;
		call CScreenElement__elementBoxMethod;
		mov [esp + 20h + 0Ch], 0;
	l1:
		test edi, edi;
		jz   l2;
		mov  eax, [esp + 20h - 10h];
		jmp  returnAddress2_success;
	l2:
		jmp  returnAddress3_fail;

	NOT_OPEN:
		mov  ebp, 10000h;
		jmp  returnAddress;
	}
}

int*fxNoteIdx = (int*)0x00A13134;
void*CheckNoteHoldStart_detour = (void*)0x0042B999;
void __declspec(naked) CheckNoteHoldStart_4Opens()
{
	static const unsigned int returnAddress  = 0x0042B9A4;
	static const unsigned int returnAddress2 = 0x0042B9E9;
	__asm
	{
		mov  ebp, 10000h; // green
		xor  edi, edi;
		mov  ecx, [esp + 28h - 14h];
		cmp  ecx, 33333h;
		jne  NOT_OPEN;

		//and dword ptr[esp + 18h], ~0x11111;

		// i couldve probably just used any of
		// the last gem key slots rather
		// than reimplement this
		// but that could also require
		// redundant separate jmp patches
		mov  eax, [esp + 28h - 18h];
		mov  ecx, 00B54440h;
		lea  edx, [edi + eax * 4];
		add  edx, eax;
		mov  eax, g_lastOpenWhammyKeys[edx * 4];
		push 0;
		push eax;
		lea  eax, [esp + 30h - 0Ch];
		push eax;
		call element_sub_48F8D0;
		mov  ecx, [esp + 28h - 0Ch];
		test ecx, ecx;
		mov  esi, [eax];
		jz   l1;
		lea  edx, [esp + 28h - 0Ch];
		push edx;
		call CScreenElement__elementBoxMethod;
		mov [esp + 28 - 0Ch], 0;
	l1:
		test esi, esi;
		mov  edi, 4;
		jmp  returnAddress2;

	NOT_OPEN:
		jmp  returnAddress;
	}
}

void* CheckNoteHoldPerFrame_detour = (void*)0x0042BC5B;
void __declspec(naked) CheckNoteHoldPerFrame_4Opens()
{
	static const unsigned int returnAddress = 0x0042BC60;
	__asm
	{
		mov  eax, 1;
		cmp  ebx, 33333h;
		jne  NOT_OPEN;
		// invert condition
		// so if frets are held,
		// cut off the note
		xor  ebx, ebx;
	NOT_OPEN:
		cmp  esi, ebx;
		jmp  returnAddress;
	}
}

// detour CalcSongScoreInfo to add
// points for open sustains too
void* CalculateSustainPoints_detour = (void*)0x00423AFE;
void __declspec(naked) CalculateSustainPoints_4Opens()
{
	static const unsigned int returnAddress  = 0x00423B03;
	static const unsigned int returnAddress2 = 0x00423A90;
	__asm
	{
		add  ecx, ebx;
		cmp  ecx, 5;
		je   _OPEN;
		jl   CONTINUE;
		//cmp  ecx, 7;
		//je   _OPEN_DONE;

	//_OPEN_DONE:
		jmp  returnAddress;

	_OPEN:
		mov  ecx, 7;
		// no copy and paste this time :nice:
	CONTINUE:
		jmp  returnAddress2;
	}
}

bool TryApplyNoteLoadingPatches()
{


	return (g_patcher.WriteInt8((void *)0x0041AAD6, 9) && // Allocate an extra slot for open notes in the open note structure (normally 8)
			g_patcher.WriteJmp(hopoFlagDetour, LoadOpenHopoTappingBitsNaked) &&

			g_patcher.WriteJmp(CreateNote, CreateNoteWithOpen) && //Substitute our own CreateNote function that handles open notes
			g_patcher.WriteJmp(SetUpSustainsDetour, Sustains4Opens) && // Hack sustain code to apply note lengths for opens too
			g_patcher.WriteJmp(CheckNoteHoldInit_detour, CheckNoteHoldInit_4Opens) && // wtf
			g_patcher.WriteJmp(CheckNoteHoldWait_detour, CheckNoteHoldWait_4Opens) &&
			g_patcher.WriteJmp(CheckNoteHoldStart_detour, CheckNoteHoldStart_4Opens) &&
			g_patcher.WriteJmp(CheckNoteHoldPerFrame_detour, CheckNoteHoldPerFrame_4Opens) &&
			g_patcher.WriteJmp(CalculateSustainPoints_detour, CalculateSustainPoints_4Opens) &&
			g_patcher.WriteNOPs((void *)0x0041D452, 3) && //NOP out the "sub esp 20h" following any CreateNote call since we are cleaning up the stack ourselves (effectively changing the calling convention)
			g_patcher.WriteNOPs((void *)0x0041AD97, 3) &&
			g_patcher.WriteInt8((void *)0x0041D451, 0x28)); //Since the stack will no longer be off by 0x20 we need to fix anything that grabs a stack variable before it is correct);
}
