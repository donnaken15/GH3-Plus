#include "core\Patcher.h"
#include "gh3\GH3Keys.h"
#include "gh3\GH3GlobalAddresses.h"
#include "tapHopoChord.h"


///////////////////////////////
// Primary gem loading logic //
///////////////////////////////


//Fix open note scaling
//Open notes should always be scaled as large as star notes


void __stdcall setGemScale(float *gemStartScale, float *gemEndScale, uint32_t gemIndex)
{
	static float *globalGemStartScale = reinterpret_cast<float *>(0x00A130C8);
	static float *globalGemEndScale = reinterpret_cast<float *>(0x00A130CC);
	static float *globalGemStarScale = reinterpret_cast<float *>(0x00A130D0);

	float startScale = *globalGemStartScale;
	float endScale = *globalGemEndScale;

	if (gemIndex == 5)
	{
		float starScale = *globalGemStarScale;
		startScale *= starScale;
		endScale *= starScale;
	}

	*gemStartScale = startScale;
	*gemEndScale = endScale;
}

static void * const setGemScaleDetour = (void *)0x00432956;
__declspec(naked) void setGemScaleNaked()
{
	static const void * const returnAddress = (void *)0x00432972;
	__asm
	{
		mov		eax, [esp + 94h]; //gem Index
		lea		ebx, [esp + 28h]; //gem End Scale
		lea		ecx, [esp + 18h]; //gem Start Scale
		push	eax;
		push	ebx;
		push    ecx;
		call	setGemScale;
		jmp		returnAddress;
	}
}






//Star gems
static void * const setStarGemAppearanceDetour = (void *)0x004328C2;
__declspec(naked) void makeStarTapGemsNaked()
{
	const static uint32_t tappingReturn = 0x00432901;

	const static uint32_t bossReturn = 0x004328ED;
	const static uint32_t normalReturn = 0x004328D7;

	//edx = 1
	//edi = gemMaterial2
	//eax = gemIndex
	//ebp = hammerOn flag
	// This is easier to do in asm since it's so simple. The bulk of this is moving the original logic around
	__asm
	{
		cmp ebp, edx; //cmp hopo flag vs 1
		jle L_NOT_TAPPING; //if 0 or 1 then it's strummed or hopo

						   //tapping note
		mov edi, [g_gemMatTapSp + eax * 4];
		jmp tappingReturn;

	L_NOT_TAPPING:
		push eax
			mov eax, ADDR_gameMode
			cmp dword ptr[eax], KEY_P2_BATTLE;
		je L_BOSS_BATTLE;
		mov eax, ADDR_bossBattle
			cmp dword ptr[eax], 0;
		jnz L_BOSS_BATTLE;

		pop eax
			jmp normalReturn;

	L_BOSS_BATTLE:
		pop eax;
		jmp bossReturn;
	}
}

//Normal gems
static void * const setNormalGemAppearanceDetour = (void *)0x0043293A;
__declspec(naked) void makeTapGemsNaked()
{
	const static uint32_t returnAddress = 0x0043294E;
	__asm
	{
		cmp     ebp, edx;
		jg      L_TAPPING_GEM;
		je      L_HAMMER_GEM;

		mov edi, [g_gemMatNormal + eax * 4];
		jmp returnAddress;

	L_HAMMER_GEM:
		mov edi, [g_gemMatHammer + eax * 4];
		jmp returnAddress;

	L_TAPPING_GEM:
		mov edi, [g_gemMatTap + eax * 4];
		jmp returnAddress;
	}
}

//Sp Gems
static void * const setSpGemAppearanceDetour = (void *)0x0043289E;
__declspec(naked) void makeSpTapGemsNaked()
{
	const static uint32_t returnAddress = 0x004328A5;

	//.data:00A17868 g_gemMatHammerSp2 dd 0  
	__asm
	{
		mov		edx, 2;
		cmp		ebp, edx;
		mov		edx, 1;
		jnz		NORMAL_NOTE;

		//Tap notes
		mov		eax, SP_TAP_NOTE_GEM_INDEX;
		mov     edi, [g_gemMatTap + eax * 4];
		jmp		returnAddress;

	NORMAL_NOTE:
		mov     edi, [g_gemMatSp + eax * 4];
		jmp returnAddress;
	}
}

#ifdef OVERLAPPING_STARPOWER
int overlapping_starpower;

// compiler says this (with original names)
// is both undefined and already exists
// WTF >:(
typedef int why2(int);
why2* why = (why2*)(0x004A5960);

int why3;

