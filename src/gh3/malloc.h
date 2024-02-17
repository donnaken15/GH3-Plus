#pragma once

#include "gh3\QbStruct.h"

namespace GH3
{
	///used for struct items?
	static void * (__cdecl *qbItemMalloc)(size_t size, int unused) = reinterpret_cast<void * (__cdecl *)(size_t size, int unused)>(0x00437410); 

	///Used for qbstructs
	static void * (__cdecl *qbMalloc)(size_t size, int unused) = reinterpret_cast<void * (__cdecl *)(size_t size, int unused)>(0x00402370);

	// "specialMalloc" used sometimes for array allocation
	static void * (__cdecl *qbArrMalloc)(size_t size, int unused) = reinterpret_cast<void * (__cdecl *)(size_t size, int unused)>(0x004373A0);

	static void * (__cdecl *qbFree)(QbStruct*_this) = reinterpret_cast<void * (__cdecl *)(QbStruct*_this)>(0x004023E0);
}