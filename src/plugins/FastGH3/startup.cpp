#include "gh3\GH3Keys.h"
#include "gh3\QbStruct.h"
#include "gh3\QbScript.h"
#include "core\Patcher.h"
#include "startup.h"
#include <stdint.h>
#include <stdio.h>
#include <Windows.h>
#include <mmsystem.h>
#include <d3d9.h>
#include <D3dx9tex.h>

// TODO!!!!!!!!:
// detour D3D9 load texture from memory
// function to see if I can use a
// faster alternative

#pragma comment(lib, "E:\\D3D9\\d3dx9.lib")
#pragma comment(lib, "winmm.lib")
static GH3P::Patcher g_patcher = GH3P::Patcher(__FILE__);

static void* D3DPPpi = (void*)0x0057BB79;
static LPDIRECT3DDEVICE9* d3ddev = (LPDIRECT3DDEVICE9*)0x00C5C7A8;

HWND* hWnd;
RECT WndR;
int sizeX, sizeY, centerX, centerY;

BYTE vsync, borderless, windowed;

static void* hWndDetour = (void*)0x0057BA6F;
static int* wndStyle = (int*)0x0057BA5D;
__declspec(naked) void hWndHack()
{
	static const uint32_t returnAddress = 0x0057BA75;
	{
		hWnd = (HWND*)0xC5B8F8;
		GetWindowRect(*hWnd, &WndR);
		sizeX = WndR.right - WndR.left;
		sizeY = WndR.bottom - WndR.top;
		centerX = (GetSystemMetrics(SM_CXSCREEN) / 2) - (sizeX / 2);
		centerY = (GetSystemMetrics(SM_CYSCREEN) / 2) - (sizeY / 2);
		if (!borderless)
		{
			sizeX += (GetSystemMetrics(SM_CXEDGE) * 2) + 2;
			sizeY += (GetSystemMetrics(SM_CYFIXEDFRAME) * 2)
				+ GetSystemMetrics(SM_CYCAPTION)
				+ GetSystemMetrics(SM_CXFIXEDFRAME) - 3;
			// figure this out so its just dependent on GSM
			// without these extra constants // i tried everything here...
			// its actually impossible
		}
		SetWindowPos(*hWnd, 0,
			centerX, centerY, sizeX, sizeY,
			SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOACTIVATE);
		__asm jmp returnAddress;
	}
	// lazy to find and write another one time detour

}

#define SCREENSHOT 0

float* GameRes_X = (float*)0x00C5E6B0;
#if SCREENSHOT
int* GameRes_Xi = (int*)0x00C5E6B8;
int* GameRes_Yi = (int*)0x00C5E6BC;
using namespace GH3;
static void* screenshotDetour = (void*)0x005377B0;
bool ScreenShot(QbStruct* str, QbScript* scr)
{
	// turned off because of D3DXSaveSurfaceToFile bloating the DLL by 334KB
	// "just make it a separate plugin"
	// also UPX
	IDirect3DSurface9* pSurface;
	IDirect3DSurface9* surf;
	D3DSURFACE_DESC sd;

	(*d3ddev)->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pSurface);
	pSurface->GetDesc(&sd);
	(*d3ddev)->CreateOffscreenPlainSurface(
		sd.Width, sd.Height, sd.Format, D3DPOOL_SYSTEMMEM, &surf, NULL);
	(*d3ddev)->GetRenderTargetData(pSurface, surf);

	char maindir[MAX_PATH];
	char filepath[MAX_PATH];
	char* filename;
	if (!str->GetString(QbKey("filename"), filename))
		filename = "screen";
	GetCurrentDirectoryA(MAX_PATH, maindir);
	sprintf_s(filepath, "%s\\%s.bmp", maindir, filename);
	// WHY DO PNG AND JPG FAIL
	// DDS USES ARGB8 AND IS THE SAME SIZE AS BITMAP >:(
	// AND WHY IS IT SO BIG FOR WRITING AN UNCOMPRESSED
	// RGB32 STUPIDLY LARGE SCREENSHOT

	D3DXSaveSurfaceToFileA(filepath, D3DXIFF_BMP, pSurface, NULL, NULL);

	pSurface->Release();
	surf->Release();
	return 1;
}
#endif