float tmp;
static void * const setSpStarGemAppearanceDetour = (void *)0x004328A5;
void __declspec(naked) makeSpStarGemsNaked()
{
	static float* globalGemStartScale = reinterpret_cast<float*>(0x00A130C8);
	static float* globalGemEndScale = reinterpret_cast<float*>(0x00A130CC);
	static float* globalGemStarScale = reinterpret_cast<float*>(0x00A130D0);
	const static uint32_t returnAddress  = 0x00432901;
	const static uint32_t returnAddress2 = 0x00432956;

	__asm { mov why3, eax }
	//                      kill me
	overlapping_starpower = why(0x4039F5F1);

	// WHY DOES THIS DETOUR SCREW UP COLORS
	// EVEN WHEN MY OWN CODE DOESN'T RUN
	__asm
	{
		mov  overlapping_starpower, eax
		mov  eax, why3

		mov  edx, overlapping_starpower;
		test edx, edx;
		jz   RETURN_NOT_STAR;

		cmp  bl, 1; // makes below key (EBX) set to `star` if 1
		jne  RETURN_NOT_STAR;

		cmp  ebp, 2;
		jnz  STAR_NOT_TAP;

		mov  edi, g_gemMatTapSpSp[eax * 4];
		jmp  RETURN;

	STAR_NOT_TAP:

		cmp  ebp, 1; // hopo flag
		jnz  STAR_NOT_HOPO;
		mov  edi, g_gemMatHammerSpSp[eax * 4];
		jmp  RETURN;
	STAR_NOT_HOPO:
		mov  edi, g_gemMatSpSp[eax * 4];

	RETURN:
		neg  bl; // ternary operation
		sbb  ebx, ebx;
		and  ebx, 0CF752C9Ch;
		mov [esp + 24h] /*gemType*/, ebx;
		jmp  returnAddress;
		// use star scale if originally a star note
		
		// dumb
	RETURN_NOT_STAR:
		neg  bl;
		sbb  ebx, ebx;
		and  ebx, 0CF752C9Ch;
		mov [esp + 24h], ebx;
		jmp  returnAddress2;
	}
}
#endif

//Point to the new gem constants
bool PatchGemConstants()
{
	// could this be more efficient with an array
	// and make returning if this was successful easier
	g_patcher.WritePointer((void *)(0x00432895 + 3), &g_gemMatHammerSp);
	//g_patcher.WritePointer((void *)(0x0043289E + 3), &g_gemMatSp); <-- We are detouring here so don't patch this one out. Uncomment if detour is removed
	g_patcher.WritePointer((void *)(0x004328F1 + 3), &g_gemMatHammerBattle);
	g_patcher.WritePointer((void *)(0x004328FA + 3), &g_gemMatBattle);
	g_patcher.WritePointer((void *)(0x004328DB + 3), &g_gemMatHammerStar);
	g_patcher.WritePointer((void *)(0x004328E4 + 3), &g_gemMatStar);
	g_patcher.WritePointer((void *)(0x0043293E + 3), &g_gemMatHammer);
	g_patcher.WritePointer((void *)(0x00432947 + 3), &g_gemMatNormal);
	g_patcher.WritePointer((void *)(0x00432D4F + 3), &g_gemMatSpWhammy);
	g_patcher.WritePointer((void *)(0x00432D58 + 3), &g_gemMatWhammy);

	g_patcher.WritePointer((void *)(0x00432813 + 5), &g_gemLeftStartPosX);
	g_patcher.WritePointer((void *)(0x00432825 + 5), &g_gemLeftStartPosY);
	g_patcher.WritePointer((void *)(0x00432834 + 5), &g_gemLeftEndPosX);
	g_patcher.WritePointer((void *)(0x00432843 + 5), &g_gemLeftEndPosY);
	g_patcher.WritePointer((void *)(0x0043284E + 5), &g_gemStartPosX);
	g_patcher.WritePointer((void *)(0x00432860 + 5), &g_gemStartPosY);
	g_patcher.WritePointer((void *)(0x0043286F + 5), &g_gemEndPosX);
	g_patcher.WritePointer((void *)(0x0043287E + 5), &g_gemEndPosY);
	g_patcher.WritePointer((void *)(0x00432D2B + 5), &g_gemLeftAngle);
	g_patcher.WritePointer((void *)(0x00432D36 + 5), &g_gemAngle);

	return true;
	
}








int __fastcall gemStepLoopFixA(GH3::QbArray *note, uint32_t gemIndex)
{
	if (gemIndex != 5)
	{
		if (note->Get(8)) //Is Open
			return false; //Don't allow open note chords
		return note->Get(gemIndex + 1);
	}
	else
		return note->Get(8); //Open note is at index 8
}

