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

BYTE vsync, borderless;

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
			// figure this out so its just dependent on GSM // i tried everything here...
		}
		SetWindowPos(*hWnd, 0,
			centerX, centerY,
			sizeX, sizeY,
			SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOACTIVATE);
		__asm {
			jmp returnAddress;
		}
	}
	// lazy to find and write another one time detour

}

using namespace GH3;
static void* screenshotDetour = (void*)0x005377B0;
bool ScreenShot(QbStruct*str,QbScript*scr)
{
	// turned off because of D3DXSaveSurfaceToFile bloating the DLL by 334KB
	// "just make it a separate plugin"
#if 0
	IDirect3DSurface9* pSurface; // do i really need two surfaces???
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

	D3DXSaveSurfaceToFileA(filepath, D3DXIFF_BMP, surf, NULL, NULL);
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
dl LAG_0, LAG_1;
dl CLOCK_FREQ;
float frameRate = 60.0f;

#define FRAMERATE_FROM_QB 1

#include <gh3\GH3Functions.h>

static void* beforeMainloopDetour = (void*)0x00401FFD;
void initFrameTimer()
{
	QueryPerformanceFrequency((LARGE_INTEGER*)&CLOCK_FREQ);
	frameBase = -((float)(CLOCK_FREQ) / frameRate);
	frameLimiter = CreateWaitableTimer(NULL, 1, NULL);
}
static void* beforePresentDetour = (void*)0x0048453B;
void frameLimit()
{
#if (FRAMERATE_FROM_QB)
	float frameRate = GlobalMapGetInt(QbKey("fps_max")); // can't get a float >:(
#endif
	if (!frameRate)
	{
		return;
	}
	timeBeginPeriod(1);
	QueryPerformanceCounter((LARGE_INTEGER*)&frameTime);
	frameBase = -((float)(CLOCK_FREQ) / frameRate);
	frameTime += frameBase - LAG_1;
	SetWaitableTimer(frameLimiter, (LARGE_INTEGER*)&frameTime, 0, NULL, NULL, 0);
	WaitForSingleObject(frameLimiter, INFINITE);
	QueryPerformanceCounter((LARGE_INTEGER*)&LAG_1);
	timeEndPeriod(1);
	// still unstable around 120 FPS :toolcoder:
}

static char inipath[MAX_PATH];
static char test[10];
static int*presint = (int*)0x00C5B934;
static char*CD = (char*)0x00B45A11;

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
	if (!vsync)
	{
		g_patcher.WriteNOPs(D3DPPpi, 6);
		g_patcher.WriteInt32(presint, 0x80000000);
	}
	g_patcher.WriteJmp(hWndDetour, hWndHack);
	if (!borderless)
	{
		g_patcher.WriteInt32(wndStyle, WS_SYSMENU | WS_MINIMIZEBOX);
	}
	g_patcher.WriteInt32(wndStyle, WS_SYSMENU | WS_MINIMIZEBOX);
	g_patcher.WriteJmp(screenshotDetour, ScreenShot);
	g_patcher.WriteCall(beforeMainloopDetour, initFrameTimer);
	g_patcher.WriteCall(beforePresentDetour, frameLimit);
}