// copied from my OGL game test
HANDLE frameLimiter;
typedef long long dl;
typedef unsigned long long dq;
dl frameBase, frameTime;
dl LAG_1, LAG_2;
dl CLOCK_FREQ;
float frameRate = 60.0f; // target FPS limit

#define FRAMERATE_FROM_QB 1

#include <gh3\GH3Functions.h>

float realFPS;
bool couldntCap = false;

static void* beforeMainloopDetour = (void*)0x00401FFD;
void initFrameTimer()
{
	QueryPerformanceFrequency((LARGE_INTEGER*)&CLOCK_FREQ);
	frameBase = -(CLOCK_FREQ / frameRate);
}

//nullsub detoured for testing something
static void* profiletimeDetour = (void*)0x00537300;
dl PROFILE_TIME;
double test2;
bool ProfileTime(QbStruct* str, QbScript* scr)
{
	timeBeginPeriod(1);
	QueryPerformanceCounter((LARGE_INTEGER*)&PROFILE_TIME);
	timeEndPeriod(1);
	scr->qbStruct1C->InsertIntItem(KEY_TIME, PROFILE_TIME&0x7FFFFFFF);
	//CRCD(0x26A3E82E,"profile_time")
	return 1;
}

// doesn't work?
//extern "C" NTSYSAPI NTSTATUS NTAPI NtSetTimerResolution(ULONG DesiredResolution, BOOLEAN SetResolution, PULONG CurrentResolution);

static char inipath[MAX_PATH];

int killswitchTimer = 0;
//int killswitchCheckInterval = 100;

typedef int lol2();
lol2* lol3 = (lol2*)(0x005377F0);

static void* beforePresentDetour = (void*)0x0048453B;
void frameLimit()
{
#if (FRAMERATE_FROM_QB)
	frameRate = GlobalMapGetInt(QbKey("fps_max")); // can't get a float >:(
#endif

	// kill me
#if 0
	if (killswitchTimer++ < (frameRate * 2))
	{
		killswitchTimer = 0;
		if (GetPrivateProfileIntA("Misc", "Killswitch", 0, inipath))
		{
			WritePrivateProfileStringA("Misc", "Killswitch", "0", inipath);
			lol3();
		}
	}
#endif

	if (!frameRate)
	{
		couldntCap = true;
		timeBeginPeriod(1);
		QueryPerformanceCounter((LARGE_INTEGER*)&LAG_1);
		timeEndPeriod(1);
		return;
	}
	timeBeginPeriod(1);
	// get timestamp before rendering a frame
	QueryPerformanceCounter((LARGE_INTEGER*)&frameTime);
	// and then do math to set how much time to wait
	// subtracted by how much time game code ran for
	frameBase = -(CLOCK_FREQ / frameRate);
	// what's even the point of float cast
	// when using whole number limits
	// and when I CAN'T EVEN GET A
	// FLOAT IN QB THROUGH C++
	frameTime += frameBase - LAG_1;
	if (frameTime < 0) // positive value means absolute time for these funcs but
		// guessing that if it didnt get a time long enough to subtract
		// frame rate delta from it without going inverse, make later
		// function rely on game's original delta
		// because somehow, even when the limit isnt reached
		// when vsync is on, and despite changing the code,
		// so the particles have to rely on two timestamps,
		// the particles still act like its using the unreachable limit
		// WTFFFFFFF!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	{
		couldntCap = false;
		frameLimiter = CreateWaitableTimerA(NULL, 1, NULL);
		SetWaitableTimer(frameLimiter, (LARGE_INTEGER*)&frameTime, 0, NULL, NULL, 0);
		WaitForSingleObject(frameLimiter, (1.0f / frameRate) * 1000);
		CloseHandle(frameLimiter);
	}
	else
		couldntCap = true;
	QueryPerformanceCounter((LARGE_INTEGER*)&LAG_1);
	timeEndPeriod(1);
	// still unstable around 120 FPS :toolcoder:
}
lol2* lol = (lol2*)(0x00492780);
static void* afterPresentDetour = (void*)0x0048458B;
float* g_delta = (float*)0x009596BC;
//int test2 = 0;
void lagBegin()
{
	lol();
	if (!couldntCap) // stupid
		realFPS = 1.0f / ((float)(LAG_1 - LAG_2) / 10000000);
	else // read line 173 for why this is like this
		realFPS = 1.0f / *g_delta;
	timeBeginPeriod(1);
	//get a timestamp immediately after rendering a frame
	QueryPerformanceCounter((LARGE_INTEGER*)&LAG_2);
	timeEndPeriod(1);
}