static void * const gemStepLoopDetourA = (void *)(0x00435A20);
void __declspec(naked) gemStepLoopFixANaked()
{
	static const void * const returnAddress = (void *)0x00435A2D;
	__asm
	{
		push ecx;
		push edx;

		mov ecx, esi;
		mov edx, ebp;
		call gemStepLoopFixA;

		pop edx;
		pop ecx;
		jmp returnAddress;
	}
}



int __fastcall gemStepLoopFixB(uint32_t gemIndex, QbFretMask fretMask)
{
	QbFretMask gemMask;
	if (gemIndex < 5)
		gemMask = static_cast<QbFretMask>((1 << gemIndex));
	else
		gemMask = QbFretMask::QbOpen;

	if (fretMask & QbOpen && gemIndex != 5) //Don't let people make chords with open notes
		return false;

	return (gemMask & fretMask);
}

static void * const gemStepLoopDetourB = (void *)0x00435BE0;
void __declspec(naked) gemStepLoopFixBNaked()
{
	static const void * const returnAddress = (void *)0x00435BEF;
	__asm
	{
		mov		edx, [esp + 28h]; //note qbfretmask
		mov		ecx, ebp;
		call	gemStepLoopFixB;

		test	eax, eax;
		jmp		returnAddress;
	}
}

#define CRCD(k,n) k


#ifdef OVERLAPPING_STARPOWER
// this requires modified scripts
// in guitar_starpower.qb

char star_power_used;
int boss_battle;

// this code also allows original unpatched code to run
// unless if overlapping_starpower in QB == 1

// patch GiveSPIfPhraseIsHit
// so star sequence can be hit with starpower on
static void* const starpowerusedDetour1A = (void*)0x004303AD;
void __declspec(naked) starpowerusedPatch1A()
{
	// exile v     wtf   v
	static const void* const returnAddress  = (void*)0x0043042E;
	static const void* const returnAddress2 = (void*)0x004303B5;
	static const void* const SP_NOT_USED = (void*)0x00430422;
	static int* bossbattle = (int*)0x00A12BC0;
	__asm push eax
	// why is QbKey constexpr f%$#ing up here
	// i was actually going schizophrenic that
	// it was the below assembly's fault
	
	overlapping_starpower = why(CRCD(0x4039F5F1,"overlapping_starpower"));
	//         ns oldskool style ^
	__asm {
		pop  eax;

		//jmp  OFF; a cliff re*%#@ed compiler
		mov  ecx, overlapping_starpower;
		test ecx, ecx;
		jnz  ACTIVE;

	OFF:
		test al, al;
		jz   SP_NOT_USED_;
		mov  eax, [esp + 14h - 4h];
		jmp  returnAddress2;

	ACTIVE:
		jmp  returnAddress;

	SP_NOT_USED_:
		jmp  SP_NOT_USED;
	}
}

// AwardSP patches
// add starpower amount for player
static void* const starpowerusedDetour1B = (void*)0x00424941;
void __declspec(naked) starpowerusedPatch1B()
{
	static const void* const returnAddress  = (void*)0x0042495F;
	static const void* const returnAddress2 = (void*)0x00424947;
	static const void* const SP_NOT_USED = (void*)0x00424954;
	__asm { push eax }
	overlapping_starpower = why(CRCD(0x4039F5F1, "overlapping_starpower"));
	__asm {
		pop  eax;
		// i forgot what this was for, for a moment

		mov  edx, overlapping_starpower;
		test edx, edx;
		jnz  ACTIVE;

	OFF:
		test al, al;
		jnz  SP_NOT_USED_;
		push 1;
		jmp  returnAddress2;

	ACTIVE:
		mov  edx, [esp + 0Ch]; // stupid
		mov  star_power_used,  dl;
		jmp  returnAddress;

	SP_NOT_USED_:
		jmp  SP_NOT_USED;
	}
}
const float const_50_0 = 50.0f; // should use original direct value but whatever
// skip execution of show_star_power_ready when star power is on
// somehow still does even when detoured to disable it
// specific example: venetian snares mouth autoplay
static void* const starpowerusedDetour1C = (void*)0x00424A22;
void __declspec(naked) starpowerusedPatch1C()
{
	static const void* const returnAddress = (void*)0x00424AD0;
	static const void* const returnAddress2 = (void*)0x00424A2A;
	__asm { push eax }
	overlapping_starpower = why(CRCD(0x4039F5F1, "overlapping_starpower"));
	__asm {
		pop  eax;
		//jmp  OFF; // literally
		mov  edx, overlapping_starpower;
		test edx, edx;
		jz   OFF;
		mov   dl, star_power_used;
		test  dl,  dl; // if (%star_power_used == 1)
		jnz  ACTIVE;

	OFF:
		movss xmm0, const_50_0;
		jmp  returnAddress2; // continue condition

	ACTIVE:
		jmp  returnAddress; // skip condition
	}
}

