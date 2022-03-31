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
		uint32_t gap;  // always 0 most likely
		QbStruct *qbStruct4;
		QbScript *qbScript8; // cheat engine shows like it's a script
		uint32_t unkC;
		uint8_t *instructionPointer;
		QbStruct *qbStruct14; // lines up with class items:
							// mp_function_params and mp_params
		//uint32_t dword18; // near IP, moves with IP, cursor for current op?
							// IDA shows it has a similar structure to QbStruct
		QbStruct *qbStruct18;
		QbStruct *qbStruct1C;
		uint32_t unk20; // sometimes links to this struct's values @ 0x24
		uint32_t unk24; // near IP // points to stuff related to script
		uint32_t unk28; // near IP // SLoop class here?
		uint32_t unk2C; // points to stuff related to script
		uint32_t unk30; // used for repeat statement it looks like (decrements)
		uint32_t unk34;
		uint32_t scriptDepth; // stack size / call depth
		uint32_t rootScript; // consistently =s guitar_startup during init
		QbStruct *qbStruct40; // root script's struct it looks like
								// init values persist after guitar_startup
		uint8_t *scriptBegin; // pointer to start of script possibly // or not // as expected
		uint32_t unk48; // points to stuff related to script
		uint32_t unk4C; // sometimes links to value 0x24
		uint32_t unk50; // always 0 maybe
		uint32_t unk54; //
		uint32_t unk58; //
		uint32_t unk5C; //
		uint32_t unk60; //
		uint32_t unk64; //
		uint32_t unk68; // always 0 maybe
		uint32_t unk6C; // points to code it looks like
		uint32_t someKey70; // parent script ?
		QbStruct *qbStruct74;
		uint32_t unk78; // looks like uncompressed script
						// checked on the script ProcessorGroup_RegisterDefault
						// and it lines up
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
		uint32_t dwordA4; // links to rootScript?
						// in an instance, it points to
						// guitar_startup, a struct,
						// and a pointer to code with
						// key setup_sprites
		uint32_t unkStructPtrA8; // always 0 maybe
		uint32_t dwordAC;
		uint32_t unkB0;
		uint32_t dwordB4; // always 0 maybe
		uint32_t dwordB8;
		uint8_t unkBC; // checked in QbStruct::Update for game frame related
		uint8_t unkBD;
		uint8_t unkBE;
		uint8_t unkBF;
		uint8_t *nextIP; // sometimes is 0, why
						// when it's not, it's just the same as the original IP
						// also changes one of the bytes before this to be 0x02
						// sometimes 0x06
		uint8_t unkC4; // always 0 maybe
		uint8_t unkC5;
		uint8_t unkC6;
		uint8_t unkC7;
		uint32_t node; // "Number of the node that caused this script to be spawned, -1 if none specific"
		uint32_t dwordCC; // points to stuff related to script
		uint32_t dwordD0;
		uint32_t dwordD4; // becomes a pointer at one POINT
		QbKey type;
	};

	///used for struct items?
	static void (__cdecl * const SpawnScript)(QbStruct *aStruct, QbScript *parentScript) = reinterpret_cast<void (__cdecl *)(QbStruct *, QbScript *)>(0x00534100);
	static void (__cdecl * const SpawnScriptNow)(QbStruct *aStruct, QbScript *parentScript) = reinterpret_cast<void(__cdecl *)(QbStruct *, QbScript *)>(0x00534140);
	static void (__cdecl * const SpawnScriptLater)(QbStruct *aStruct, QbScript *parentScript) = reinterpret_cast<void(__cdecl *)(QbStruct *, QbScript *)>(0x00534160);
}