static char test[10];
static int* presint = (int*)0x00C5B934;
static char* CD = (char*)0x00B45A11;

// patch framerate fixed velocity and friction of particles
float frameFrac = 60.0f;
float* g_gameSpeed1 = (float*)0x009596B4;
static void* Upd2DPSys_detour = (void*)0x00428E4C;
__declspec(naked) void velocityFix()
{
	static const uint32_t returnAddress = 0x00428E51;
	__asm {
		movss   dword ptr[esi + 8], xmm3;

		// this is our velocity mod
		// velMod =
		movss   xmm4, dword ptr realFPS;   // (frameRate
		divss   xmm4, dword ptr frameFrac; //            / frameFrac)
		push    eax;
		mov     eax, dword ptr g_gameSpeed1; // also apply fix for slomo
		divss   xmm4, [eax];
		divss   xmm1, xmm4; // vel = vel * velMod
		pop     eax;

		jmp returnAddress;
	}
}
static void* Upd2DPSys_detour2 = (void*)0x00429178;
__declspec(naked) void velocityFix2()
{
	static const uint32_t returnAddress = 0x0042917D;
	__asm {
		movss   dword ptr[esi + 8], xmm5;

		movss   xmm6, dword ptr realFPS;
		divss   xmm6, dword ptr frameFrac;
		push    eax;
		mov     eax, dword ptr g_gameSpeed1;
		divss   xmm6, [eax];
		divss   xmm3, xmm6;
		pop     eax;

		/*
		* tried to fix for particles
		* falling shorter and slower
		* with slomo when FPS is not
		* limited
		cmp     couldntCap, 0;
		jnz     COULDNT_CAP;
		divss   xmm4, [eax];
		jmp     SKIP;
	COULDNT_CAP:
		mulss   xmm4, [eax];
	SKIP:
		*/

		jmp returnAddress;
	}
}

int BossAttack_waitFrames = 100;
// TODO: use global value/create boss props value
int BossAttack_waitFrames_ = BossAttack_waitFrames;
// could i probably make this more efficent by using GetSongTime and setting wait time to 1 until (100 frames in seconds) is reached
static void* BossWaitForAttack_framesDetour1 = (void*)0x00D7EBF1;
__declspec(naked) void BWFA_frames2Realtime1()
{
	static const uint32_t returnAddress = 0x00D7EBF8;
	BossAttack_waitFrames_ = (float)(BossAttack_waitFrames) * (realFPS / frameFrac);
	__asm {
		mov edx, BossAttack_waitFrames_;
		mov dword ptr[esi + 0Ch], edx;

		jmp returnAddress;
	}
}
static void* BossWaitForAttack_framesDetour2 = (void*)0x00D7EC0B;
__declspec(naked) void BWFA_frames2Realtime2()
{
	static const uint32_t returnAddress = 0x00D7EC12;
	BossAttack_waitFrames_ = (float)(BossAttack_waitFrames) * (realFPS / frameFrac);
	__asm {
		mov edx, BossAttack_waitFrames_;
		mov dword ptr[esi + 0Ch], edx;

		jmp returnAddress;
	}
}

float resCurrent = 1280.0f;
float resFrac = 1280.0f;
float dummy;
static void* whammyWidth_Detour = (void*)0x0060354C;
__declspec(naked) void whammyWidthFix()
{
	static const uint32_t returnAddress = 0x00603552;
	__asm {
		movss   dummy, xmm0;

		push    eax;
		mov     eax, dword ptr GameRes_X;
		movss   xmm0, [eax]; // couldnt i just mov value, [eax]
		// what's even the point of this in compilation
		// when nothing is being done to the float
		movss   resCurrent, xmm0;
		pop     eax;
	}
	// ez hack overwriting xmm0 using temporary math operations
	dummy = dummy * (resCurrent / resFrac);
	__asm {
		movss[esp + 58h], xmm0;

		jmp returnAddress;
	}
}

#include "mp3seek.h"

#if ACCURATETIME

WORD** VBRfr;
int* VBRfrL;
int* VBRlens;
BYTE* isVBR;
int baseBR = 128;

