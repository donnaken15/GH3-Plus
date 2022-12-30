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

#pragma comment(lib, "E:\\D3D9\\d3dx9.lib")
#pragma comment(lib, "winmm.lib")
static GH3P::Patcher g_patcher = GH3P::Patcher(__FILE__);

static void *D3DPPpi = (void *)0x0057BB79;
static LPDIRECT3DDEVICE9*d3ddev = (LPDIRECT3DDEVICE9*)0x00C5C7A8;

HWND*hWnd;
RECT WndR;
int sizeX, sizeY, centerX, centerY;

BYTE vsync, borderless, windowed;

static void *hWndDetour = (void *)0x0057BA6F;
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

int* GameRes_Xi = (int*)0x00C5E6B8;
int* GameRes_Yi = (int*)0x00C5E6BC;
float* GameRes_X = (float*)0x00C5E6B0;
using namespace GH3;
static void*screenshotDetour = (void*)0x005377B0;
bool ScreenShot(QbStruct*str,QbScript*scr)
{
	// turned off because of D3DXSaveSurfaceToFile bloating the DLL by 334KB
	// "just make it a separate plugin"
	// also UPX
#if SCREENSHOT
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
	char*filename;
	if (!str->GetString(QbKey("filename"), filename))
		filename = "screen";
	GetCurrentDirectoryA(MAX_PATH, maindir);
	sprintf_s(filepath,"%s\\%s.bmp",maindir,filename);
	// WHY DO PNG AND JPG FAIL
	// DDS USES ARGB8 AND IS THE SAME SIZE AS BITMAP >:(
	// AND WHY IS IT SO BIG FOR WRITING AN UNCOMPRESSED
	// RGB32 STUPIDLY LARGE SCREENSHOT
	
	D3DXSaveSurfaceToFileA(filepath, D3DXIFF_BMP, pSurface, NULL, NULL);

	pSurface->Release();
	surf->Release();
#endif
	return 1;
}

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

// doesn't work?
extern "C" NTSYSAPI NTSTATUS NTAPI NtSetTimerResolution(ULONG DesiredResolution, BOOLEAN SetResolution, PULONG CurrentResolution);

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
	if (killswitchTimer++ < (frameRate*2))
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
		WaitForSingleObject(frameLimiter, (1.0f/frameRate)*1000);
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
static int*presint = (int*)0x00C5B934;
static char*CD = (char*)0x00B45A11;

// patch framerate fixed velocity and friction of particles
float frameFrac = 60.0f;
float*g_gameSpeed1 = (float*)0x009596B4;
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
		mov     eax , dword ptr g_gameSpeed1; // also apply fix for slomo
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
		mov     eax , dword ptr g_gameSpeed1;
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
		movss   [esp + 58h], xmm0;

		jmp returnAddress;
	}
}

// fix audio seeking (SOON TM)
static void* FMOD_setPosition_Detour = (void*)0x00000000;
__declspec(naked) void setPosFix()
{
	static const uint32_t returnAddress = 0x00000000;
	__asm {
		jmp returnAddress;
	}
}
static void* FMOD_getPosition_Detour = (void*)0x00000000;
__declspec(naked) void getPosFix()
{
	static const uint32_t returnAddress = 0x00000000;
	__asm {
		jmp returnAddress;
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
	g_patcher.WriteJmp(screenshotDetour, ScreenShot);
	g_patcher.WriteCall(beforeMainloopDetour, initFrameTimer);
	g_patcher.WriteCall(beforePresentDetour, frameLimit);
	g_patcher.WriteCall(afterPresentDetour, lagBegin);
	g_patcher.WriteJmp(Upd2DPSys_detour, velocityFix);
	g_patcher.WriteJmp(Upd2DPSys_detour2, velocityFix2);
	g_patcher.WriteJmp(BossWaitForAttack_framesDetour1, BWFA_frames2Realtime1);
	g_patcher.WriteJmp(BossWaitForAttack_framesDetour2, BWFA_frames2Realtime2);
	g_patcher.WriteJmp(whammyWidth_Detour, whammyWidthFix);
}