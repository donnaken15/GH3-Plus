
#pragma once

#include "QbStruct.h"
#include "QbArray.h"

using namespace GH3;

typedef int CFunc();
typedef int CFuncI(int);
typedef int CFuncI2(int, int);
typedef int CFuncQ(QbStruct*);
typedef int CFuncQI(QbStruct*, int);
typedef int CFuncQK(QbStruct*, QbKey);
typedef int CFuncES2(QbKey, QbStruct *, int, QbStruct *, int, int, char, char, char, char);
typedef int PrintsubIthinkT(char*, size_t, int);
typedef bool CXT_CreateFromImgFile(char*, size_t, int);

CFuncI2 * WinPortSioGetDevicePress          = (CFuncI2 *)(0x00419A20);
CFuncI2 * WinPortSioGetControlPress         = (CFuncI2 *)(0x00419A50);
CFuncI  * WinPortSioIsDirectInputGamepad    = (CFuncI  *)(0x00419AE0);
CFuncI  * WinPortSioIsKeyboard              = (CFuncI  *)(0x00419B30);
CFuncI  * WinPortSioSetDevice0				= (CFuncI  *)(0x00419B80);
CFuncQ  * LoadPak							= (CFuncQ  *)(0x004A1780);
CFuncQ  * Wait								= (CFuncQ  *)(0x0052EAF0);
CFuncQI * SpawnScriptNow					= (CFuncQI *)(0x00534140);
CFuncQK * ExecuteScriptNoCallback			= (CFuncQK *)(0x00423E80);
CFuncES2 * ExecuteScript2					= (CFuncES2 *)(0x00495DC0);
CXT_CreateFromImgFile * CreateFromImgFile	= (CXT_CreateFromImgFile *)(0x005F7CE0);

typedef int CFuncCreateNote(QbArray *noteArray, int noteIndex, int noteTime, int greenLength, int redLength, int yellowLength, int blueLength, int orangeLength, int hopoFlag, int nextNoteTime);
CFuncCreateNote  * CreateNote = (CFuncCreateNote*)(0x41D41B);

CFuncI2 * ScriptAssert                      = (CFuncI2 *)(0x00532DD0);
PrintsubIthinkT * PrintsubIthink            = (PrintsubIthinkT *)(0x00532A80);