/**/typedef int imgbase_func();
imgbase_func* setPosition = (imgbase_func*)(0x007A57AC);
imgbase_func* getPosition = (imgbase_func*)(0x007A5764);
imgbase_func* QBStr_GetValue = (imgbase_func*)(0x00478E50);
//imgbase_func* getWaveData = (imgbase_func*)(0x007A57C4);

// convert time to accurate time for FMOD
float VBR2CBR(WORD* BR, float fcount, int maxf, int freq)
{
	float nsamp = 0;
	if (fcount > maxf)
		fcount = maxf;
	for (int i = 0; i < (int)fcount; i++)
	{
		nsamp += ((float)baseBR / BR[i]); // * 128
	}
	nsamp += fmod(fcount, 1); // lol
	nsamp *= ((float)spf / freq);
	return nsamp;
}
FILE* test3;

#define __FC 4
#define __ESIZE 0x50
#define __SSIZE 8 + (__ESIZE * __FC)
#define __FNSIZE 30
const int ssize = __SSIZE;
const int fc = __FC; // FC ACAIPOG
const int esize = __ESIZE;
const int fnsize = __FNSIZE;
typedef struct {
	uint16_t ssize; // __SSIZE
	char fname[__FNSIZE]; // 30 bytes
	uint32_t samples;
	uint32_t datasize;
	uint32_t loop_start;
	uint32_t loop_end;
	uint32_t mode;
	uint32_t rate;
	uint16_t vol;
	uint16_t pan;
	uint16_t priority;
	uint16_t chs;
	float min_dist;
	float max_dist;
	uint32_t var_freq;
	uint16_t var_vol;
	uint16_t var_pan;
} FENTRY;
FENTRY* FSBs;

int seekval;
int UID2idx(int key)
{
	//return 2;
	/**/switch (key) // wtf
	{
	case 2: // fastgh3_guitar
		return 0;
	case 3: // fastgh3_rhythm
		return 1;
	case 1: // fastgh3_song
		return 2;
	}/**/
}
int sID; // fastgh3_* key, set from preload_song
int sIDx;
float _test;
// fix audio seeking (SOON TM)
static void* FMOD_setPosition_Detour = (void*)0x00D9D12B;
__declspec(naked) void setPosFix()
{
	// weirdly messes up sometimes
	static const uint32_t returnAddress = 0x00D9D13A;
	__asm {
		mov  seekval, eax; // get milliseconds
	}
	//test3 = fopen("ftest.bin", "wb");
	_test = ((float)seekval) / 1000.0f;
	//fwrite(&_test, 4, 1, test3);
	_test /= ((float)spf / 44100.0f);// FSBs[sIDx].rate);
	//fwrite(&_test, 4, 1, test3);
	sIDx = UID2idx(sID);
	seekval = VBR2CBR(VBRfr[sIDx], _test, VBRfrL[sIDx], FSBs[sIDx].rate) * 1000;
	//fwrite(&seekval, 4, 1, test3);
	//fclose(test3);
	__asm {
		mov  eax, seekval;
		push 1;
		push eax;
		mov  eax, [esi + 21Ch];
		push eax;
		call setPosition;
		jmp  returnAddress;
	}
}
// only runs for fastgh3_song in highway scroll func
static void* FMOD_getPosition_Detour = (void*)0x00D932A4;
__declspec(naked) void getPosFix()
{
	static const uint32_t returnAddress = 0x00D932B9;
	__asm {
		mov  seekval, ecx;
	}

	__asm {
		mov  ecx, seekval;
		push 1;
		//lea  ecx, [esp + 4];
		push ecx;
		push eax;
		mov[esp + 10h - 4], 0;
		call getPosition;
		jmp  returnAddress;
	}
}
#endif

char* FSBa;
char* FSBaa;
char* FSBb;
int  FSBm;
FILE* FSBf;
char* FSBc;

