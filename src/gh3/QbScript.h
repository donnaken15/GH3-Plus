#pragma once

#include "core\GH3Plus.h"
#include "QbStruct.h"
#include "QbKey.h"
#include <stdint.h>
#include <inttypes.h>

typedef unsigned char byte;
namespace GH3
{
	struct GH3P_API // Begin-repeat loop structure.    
		SLoop
	{
		// Pointer to the start of the loop, which is the
		// token following the Begin.
		byte*mpStart;

		// Pointer to the end of the loop, ie the next instruction after the loop.
		byte*mpEnd;

		bool mGotCount;			// Whether it is a finite loop with a count value.
		bool mNeedToReadCount;	// Whether the count value following the repeat needs to be read.
		int mCount;				// Counts down to zero, and skips past the repeat once it reaches zero.
	};

	// Values for CScript::m_wait_type
	enum EWaitType
	{
		WAIT_TYPE_NONE = 0,
		WAIT_TYPE_COUNTER,
		WAIT_TYPE_TIMER,
		WAIT_TYPE_BLOCKED,
		WAIT_TYPE_OBJECT_MOVE,
		WAIT_TYPE_OBJECT_ANIM_FINISHED,
		WAIT_TYPE_OBJECT_JUMP_FINISHED,
		WAIT_TYPE_OBJECT_STOP_FINISHED,
		WAIT_TYPE_OBJECT_ROTATE,
		WAIT_TYPE_STREAM_FINISHED,
		WAIT_TYPE_ONE_PER_FRAME,
	};

	enum ESingleStepMode
	{
		OFF = 0,
		WAITING,
		STEP_INTO,
		STEP_OVER,
	};

	typedef unsigned int Time;

	// Saves the original location & other info when calling a subroutine.
	struct SReturnAddress
	{
		QbKey mScriptNameChecksum;
		QbStruct*mpParams;
		byte*mpReturnAddress;
		uint32_t*mpObject;
		SLoop* mpLoop;
		EWaitType mWaitType;
		uint32_t*mpWaitComponent;
		int mWaitTimer;
		Time mStartTime;
		Time mWaitPeriod;
		bool mInterrupted;
		unsigned int unk1, unk2;
	};

	// dev comments
// The maximum level of Begin-Repeat nesting.
#define MAX_NESTED_BEGIN_REPEATS 10
#define NESTED_BEGIN_REPEATS_SMALL_BUFFER_SIZE 1

// The maximum number of nested calls to other scripts.                              
#define MAX_RETURN_ADDRESSES 16
// The number of SReturnAddress structures in the small buffer in each CScript
// Mostly 2 is all that is needed, so a buffer of MAX_RETURN_ADDRESSES is only allocated when needed.
#define RETURN_ADDRESSES_SMALL_BUFFER_SIZE 2

	// Return values from CScript::Update()
	enum EScriptReturnVal
	{
		ESCRIPTRETURNVAL_FINISHED,
		ESCRIPTRETURNVAL_BLOCKED,
		ESCRIPTRETURNVAL_WAITING,
		ESCRIPTRETURNVAL_ERROR,
		ESCRIPTRETURNVAL_STOPPED_IN_DEBUGGER,
		ESCRIPTRETURNVAL_FINISHED_INTERRUPT,
	};

	/// Neversoft's script class. Most of the contents of the script class are unknown. Also known as CScript
	struct GH3P_API QbScript
	{
		uint32_t gap;  // always 0 most likely
		QbScript*next;
		QbScript*prev;
		uint32_t unkC;
		uint8_t*instructionPointer; //or mp_pc
		QbStruct*qbStruct14; // lines up with class items:
							// mp_function_params and mp_params
		QbStruct*qbStruct18;
		QbStruct*qbStruct1C;
		// Begin-Repeat loop stuff.
		// NULL if not in a loop, otherwise points into the following array.
		SLoop*currentLoop;
		// If the number of loops needs to be bigger than NESTED_BEGIN_REPEATS_SMALL_BUFFER_SIZE then
		// mp_loops will point to a dynamically allocated array of MAX_NESTED_BEGIN_REPEATS SLoop structures,
		// otherwise mp_loops will equal mp_loops_small_buffer
		SLoop mp_loops_small_buffer[NESTED_BEGIN_REPEATS_SMALL_BUFFER_SIZE];
		SLoop*mp_loops;
		uint32_t scriptDepth; // stack size / call depth
		SReturnAddress mp_return_addresses_small_buffer[RETURN_ADDRESSES_SMALL_BUFFER_SIZE];
		// If the num return addresses needs to be bigger than RETURN_ADDRESSES_SMALL_BUFFER_SIZE then
		// mp_return_addresses will point to a dynamically allocated array of MAX_RETURN_ADDRESSES SReturnAddress structures,
		// otherwise mp_return_addresses will equal mp_return_addresses_small_buffer
		SReturnAddress*mp_return_addresses;
		uint32_t unkStructPtrA8; // always 0 maybe
		uint32_t dwordAC;
		uint32_t qbState; // unkB0
		uint32_t dwordB4; // always 0 maybe
		uint32_t scriptIndex;
		uint8_t unkBC; // checked in QbStruct::Update for game frame related
		uint8_t flags2;
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
		uint32_t*mpObject; // ?? // points to stuff related to script
		uint32_t dwordD0;
		uint32_t dwordD4; // becomes a pointer at one POINT
		QbKey type;
		uint32_t flags;
	};

	///used for struct items?
	static void (__cdecl * const SpawnScript)(QbStruct *aStruct, QbScript *parentScript) = reinterpret_cast<void (__cdecl *)(QbStruct *, QbScript *)>(0x00534100);
	static void (__cdecl * const SpawnScriptNow)(QbStruct *aStruct, QbScript *parentScript) = reinterpret_cast<void(__cdecl *)(QbStruct *, QbScript *)>(0x00534140);
	static void (__cdecl * const SpawnScriptLater)(QbStruct *aStruct, QbScript *parentScript) = reinterpret_cast<void(__cdecl *)(QbStruct *, QbScript *)>(0x00534160);
}