// detour of "highwayGemSub"'s action star_power_on
// patched to change star gems to my custom textures
// instead of normal gems
//
// 00A15860 = array of type of highway elements (gem,star,fretbar,etc)
// 00A16860 = gem colors (noname when not a gem or star)
// actually don't need to use these because the game automatically
// applies my patch for the gems i want to be stars :)
// except for star opens :(
int*highwayGemColors = (int*)0x00A16860;
static void* const starpowerusedDetour1D = (void*)0x00427A8A;
void __declspec(naked) starpowerusedPatch1D()
{
	static const void* const returnAddress = (void*)0x00427A8F;
	static const void* const returnAddress2 = (void*)0x00427647;
	static const int star = CRCD(0x3624A5EB, "star");
	static const int gem  = CRCD(0x66AF794F, "gem");
	static const int open = CRCD(0x5B8F7C5B, "open");
	static const int starstar     = StarStarTextureKey;
	static const int starstarhopo = StarHammerStarTextureKey;
	static const int starstartap  = StarTapStarTextureKey;
	static const int openstarstar     = OpenStarStarTextureKey;
	static const int openstarstarhopo = OpenStarHammerStarTextureKey;
	overlapping_starpower = why(CRCD(0x4039F5F1, "overlapping_starpower"));
	__asm {
		mov  eax, overlapping_starpower;
		test eax, eax;
		jz   OFF;

	ACTIVE:
		// cmp A15860+(4*i) = star
		mov  eax , highwayGemColors;
		mov  edx , [eax + esi * 4];
		cmp  edx , open; // brain damage
		je   _OPEN;

	NOT_OPEN:
		mov  eax , starstar;
		cmp [esp + 14h], 0; // hopo
		jz   STRUM;
		mov  eax , starstarhopo;
		cmp [esp + 14h], 2; // tap
		je   TAP;
		jmp  STRUM;
		// TODO: scale these
		// use code from where another patch jumps to?
	TAP:
		mov  eax , starstartap;

	STRUM:
		push eax;
		jmp  returnAddress2;

	_OPEN:
		mov  eax , openstarstar;
		cmp [esp + 14h], 0; // hopo
		jz   OPEN_STRUM;
		mov  eax , openstarstarhopo;
		jmp  OPEN_STRUM;
	OPEN_TAP:
		mov  eax , starstartap;

	OPEN_STRUM:
		push eax;
		jmp  returnAddress2;

	OFF: // or just a gem
		cmp [esp + 14h], 0;
		jmp  returnAddress;
	}
	// TODO: make more versitile by pulling
	// tex keys from guitar_gems
	// tried already with changing taps
	// during starpower and crashed
}
#endif






bool TryApplyGemLoadingPatches()
{
	if (!g_patcher.WriteJmp(setGemScaleDetour, &setGemScaleNaked))
		return false;

	if (!g_patcher.WriteJmp(setStarGemAppearanceDetour, &makeStarTapGemsNaked) ||
		!g_patcher.WriteJmp(setNormalGemAppearanceDetour, &makeTapGemsNaked) ||
		!g_patcher.WriteJmp(setSpGemAppearanceDetour, &makeSpTapGemsNaked) ||
		!PatchGemConstants())
		return false;

#ifdef OVERLAPPING_STARPOWER
	if (!g_patcher.WriteJmp(setSpStarGemAppearanceDetour, &makeSpStarGemsNaked) ||
		!g_patcher.WriteJmp(starpowerusedDetour1A, starpowerusedPatch1A) ||
		!g_patcher.WriteJmp(starpowerusedDetour1B, starpowerusedPatch1B) ||
		!g_patcher.WriteJmp(starpowerusedDetour1C, starpowerusedPatch1C) ||
		!g_patcher.WriteJmp(starpowerusedDetour1D, starpowerusedPatch1D)
		)
		return false;
#endif

	if (!g_patcher.WriteJmp(gemStepLoopDetourA, gemStepLoopFixANaked) ||
		!g_patcher.WriteInt8((void*)(0x00435B92+2), 6))
		return false;

	if (!g_patcher.WriteJmp(gemStepLoopDetourB, gemStepLoopFixBNaked) ||
		!g_patcher.WriteInt8((void*)(0x00435D84+2), 6))
		return false;

	return true;
}