// Invo
char SwapByteBits(unsigned char cInput)
{
	unsigned char nResult = 0;
	for (int i = 0; i < 8; i++)
	{
		nResult = nResult << 1;
		nResult |= (cInput & 1);
		cInput = cInput >> 1;
	}
	return (nResult);
}
#if ACCURATETIME
void decfsbpart(char* inp, int len, char* key, int keylen, int keyoff)
{
	if (!key || !keylen || !len)
		return;
	for (int i = 0; i < len; i++)
	{
		*inp = SwapByteBits(*inp ^ key[(i + keyoff) % keylen]);
		inp++;
	}
}
int strlenNZ(char* str)
{
	if (str) return strlen(str);
	return 0;
}
void AlignFile(FILE* f, unsigned char bits)
{
	// bits == 2, align to 4 bytes
	bits = 1 << bits;
	unsigned char align = ftell(f);
	if (align & (bits - 1))
		fseek(f, bits - (align & (bits - 1)), SEEK_CUR);
}
int align(int a, unsigned char bits)
{
	bits = 1 << bits;
	unsigned char align = a;
	if (align & (bits - 1))
		return bits - (align & (bits - 1));
	return 0;
}

#include <vector>
std::vector<WORD> tmpvec;

int iiiii, MP3Cursor;
int fixseeking = 0;
#endif
int realEBX, realECX, realEDX;
// thx zed
static void* FSBLoad_Detour = (void*)0x00548F46;
__declspec(naked) void FSBLoadAllowUnenc() // and fix seeking VBR MP3s
{
	static const uint32_t returnAddress = 0x00548F4D;
	__asm {
		mov  edx, [esp + 24h - 0Ch];
		mov  FSBb, edx;
		mov  realEBX, ebx; // later functions use these registers >:(
		mov  realECX, ecx;
		mov  realEDX, edx;
	}
	FSBa = (char*)malloc(MAX_PATH);
	FSBaa = (char*)malloc(MAX_PATH);
	GetModuleFileNameA(0, FSBa, MAX_PATH);
	FSBc = strrchr(FSBa, '\\');
	if (FSBc)
		*FSBc = 0;
	sprintf_s(FSBaa, MAX_PATH, "%s\\DATA\\%s.fsb.xen", FSBa, FSBb);

	FSBf = fopen(FSBaa, "rb");
	if (FSBf)
	{
		fread(&FSBm, 4, 1, FSBf);
		//fclose(FSBf);

		if (FSBm == 0x33425346)
		{
			realECX = 0;
		}
#if ACCURATETIME
		if (fixseeking)
		{
			decfsbpart((char*)&FSBm, 4, (char*)realECX, strlenNZ((char*)realECX), 4);
			if (FSBm != 0x33425346) goto FAIL;
			//fseek(FSBf, 0, SEEK_SET);
			// file count
			fread(&FSBm, 4, 1, FSBf);
			decfsbpart((char*)&FSBm, 4, (char*)realECX, strlenNZ((char*)realECX), 8);
			if (FSBm != __FC) goto FAIL;
			// struct size
			fread(&FSBm, 4, 1, FSBf);
			decfsbpart((char*)&FSBm, 4, (char*)realECX, strlenNZ((char*)realECX), 12);
			if (FSBm != __SSIZE) goto FAIL;
			fseek(FSBf, 12, SEEK_CUR); // skip extra bytes

			FSBs = (FENTRY*)malloc(__SSIZE);
			fread(FSBs, 1, __SSIZE, FSBf);
			decfsbpart((char*)FSBs, __SSIZE,
				(char*)realECX, strlenNZ((char*)realECX), ftell(FSBf) - 4);
			//test3 = fopen("ftest.bin","wb");
			isVBR = (BYTE*)calloc(3, 1); // just use a bitfield
			VBRfrL = (int*)malloc(3 * sizeof(int));
			VBRfr = (WORD**)calloc(3, sizeof(WORD));
			//test3 = fopen("ftest.bin", "wb");
			for (iiiii = 0; iiiii < 3; iiiii++) // dont read preview.mp3
			{
				MP3Cursor = 0;
				tmpvec.clear();
				while (MP3Cursor < FSBs[iiiii].samples) //FSBs[0].datasize
				{
					// brain drain
					fread(&FSBm, 4, 1, FSBf);
					decfsbpart((char*)&FSBm, 4,
						(char*)realECX, strlenNZ((char*)realECX), ftell(FSBf) - 4);
					//fwrite(&FSBm,4,1,test3);
					// first time i use a std::vector
					FSBm = ESWAP(FSBm);
					tmpvec.push_back(bitrates(MFRAME_GET_BTIDX(FSBm)));
					if (tmpvec.size() > 1 && !isVBR[iiiii])
					{
						if (tmpvec[tmpvec.size() - 2] == tmpvec[tmpvec.size() - 1])
						{
							isVBR[iiiii] = 1;
						}
					}
					FSBm = MFRAME_SIZE(FSBm) + MFRAME_GET_PADBT(FSBm);
					fseek(FSBf, FSBm - 4, SEEK_CUR);
					MP3Cursor += spf;
				}
				AlignFile(FSBf, 4);
				// does this actually work?
				VBRfr[iiiii] = (WORD*)malloc(tmpvec.size() * sizeof(WORD));
				VBRfrL[iiiii] = tmpvec.size();
				memcpy(VBRfr[iiiii], &tmpvec[0], tmpvec.size() * sizeof(WORD));
				//fwrite(VBRfr[iiiii], sizeof(WORD), tmpvec.size(), test3);
				//FSBm = 0xFFFFFFFF;
				//fwrite(&FSBm, 4, 1, test3);
				//fwrite(&FSBm, 4, 1, test3);
				//fwrite(&FSBm, 4, 1, test3);
				//fwrite(&FSBm, 4, 1, test3);
				//aaaa = VBR2CBR(VBRfr[iiiii], 8, VBRfrL[iiiii], 44100);
				//fwrite(&aaaa, 4, 1, test3);
			}
			tmpvec.clear();
			//fclose(test3);
			//exit(0);
		}
#endif

		fclose(FSBf);
	}
FAIL:
	// else assert :/

	__asm {
		push realECX;
		push realEBX;
		push realEDX;
		jmp  returnAddress;
	}
}

