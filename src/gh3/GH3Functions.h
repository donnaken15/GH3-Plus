
#pragma once

#include "QbStruct.h"
#include "QbScript.h"
#include "QbArray.h"
#include <Windows.h>

using namespace GH3;

// needs cleanup i think

typedef int CFunc(QbStruct* str, QbScript* scr);
typedef int CFuncI(int);
typedef int CFuncF(float);
typedef int CFuncI2(int, int);
typedef int CFuncQ(QbStruct*);
typedef int CFuncQI(QbStruct*, int);
typedef int CFuncQK(QbStruct*, QbKey);
typedef int CFuncES2(QbKey scriptKey, QbStruct *params, QbKey callbackScript, QbStruct *callbackParams, int node, int id, char net, char permanent, char nonsessspecific, char pausewobj);
typedef int PrintsubIthinkT(char*, size_t, int);
typedef bool CXT_CreateFromImgFile(char*, size_t, int);
typedef QbArray*CFuncQAI(int);
typedef QbStruct*CFuncQSI(int);
typedef DWORD*CFuncLPDLPDI(DWORD*, int);
typedef int*CFuncLPI(int);

CFuncI2 * WinPortSioGetDevicePress          = (CFuncI2 *)(0x00419A20);
CFuncI2 * WinPortSioGetControlPress         = (CFuncI2 *)(0x00419A50);
CFuncI  * WinPortSioIsDirectInputGamepad    = (CFuncI  *)(0x00419AE0);
CFuncI  * WinPortSioIsKeyboard              = (CFuncI  *)(0x00419B30);
CFuncI  * WinPortSioSetDevice0				= (CFuncI  *)(0x00419B80);
CFuncQ  * LoadPak							= (CFuncQ  *)(0x004A1780);
CFuncQ  * Wait								= (CFuncQ  *)(0x0052EAF0);
CFuncQI * SpawnScriptNow					= (CFuncQI *)(0x00534140);
CFuncQK * ExecuteScriptNoCallback			= (CFuncQK *)(0x00423E80);
CFuncES2 * ExecuteScript2					= (CFuncES2*)(0x00495DC0);
CXT_CreateFromImgFile * CreateFromImgFile	= (CXT_CreateFromImgFile *)(0x005F7CE0);
CFuncI  * GlobalMapGetInt					= (CFuncI  *)(0x004A5960);
CFuncF  * GlobalMapGetFloat					= (CFuncF  *)(0x004A5960);
CFuncQAI* GlobalMapGetQbArray				= (CFuncQAI*)(0x004A5E00);
CFuncQSI* GlobalMapGetQbStruct				= (CFuncQSI*)(0x004A5D70);
CFuncLPDLPDI* GetKeyValue					= (CFuncLPDLPDI*)(0x004A5A00); // WHAT IS THIS
CFuncI  * GlobalMapGet						= (CFuncI  *)(0x004A5C70);


// WHAT IS THIS
typedef int CFuncCreateNote(QbArray *noteArray, int noteIndex, int noteTime, int greenLength, int redLength, int yellowLength, int blueLength, int orangeLength, int hopoFlag, int nextNoteTime);
CFuncCreateNote  * CreateNote = (CFuncCreateNote*)(0x41D41B);

CFuncI2 * ScriptAssert                      = (CFuncI2 *)(0x00532DD0);
PrintsubIthinkT * PrintsubIthink            = (PrintsubIthinkT *)(0x00532A80);
