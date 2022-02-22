#pragma once

#include "core\GH3Plus.h"
#include "QbStruct.h"
#include "QbKey.h"
#include <stdint.h>

namespace GH3
{
    /// Neversoft's script class. Most of the contents of the script class are unknown. Also known as CScript
	struct GH3P_API QbScript
	{
		uint32_t gap;  // always 0 maybe
		QbStruct *qbStruct4;
		uint32_t unk8;
		uint32_t unkC;
		uint8_t *instructionPointer;
		QbStruct *qbStruct14;
		uint32_t dword18; // near IP, moves with IP, cursor for current op?
		QbStruct *qbStruct1C;
		uint32_t unk20;
		uint32_t unk24; // near IP
		uint32_t unk28; // near IP
		uint32_t unk2C; // always 0 maybe
		uint32_t unk30;
		uint32_t unk34;
		uint32_t unk38; // stack size / call depth
		uint32_t unk3C; // root script ? consistently =s guitar_startup during init
		QbStruct *qbStruct40;
		uint32_t unk44;
		uint32_t unk48; // always 0 maybe
		uint32_t unk4C; //
		uint32_t unk50; //
		uint32_t unk54; //
		uint32_t unk58; //
		uint32_t unk5C; //
		uint32_t unk60; //
		uint32_t unk64; //
		uint32_t unk68; //
		uint32_t unk6C; // always 0 maybe
		uint32_t unk70; // parent script ?
		QbStruct *qbStruct74;
		uint32_t unk78;
		uint32_t unk7C; // always 0 maybe
		uint32_t unk80; //
		uint32_t unk84; //
		uint32_t unk88; //
		uint32_t unk8C; //
		uint32_t unk90; //
		uint32_t unk94; //
		uint32_t unk98; //
		uint32_t unk9C; ////
		uint32_t dwordA0; // always 0 maybe
		uint32_t dwordA4;
		uint32_t unkStructPtrA8; // always 0 maybe
		uint32_t dwordAC;
		uint32_t unkB0;
		uint32_t dwordB4; // always 0 maybe
		uint32_t dwordB8;
		uint8_t unkBC;
		uint8_t unkBD;
		uint8_t unkBE;
		uint8_t unkBF;
		uint8_t *nextIP; // sometimes is 0, why
		uint8_t unkC4; // always 0 maybe, all the way down
		uint8_t unkC5;
		uint8_t unkC6;
		uint8_t unkC7;
		uint32_t dwordC8;
		uint32_t dwordCC;
		uint32_t dwordD0;
		uint32_t dwordD4;
		QbKey type;
	};

	///used for struct items?
	static void (__cdecl * const SpawnScript)(QbStruct *aStruct, QbScript *parentScript) = reinterpret_cast<void (__cdecl *)(QbStruct *, QbScript *)>(0x00534100);
	static void (__cdecl * const SpawnScriptNow)(QbStruct *aStruct, QbScript *parentScript) = reinterpret_cast<void(__cdecl *)(QbStruct *, QbScript *)>(0x00534140);
	static void (__cdecl * const SpawnScriptLater)(QbStruct *aStruct, QbScript *parentScript) = reinterpret_cast<void(__cdecl *)(QbStruct *, QbScript *)>(0x00534160);
}

