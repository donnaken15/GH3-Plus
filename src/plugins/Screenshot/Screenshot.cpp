#include "gh3\GH3Keys.h"
#include "gh3\QbStruct.h"
#include "gh3\QbScript.h"
#include "core\Patcher.h"
#include "Screenshot.h"
#include <stdint.h>
#include <stdio.h>
#include <Windows.h>
#include <d3d9.h>
#include <D3dx9tex.h>

#pragma comment(lib, "E:\\D3D9\\d3dx9.lib")
static GH3P::Patcher g_patcher = GH3P::Patcher(__FILE__);
static LPDIRECT3DDEVICE9* d3ddev = (LPDIRECT3DDEVICE9*)0x00C5C7A8;
//#define STBIW_WINDOWS_UTF8
//#define STB_IMAGE_WRITE_IMPLEMENTATION
//#define STBIW_ASSERT(x)
//#include "stb_image_write.h"
using namespace GH3;
static void* screenshotDetour = (void*)0x005377B0;
bool ScreenShot(QbStruct* str, QbScript* scr)
{
	IDirect3DSurface9* backBuf;
	IDirect3DSurface9* surf;
	D3DSURFACE_DESC sd;

	(*d3ddev)->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuf);
	backBuf->GetDesc(&sd);
	(*d3ddev)->CreateOffscreenPlainSurface(
		sd.Width, sd.Height, sd.Format, D3DPOOL_SYSTEMMEM, &surf, NULL);
	//D3DXLoadSurfaceFromSurface(surf,0,0,backBuf,0,0,0,0);
	//(*d3ddev)->GetFrontBufferData(0, surf);
	(*d3ddev)->GetRenderTargetData(backBuf, surf);

	// HOW DOES THIS WORK?!?!?!?!?!?
	//D3DLOCKED_RECT LR;
	//if (surf->LockRect(&LR, 0, D3DLOCK_READONLY))
	//{
	//	backBuf->Release();
	//	surf->Release();
	//	return 0;
	//}
	//CollapseRowPitch((UINT*)LR.pBits, sd.Width, sd.Height, LR.Pitch >> 2);
	//DWORD* rgbx = (DWORD*)malloc((sd.Width * sd.Height) * 4);
	//DWORD i = 0;
	//for (DWORD y = 0; y < sd.Height; y++)
	//{
	//	for (DWORD x = 0; x < sd.Width; x++)
	//	{
	//		rgbx[i++] = 0xFF000000 | x;// *(DWORD*)((byte*)(LR.pBits) + ((4 * y) + (4 * x)));
	//	}
	//	//rgbx = (DWORD*)(((byte*)rgbx) + 1);
	//}

	wchar_t maindir[MAX_PATH];
	wchar_t filepath[MAX_PATH];
	wchar_t* filename;
	char* filenameA;
	if (!str->GetWString(QbKey("filename"), filename))
		filename = L"screen";
	if (!str->GetString(QbKey("filename"), filenameA))
	{
		filenameA = "screen";
	}
	else
	{
		int alen = strlen(filenameA);
		filename = (wchar_t*)calloc(++alen, 2);
		mbstowcs(filename, filenameA, alen);
	}
	GetCurrentDirectoryW(MAX_PATH, maindir);
	swprintf_s(filepath, L"%s\\%s.bmp", maindir, filename);
	// WHY DO PNG AND JPG FAIL
	// DDS USES ARGB8 AND IS THE SAME SIZE AS BITMAP >:(
	// AND WHY IS IT SO BIG FOR WRITING AN UNCOMPRESSED
	// RGB32 STUPIDLY LARGE SCREENSHOT

	//int q;
	//if (!str->GetInt(QbKey("quality"), q))
	//	q = 85;
	//int utf8_ln = lstrlenW(filename) * 4;
	//char*utf8_fn = (char*)malloc(utf8_ln);
	//stbiw_convert_wchar_to_utf8(utf8_fn,utf8_ln,filename);
	//stbi_write_jpg("test.png", sd.Width, sd.Height, 4, rgbx, q);

	//surf->UnlockRect();
	//free(rgbx);
	D3DXSaveSurfaceToFileW(filepath, D3DXIFF_BMP, backBuf, NULL, NULL);

	backBuf->Release();
	surf->Release();
	return 1;
}

void ApplyHack()
{
	g_patcher.WriteJmp(screenshotDetour, ScreenShot);
}