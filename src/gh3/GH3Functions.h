
#pragma once

#include "QbStruct.h"
#include "QbArray.h"

using namespace GH3;

typedef int CFunc();
typedef int CFuncI(int);
typedef int CFuncI2(int, int);
typedef int CFuncQ(QbStruct*);
typedef int PrintsubIthinkT(char*, size_t, int);

CFuncI2 * WinPortSioGetDevicePress          = (CFuncI2 *)(0x00419A20);
CFuncI2 * WinPortSioGetControlPress         = (CFuncI2 *)(0x00419A50);
CFuncI  * WinPortSioIsDirectInputGamepad    = (CFuncI  *)(0x00419AE0);
CFuncI  * WinPortSioIsKeyboard              = (CFuncI  *)(0x00419B30);
CFuncI  * WinPortSioSetDevice0				= (CFuncI  *)(0x00419B80);
CFuncQ  * LoadPak							= (CFuncQ  *)(0x004A1780);

typedef int CFuncCreateNote(QbArray *noteArray, int noteIndex, int noteTime, int greenLength, int redLength, int yellowLength, int blueLength, int orangeLength, int hopoFlag, int nextNoteTime);
CFuncCreateNote  * CreateNote = (CFuncCreateNote*)(0x41D41B);

CFuncI2 * ScriptAssert                      = (CFuncI2 *)(0x00532DD0);
PrintsubIthinkT * PrintsubIthink            = (PrintsubIthinkT *)(0x00532A80);
