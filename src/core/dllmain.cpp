// Entry point for GH3+
#include <Windows.h>
#include "core.h"

/*BOOL APIENTRY DllMain(HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )*/
BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		LoadPlugins();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
/*BOOL WINAPI _DllMainCRTStartup(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved)
{
	return DllMain(hModule, ul_reason_for_call, lpReserved);
}*/