void ApplyHack()
{
	GetCurrentDirectoryA(MAX_PATH, inipath);
	strcat_s(inipath, MAX_PATH, "\\settings.ini");
	g_patcher.WriteInt8(CD, GetPrivateProfileIntA("ActionReplay", "CD", 1, inipath));
	vsync = GetPrivateProfileIntA("Misc", "VSync", 1, inipath);
	borderless = GetPrivateProfileIntA("Misc", "Borderless", 0, inipath);
#if (!FRAMERATE_FROM_QB)
	frameRate = GetPrivateProfileIntA("Player", "MaxFPS", 60, inipath);
#endif
	//(*d3ddev)->
	windowed = GetPrivateProfileIntA("Misc", "Windowed", 1, inipath);
	// ^ because why would you want to turn this off
	// unless you like purposely suffering
	g_patcher.WriteInt32((void*)0x0057BB50, windowed);
	if (!vsync)
	{
		g_patcher.WriteNOPs(D3DPPpi, 6);
		g_patcher.WriteInt32(presint, 0x80000000);
	}
	g_patcher.WriteJmp(hWndDetour, hWndHack);
	if (!borderless)
		g_patcher.WriteInt32(wndStyle, WS_SYSMENU | WS_MINIMIZEBOX);
	else
		g_patcher.WriteInt32(wndStyle, WS_POPUP);
#if SCREENSHOT
	g_patcher.WriteJmp(screenshotDetour, ScreenShot);
#endif
	g_patcher.WriteCall(beforeMainloopDetour, initFrameTimer);
	g_patcher.WriteCall(beforePresentDetour, frameLimit);
	g_patcher.WriteCall(afterPresentDetour, lagBegin);
	g_patcher.WriteJmp(Upd2DPSys_detour, velocityFix);
	g_patcher.WriteJmp(Upd2DPSys_detour2, velocityFix2);
	g_patcher.WriteJmp(BossWaitForAttack_framesDetour1, BWFA_frames2Realtime1);
	g_patcher.WriteJmp(BossWaitForAttack_framesDetour2, BWFA_frames2Realtime2);
	g_patcher.WriteJmp(whammyWidth_Detour, whammyWidthFix);
	g_patcher.WriteJmp(FSBLoad_Detour, FSBLoadAllowUnenc);
#if ACCURATETIME
	// experimental
	if (fixseeking = GetPrivateProfileIntA("Misc", "FixSeeking", 0, inipath))
		g_patcher.WriteJmp(FMOD_setPosition_Detour, setPosFix);
#endif
	g_patcher.WriteJmp(profiletimeDetour, ProfileTime);
}
