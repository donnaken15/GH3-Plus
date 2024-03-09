#include "gh3\GH3Keys.h"
#include "gh3\GlobalMap.h"
#include "gh3\QbValueType.h"
#include "core\Patcher.h"
#include "Logger.h"
#include <stdint.h>
#include <stdio.h>
#include <Windows.h>
#include "gh3\GH3Functions.h"


#include "gh3\QbScript.h"
#include <corecrt_share.h>

/// code by wesley and not zedek
///
///   #################################
/// ##         ## # ####### ## # ######
///             ## # ######  ## # #### 
///              ## # ####    #######  
///               #######              
///
/// https://donnaken15.com/
/// https://youtube.com/donnaken15
/// https://github.com/donnaken15

using namespace GH3;

BYTE l_CreateCon, l_WriteFile, l_FixKeys,
	l_PrintStruct, l_AllSuccess = 1,
	l_CreateConFail = 0, l_DbgLoaded = 0,
	l_FmtTxtLkup, l_ParseFormatting = 0,
	l_WarnAsserts = 0;
FILE * log, * CON;
HANDLE CONh;

static GH3P::Patcher g_patcher = GH3P::Patcher(__FILE__);

#define MAX_KEYS 50000
// found that max amount of keys from dbg.pak deduped is 38.5k
#define KEYSTRLEN 128
UINT *dbgKeys;
LPSTR dbgStr;
UINT *dbgKeysCache;
char*keytmp;
UINT*keytmp2;
char keyfault[20] = "Unknown(0xXXXXXXXX)";
FILE*dbgpak;

char*FML(UINT i) // why wont this work in specific uses
{
	sprintf(keyfault + 10, "%08x", i);
	keyfault[18] = ')';
	return keyfault;
}

char*BLANK = "";
static void *UnkKeyDetour = (void *)0x004A69A0;//0x004A69D5; <--- 5 IQ
__declspec(naked) char* QbKeyPrintFix(UINT qbk)
{
	static const uint32_t returnAddress = 0x004A6A10;
	__asm {
		lea eax, [esp + 4];
		mov[keytmp2], eax;
	}
	FML(*keytmp2);
	keytmp = keyfault;
	if (!l_DbgLoaded)
	{
		sprintf(keyfault, "%08X", qbk);
		goto fail;
	}
	for (uint16_t i = 0; i < MAX_KEYS; i++)
	{
		if (*keytmp2 == dbgKeys[i])
		{
			keytmp = (dbgStr + (i*KEYSTRLEN));
			break;
		}
		if (*keytmp2 == 0)
		{
			break;
		}
	}
	fail:
	__asm {
		mov eax, keytmp;
		jmp returnAddress;
	}
}

void print(const char*str)
{
	if (l_CreateCon) {
		fputs(str, CON);
	}
	if (l_WriteFile) {
		fputs(str, log);
		fflush(log); // how can i do this only when the program crashes
	}
}

static void *ScrAsrtDetour = (void *)0x00532DFE;
__declspec(naked) void RedirectOutput()
{
	static const uint32_t returnAddress = 0x00532E03;
	static const char*logtext;
	__asm {
		mov logtext, edx;
		call PrintsubIthink;
	}
	print(logtext);
	__asm {
		jmp returnAddress;
	}
}
__declspec(naked) void RedirectOutput2()
{
	static const uint32_t returnAddress = 0x00532E2F;
	static const char*logtext;
	__asm {
		mov logtext, edx;
		call PrintsubIthink;
	}
	print(logtext);
	__asm {
		jmp returnAddress;
	}
}

typedef struct {
	void* rangeStart;
	void* rangeEnd; // on ret
	char* funcName;
} _ASSERT_INFO_FUNC;
#define AIF(S,E,N) { (void*)S,(void*)E,N }
_ASSERT_INFO_FUNC assert_functions[] = {
	AIF(0x00404D60,0x00404DE9,0),
	AIF(0x00404DF0,0x00404E2F,0),
	AIF(0x004075B0,0x00407747,"SendStructure"),
	AIF(0x004100B0,0x00410122,"InitCutsceneInfo"),
	AIF(0x00410800,0x004108B5,"CProgressionSave__method_410800"),
	AIF(0x00411290,0x004112E3,"SetLeakCheck"),
	AIF(0x00412860,0x004128B4,"ToggleLOD"),
	AIF(0x00412FC0,0x00413393,"CreateParticleSystem"),
	AIF(0x00419A50,0x00419ADE,"WinPortSioGetControlPress"),
	AIF(0x00419AE0,0x00419B2F,"WinPortSioIsDirectInputGamepad"),
	AIF(0x00419B30,0x00419B7E,"WinPortSioIsKeyboard"),
	AIF(0x00419B80,0x00419BD3,"WinPortSioSetDevice0"),
	AIF(0x00419BE0,0x00419C8F,"WinPortSioGetControlName"),
	AIF(0x00419C90,0x00419D17,"WinPortSioGetControlBinding"),
	AIF(0x00419D20,0x00419DC4,"WinPortSioSetControlBinding"),
	AIF(0x00532DD0,0x00532E10,"ScriptAssert"),
	AIF(0x0053FD30,0x0053FD7F,"LoadMovieIntoBuffer"),
	AIF(0x0053FD80,0x0053FE05,"PlayMovieFromBuffer"),
	AIF(0x00478E50,0x004790FD,"QbStruct::GetValue"),
	AIF(0x004789F0,0x00478A34,"QbStruct::GetInt"),
};
DWORD textSegs[] = {
	0x00401000, 0x00898000, // .text
	0x00D7A000, 0x00DB0000, // .HATRED
};
char*getAssertFunctionRange(void*addr)
{
	for (int i = 0; i < sizeof(assert_functions) / sizeof(_ASSERT_INFO_FUNC); i++)
	{
		_ASSERT_INFO_FUNC*current = &assert_functions[i];
		if (addr > current->rangeStart && addr < current->rangeEnd)
		{
			if (current->funcName)
				return current->funcName;
			else
			{
				char*funcName = (char*)malloc(64);
				sprintf_s(funcName, 64, "sub_%X", current->rangeStart);
				current->funcName = funcName;
				return funcName;
			}
		}
	}
	return "unknown sub";
}

void printStructBase(QbStruct* qs);

static void* LogErrDfnctDetour = (void*)0x004F6500;
void assertWarn()
{
	if (!l_WarnAsserts)
		return;
	__asm mov eax, [esp+12]; // +12 from function setting up stack or whatever
	int EIP;
	__asm mov ecx, [esp+12+8]; // get extra arguments (qbstruct)
	// I FORGOT I WAS JUST JMPING ON A NULLSUB
	// SO I COULD'VE SET PARAMETERS HERE
	// but i dont know if when i add those, ESP will move again
	QbStruct*a;
	__asm mov a, ecx;
	__asm mov EIP, eax;
	EIP -= 5;
	for (int i = 0; i < sizeof(textSegs) / sizeof(DWORD) >> 1; i++)
	{
		/*if (l_CreateCon) {
			fprintf(CON, "%p > %p\n", EIP, textSegs[(i << 1)]);
			fprintf(CON, "%p < %p\n", EIP, textSegs[(i << 1) + 1]);
		}*/
		if (EIP > textSegs[(i << 1)] &&
			EIP < textSegs[(i << 1) + 1])
			break;
		else
		{
			print("Assert execution address is not in code range !!!!!!!!!!!!!!!\n");
			return;
		}
	}
	char*funcName = getAssertFunctionRange((void*)EIP);
	if (l_CreateCon) {
		fprintf(CON, "ASSERT in %s @ %p\n", funcName, EIP);
		// just use vprintf instead of putting this twice
		// already did for qcc so why not here
	}
	if (l_WriteFile) {
		fprintf(log, "ASSERT in %s @ %p\n", funcName, EIP);
	}
	printStructBase(a);
}

int qbstrindent = 0;

// HOW ABOUT RIP THE FUNCTION FROM THUG1
static void* PrintScriptInfoDetour = (void*)0x005309A0;
void PrintScriptInfo(QbStruct*params, QbScript*_this)
{
	if (l_CreateCon) {
		fprintf(CON, "Printing script %s:\n{", QbKeyPrintFix(_this->type));
	}
	if (l_WriteFile) {
		fprintf(log, "Printing script %s:\n{", QbKeyPrintFix(_this->type));
	}
	qbstrindent++;
	//print("\n    "); // grease for half optimization
	//print("0x04 some struct: {\n");
	//printStructBase(_this->qbStruct4);
	//print("    }");
	print("\n    ");
	if (l_CreateCon) {
		fprintf(CON, "0x10 EIP: %04X", _this->instructionPointer);
	}
	if (l_WriteFile) {
		fprintf(log, "0x10 EIP: %04X", _this->instructionPointer);
	}
	print("\n    ");
	print("0x14 mp_function_params: {\n");
	printStructBase(_this->qbStruct14);
	print("    }");
	print("\n    ");
	print("0x18 Arguments: {\n");
	printStructBase(_this->qbStruct18);
	print("    }");
	print("\n    ");
	print("0x1C Variables: {\n");
	printStructBase(_this->qbStruct1C);
	print("    }");
	print("\n    ");
	if (l_CreateCon) {
		fprintf(CON, "0x38 Call depth: %u", _this->scriptDepth);
	}
	if (l_WriteFile) {
		fprintf(log, "0x38 Call depth: %u", _this->scriptDepth);
	}
	print("\n    ");
	if (l_CreateCon) {
		fprintf(CON, "0x3C Callstack:");
	}
	if (l_WriteFile) {
		fprintf(log, "0x3C Callstack:");
	}
	for (int i = _this->scriptDepth; i > 0; i--)
	{
		print("\n        ");
		auto RA = _this->mp_return_addresses[i];
		if (l_CreateCon) {
			fprintf(CON, "%s {\n", QbKeyPrintFix(RA.mScriptNameChecksum));
		}
		if (l_WriteFile) {
			fprintf(log, "%s {\n", QbKeyPrintFix(RA.mScriptNameChecksum));
		}
		qbstrindent++;
		printStructBase(RA.mpParams);
		qbstrindent--;
		print("        }");
		// this is an absolute mess
	}
	print("\n}\n");
	qbstrindent--;
}

__declspec(naked) void MyScriptAssert()
{
	static const uint32_t returnAddress = 0x00532E03;
	static const char* logtext;
	__asm {
		mov logtext, edx;
		call PrintsubIthink;
	}
	if (l_WarnAsserts)
		print("ASSERT: ");
	print(logtext);
	__asm {
		jmp returnAddress;
	}
}

#define WTF(x) (*(UINT*)(x))

const char*qbstructOpen = "QbStruct {\n";
void printStructStructIndent()
{
	for (int i = 0; i < qbstrindent + 1; i++) {
		print("    ");
	}
}
void printStructItem(QbKey key, DWORD value, QbValueType type);
void printStructBase(QbStruct*qs)
{
	if (!qs)
	{
		//printStructStructIndent();
		//print("NON-EXISTENT STRUCT!!!\n");
		return;
	}
	if (!qbstrindent)
	{
		print(qbstructOpen);
	}
	QbStructItem*qsi;
	qsi = qs->first;
	while (qsi)
	{
		printStructStructIndent();
		printStructItem(qsi->key, qsi->value, qsi->Type());
		if (!qsi->next)
			break;
		qsi = qsi->next;
	}
	if (!qbstrindent)
	{
		print("}\n");
	}
}
void printStructItem(QbKey key, DWORD value, QbValueType type)
{
	char qbstrstr[0x400];
	int qbstrstr2;
	char*keyStr;
	if (l_DbgLoaded)
	{
		if (key)
			keyStr = QbKeyPrintFix(key);
		else
			keyStr = "0x00000000";
		qbstrstr2 = sprintf_s(qbstrstr, "%02X %-16s = ", type, keyStr);
	}
	else
		qbstrstr2 = sprintf_s(qbstrstr, "%02X %08X = ", type, (int)key);
	// fallback thing, if no later type matches, print as qbkey
	sprintf_s(qbstrstr + qbstrstr2, sizeof(qbstrstr) - qbstrstr2, "%s\n", QbKeyPrintFix(value));
	switch (type)
	{
	case TypeInt:
		sprintf_s(qbstrstr + qbstrstr2, sizeof(qbstrstr) - qbstrstr2, "%d\n", (signed int)value);
		break;
	case TypeFloat:
		sprintf_s(qbstrstr + qbstrstr2, sizeof(qbstrstr) - qbstrstr2, "%f\n", (*(FLOAT*)&(value)));
		break;
	case TypeCString:
	case TypeWString:
		sprintf_s(qbstrstr + qbstrstr2, sizeof(qbstrstr) - qbstrstr2, "");
		break;
	case TypePair:
		sprintf_s(qbstrstr + qbstrstr2, sizeof(qbstrstr) - qbstrstr2, "(%f, %f)\n",
			*(float*)(&WTF(value) + 1),
			*(float*)(&WTF(value) + 2));
		break;
	case TypeVector:
		sprintf_s(qbstrstr + qbstrstr2, sizeof(qbstrstr) - qbstrstr2, "(%f, %f, %f)\n",
			*(float*)(&(WTF(value)) + 1),
			*(float*)(&(WTF(value)) + 2),
			*(float*)(&(WTF(value)) + 3));
		break;
	case TypeStringPointer:
		sprintf_s(qbstrstr + qbstrstr2, sizeof(qbstrstr) - qbstrstr2, "0x%08X\n", (DWORD*)(value));
		break;
	case TypeScript:
	case TypeCFunc:
	case TypeUnk9:
	case TypeUnk20:
	case TypeUnk21:
	case TypeBinaryTree1:
	case TypeBinaryTree2:
	case TypeQbMap:
	case TypeQbKeyStringQs:
		break;
		sprintf_s(qbstrstr + qbstrstr2, sizeof(qbstrstr) - qbstrstr2, "WHAT IS THIS: 0x%p: %08X %08X %08X %08X\n",
			(DWORD*)(value),
			*(DWORD*)(value), *(DWORD*)(value + 4),
			*(DWORD*)(value + 8), *(DWORD*)(value + 12));
		break;
	}
	if (//type != TypeStringPointer &&
		type != TypeCString &&
		type != TypeWString &&
		type != TypeQbStruct &&
		type != TypeQbArray)
	{
		print(qbstrstr);
	}
	else if (type == TypeCString) {
		print(qbstrstr);
		print("'");
		print((char*)value);
		print("'\n");
	}
	else if (type == TypeWString) {
		print(qbstrstr);
		if (l_CreateCon) {
			fprintf(CON, "\"%ls\"\n", (wchar_t*)value);
		}
		if (l_WriteFile) {
			fprintf(log, "\"%ls\"\n", (wchar_t*)value);
		}
	}
	else if (type == TypeQbArray) {
		sprintf(qbstrstr + qbstrstr2, "\n");
		print(qbstrstr);
		qbstrindent++;
		printStructStructIndent();
		sprintf(qbstrstr, "");
		QbArray*qa = (QbArray*)value;
		if (l_CreateCon)
			fprintf(CON, "%02X QbArray [", qa->Type());
		if (l_WriteFile)
			fprintf(log, "%02X QbArray [", qa->Type());
		DWORD weird;
		QbValueType qatype = qa->Type();
		// try to collapse items into multiple lines when there's a lot in the array
		DWORD qal = qa->Length();
#if 0
		DWORD qalnl = 0;
		if (qal > 16)
		{
			print("\n");
			qbstrindent++;
			printStructStructIndent();
			qalnl = 8;
			if (qal > 128)
			{
				qalnl = 1;
			}
		}
#endif
		for (UINT i = 0; i < qal; i++)
		{
			switch (qatype)
			{
			case TypeInt:
				char itoatmp[12];
				_itoa_s((signed int)qa->Get(i), itoatmp, 12, 10);
				print(itoatmp);
				break;
			case TypeFloat:
				char ftoatmp[16];
				weird = qa->Get(i);
				sprintf(ftoatmp, "%f", (*(FLOAT*)&weird));
				print(ftoatmp);
				break;
			case TypeCString:
				print(qbstrstr);
				print("\"");
				print((char*)value);
				print("\"\n");
				break;
			case TypeWString:
				if (l_CreateCon)
					fprintf(CON, "\"%ls\"", (wchar_t*)qa->Get(i));
				if (l_WriteFile)
					fprintf(log, "\"%ls\"", (wchar_t*)qa->Get(i));
				break;
			case TypeQbKey:
				print(QbKeyPrintFix(qa->Get(i)));
				break;
			case TypeQbStruct:
				print("\n");
				qbstrindent++;
				printStructStructIndent();
				print(qbstructOpen);
				qbstrindent++;
				printStructBase((QbStruct*)(qa->Get(i)));
				qbstrindent--;
				printStructStructIndent();
				if (i != qa->Length() - 1) {
					print("}");
				}
				else
				{
					qbstrindent--;
					print("}\n");
					printStructStructIndent();
					qbstrindent++;
				}
				qbstrindent--;
				break;
			}
			if (i != qa->Length() - 1) {
				print(", ");
			}
			else if (i == qa->Length() - 1)
			{
				if (qal > 8)
				{
					qbstrindent--;
				}
			}
#if 0
			if (qalnl > 0)
				if (i % qalnl == qalnl - 1 && qatype != TypeQbStruct)
				{
					print("\n");
					printStructStructIndent();
				}
#endif
		}
		print(qbstrstr);
		print("]\n");
		if (qal <= 8)
		{
			qbstrindent--;
		}
	}
	else {
		sprintf(qbstrstr + qbstrstr2, "\n");
		print(qbstrstr);
		printStructStructIndent();
		print(qbstructOpen);
		sprintf(qbstrstr, "");
		qbstrindent++;
		printStructBase((QbStruct*)(value));
		qbstrindent--;
		printStructStructIndent();
		print("}\n");
	}
}
static void *PrintStructDetour = (void *)0x00530970;
bool PrintStruct(QbStruct* params, QbScript* _this)
{
	if (params)
		printStructBase(params);
	return params;
}
char print2ndBuf[512];
// detours 0x005B0DC0
int extraPrinting(char* fmt, ...)
{
	// hasn't been executed
	print("TESTTESTTESTTESTTESTTESTTESTTEST\n");
	va_list args;
	va_start(args, fmt);
	int ret = vsprintf_s(print2ndBuf, 512, fmt, args);
	va_end(args);
	print(print2ndBuf);
	return ret;
}

FILE*gh3pl;
char gh3pll[64];
// run after GH3+ core loads evrything
static void *RandomDetour2 = (void *)0x0047B30B;
__declspec(naked) void* PrintGH3Plog()
{
	static const uint32_t returnAddress = 0x0047B310;
	gh3pl = fopen("PLUGINS\\_log.txt", "r");
	if (gh3pl)
	{
		fputs("GH3+ Log:\n", CON);
		while (fgets(gh3pll, 64, gh3pl))
		{
			fputs(gh3pll, CON);
		}
		fputs("\n", CON);
		fclose(gh3pl);
	}
	else
		fputs("Cannot access GH3+ log.\n\n", CON);
	__asm {
		jmp returnAddress;
	}
}


DWORD kIndex = 0;
static void *AddToLookupDetour = (void *)0x00DA22A6;
void AddToMyLookup(QbKey checksum, char*keyname)
{
	if (kIndex < MAX_KEYS)
	{
		for (DWORD i = kIndex - 1; i > 0; i--)
		{
			if (checksum == dbgKeys[i])
			{
				return;
			}
		}
		memcpy(dbgStr + (kIndex * KEYSTRLEN), keyname, 128);
		dbgKeys[kIndex] = checksum;
		kIndex++;
	}
	else
	{
		if (l_CreateCon)
			fputs("FormatText's AddToLookup failed. Exceeded amount of debug keys allowed.\n", CON);
	}
}

#include "gh3\malloc.h"
static void* GCStkDetour = (void*)0x00537A50;
bool GetCCallstack(QbStruct*params, QbScript*_this)
{
	QbArray*callstack = (QbArray*)GH3::qbArrMalloc(sizeof(QbArray), 0);
	memset(callstack, 0, sizeof(QbArray));
	//callstack->Clear();
#if 0
	callstack->Initialize(_this->scriptDepth, QbValueType::TypeQbStruct);
	for (int i = 0; i < _this->scriptDepth; i++)
	{
		auto RA = _this->mp_return_addresses[i];
		QbStruct*_cs = (QbStruct*)GH3::qbMalloc(sizeof(QbStruct), 0);
		memset(_cs, 0, sizeof(GH3::QbStruct));
		_cs->InsertQbKeyItem((uint32_t)0, RA.mScriptNameChecksum);
		//_cs->InsertQbStructItem((uint32_t)0, _this->mp_return_addresses[i].mpParams);
		callstack->Set(i, (UINT)_cs);
	}
#else
	callstack->Initialize(_this->scriptDepth, QbValueType::TypeQbKey);
	for (int i = 0; i < _this->scriptDepth; i++)
	{
		// feels incomplete, the above crashes somehow after printing the struct containing the item
		auto RA = _this->mp_return_addresses[i];
		callstack->Set(i, (UINT)RA.mScriptNameChecksum);
	}
#endif
	_this->qbStruct1C->InsertQbArrayItem(QbKey("callstack"), callstack);
	return 0;
}

static wchar_t inipath[MAX_PATH], logfile[MAX_PATH], dbgpath[MAX_PATH];
void ApplyHack()
{
	GetCurrentDirectoryW(MAX_PATH, inipath);
	memcpy(logfile, inipath, MAX_PATH);
	memcpy(dbgpath, inipath, MAX_PATH);
	wcscat_s(inipath, MAX_PATH, L"\\settings.ini");
	wcscat_s(dbgpath, MAX_PATH, L"\\DATA\\PAK\\dbg.pak.xen");
	l_CreateCon = GetPrivateProfileIntW(L"Logger", L"Console", 0, inipath);
	l_WriteFile = GetPrivateProfileIntW(L"Logger", L"WriteFile", 1, inipath);
	l_FixKeys = GetPrivateProfileIntW(L"Logger", L"FixKeys", 1, inipath);
	l_PrintStruct = GetPrivateProfileIntW(L"Logger", L"PrintStruct", 1, inipath);
	l_FmtTxtLkup = GetPrivateProfileIntW(L"Logger", L"FmtTxtAddToLkup", 1, inipath);
	//l_ParseFormatting = GetPrivateProfileIntW(L"Logger", L"ParseFormatting", 1, inipath); // for escape sequences for no reason, which don't actually appear ever
	l_WarnAsserts = GetPrivateProfileIntW(L"Logger", L"WarnAsserts", 0, inipath);
	if (l_CreateCon)
	{
		l_CreateCon = 0;
		if (AllocConsole())
			if (!freopen_s(&CON, "CONOUT$", "w", stdout))
				l_CreateCon = 1;
		if (!l_CreateCon)
			l_CreateConFail = 1;
	}
	if (l_WriteFile)
	{
		wcscat_s(logfile, MAX_PATH, L"\\output.txt");
		l_WriteFile = 0;
		log = _wfsopen(logfile, L"w", _SH_DENYNO);
		if (log)
		{
			l_WriteFile = 1;
			if (l_CreateConFail)
				fputs("Error creating console.\n", log);
		}
	}
	if (l_CreateCon)
	{
		fputs("Guitar Hero 3 log\n--------------------\nPatching print functions...\n", CON);
	}
	if (!g_patcher.WriteJmp(ScrAsrtDetour, MyScriptAssert) ||
		!g_patcher.WriteJmp((void*)0x00530940, (void*)0x00532E10) || // Printf jump to my function
		!g_patcher.WriteJmp((void*)0x00532E2A, RedirectOutput2)) // SoftAsrt jump to my function
	{
		g_patcher.RemoveAllChanges();
		l_AllSuccess = 0;
		if (l_CreateCon)
		{
			fputs("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!", CON);
			for (int i = 0; i < 10; i++)
				fputs("!!!!!!!!!! Failed to redirect prints !!!!!!!!!!!", CON);
			fputs("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!", CON);
		}
		g_patcher.WriteJmp((void*)0x005B0DC0, extraPrinting);
		return;
	}
	else {
		/*if (!l_ParseFormatting)
		{
			CONh = GetStdHandle(STD_OUTPUT_HANDLE);
		}*/
		if (l_FixKeys) {
			if (l_CreateCon)
				fputs("Loading debug pak.\n", CON);
			dbgpak = _wfopen(dbgpath, L"rb");
			if (dbgpak) {
				char ftmp[0x40000], *chksmsSect = "[Checksums]"; // largest official debug file is 157491
				unsigned char b0, b1, b2, b3;
				DWORD checkIfNZ = 0, QFpos = 0, QFlen = 0, curFileCur = 0, keyNameLen = 0;
				DWORD curChksm = 0;
				char scanDupe = 0;
				char cpath = 0;
				if (l_CreateCon)
					fputs("Allocating memory for lookup.\n", CON);
				dbgKeys = (UINT*)calloc(MAX_KEYS, sizeof(UINT));
				dbgStr = (char*)calloc(MAX_KEYS, KEYSTRLEN * 4);
				do
				{
					//fprintf(CON, "%u\n", curFileCur);
					fread((DWORD*)&checkIfNZ, 4, 1, dbgpak);
					if (checkIfNZ != 0xCC669555)
						break;
					// BIG ENDIAN IS BASED
					fread(&b3, sizeof b3, 1, dbgpak);
					fread(&b2, sizeof b2, 1, dbgpak);
					fread(&b1, sizeof b1, 1, dbgpak);
					fread(&b0, sizeof b0, 1, dbgpak);
					QFpos = (((DWORD)b3) << 24) | (((DWORD)b2) << 16) | (((DWORD)b1) << 8) | b0;
					fread(&b3, sizeof b3, 1, dbgpak);
					fread(&b2, sizeof b2, 1, dbgpak);
					fread(&b1, sizeof b1, 1, dbgpak);
					fread(&b0, sizeof b0, 1, dbgpak);
					QFlen = (((DWORD)b3) << 24) | (((DWORD)b2) << 16) | (((DWORD)b1) << 8) | b0;
					//fprintf(CON, "\n%p\n%p\n%p\n%p\n", checkIfNZ, ftell(dbgpak), QFpos, QFlen); nice one wes
					fseek(dbgpak, QFpos - 12, SEEK_CUR);
					fread(ftmp, 1, QFlen, dbgpak);
					fseek(dbgpak, (QFpos + QFlen)*-1, SEEK_CUR);
					fseek(dbgpak, 0x20, SEEK_CUR);
#define nodupes 0
					curFileCur = 0;
					while (strncmp(ftmp + curFileCur, chksmsSect, 11) != 0)
					{
						curFileCur++;
					}
					cpath = 0;
					curFileCur += 13;
					while (curFileCur < QFlen && kIndex < MAX_KEYS)
					{
						scanDupe = 0;
						if (ftmp[curFileCur] == '0' &&
							ftmp[curFileCur + 1] == 'x')
						{
							curFileCur += 2;
							curChksm = strtoul(ftmp + curFileCur, 0, 16);
#if !nodupes
							for (uint16_t i = 0; i < kIndex; i++) // dupes happen around 13k keys in
							{
								if (curChksm == dbgKeys[i])
								{
									scanDupe = 1;
									//fprintf(CON, "%u\n", i);
									break;
								}
								if (!dbgKeys[i])
								{
									scanDupe = 1;
									break;
								}
								// reduced dbg.pak (for faster key loading):
								// https://donnaken15.com/dbg.pak.xen
							}
							if (!scanDupe)
#endif
								dbgKeys[kIndex] = curChksm;
							curFileCur += 9;
							keyNameLen = 0;
							while (ftmp[curFileCur + keyNameLen] != 0x0D &&
								ftmp[curFileCur + keyNameLen] >= 0x20)
							{
								keyNameLen++;
							}
#if !nodupes
							if (!scanDupe)
#endif
							{
								memcpy(dbgStr + (kIndex * KEYSTRLEN), ftmp + curFileCur, keyNameLen);
								curFileCur += keyNameLen;
								kIndex++;
							}
						}
						curFileCur++;
					}
					if (l_CreateCon)
						fprintf(CON, "\rLoading debug keys. (%u)", kIndex);
				} while (checkIfNZ);
				if (l_CreateCon)
					fputs("\n", CON);
				l_DbgLoaded = 1;
				if (g_patcher.WriteJmp(UnkKeyDetour, QbKeyPrintFix))
				{
					if (l_CreateCon) {
						fputs("Patched debug key formatter.\n", CON);
					}
				}
				else {
					print("Failed to patch debug key formatter.\n");
					l_AllSuccess = 0;
				}
				if (l_FmtTxtLkup)
					if (g_patcher.WriteCall(AddToLookupDetour, AddToMyLookup))
					{
						if (l_CreateCon)
							fputs("Patched FormatText's AddToLookup code.\n", CON);
					}
					else {
						print("Failed to patch FormatText's AddToLookup code.\n");
						l_AllSuccess = 0;
					}
					fclose(dbgpak);
			}
			else {
				print("Failed to open debug pak.\nDoes it exist?\n");
				l_AllSuccess = 0;
			}
		}
		if (l_PrintStruct) {
			if (g_patcher.WriteJmp(PrintStructDetour, PrintStruct) &&
				g_patcher.WriteInt8((void*)(&PrintStructDetour + 5), 0xC3)) // lol
			{
				if (l_CreateCon)
					fputs("Patched PrintStruct.\n", CON);
			}
			else {
				print("Failed to patch PrintStruct.\n");
				l_AllSuccess = 0;
			}
			if (g_patcher.WriteJmp(PrintScriptInfoDetour, PrintScriptInfo))
			{
				if (l_CreateCon)
					fputs("Patched PrintScriptInfo.\n", CON);
			}
			else {
				print("Failed to patch PrintScriptInfo.\n");
			}
			if (g_patcher.WriteJmp(GCStkDetour, GetCCallstack))
			{
				if (l_CreateCon)
					fputs("Patched GetCCallstack.\n", CON);
			}
			else {
				print("Failed to patch GetCCallstack.\n");
			}
		}
		if (l_CreateCon)
			if (!g_patcher.WriteJmp(RandomDetour2, PrintGH3Plog))
			{
				fputs("Cannot access GH3+ log.\n\n", CON);
				l_AllSuccess = 0;
			}
		if (l_WarnAsserts && (l_CreateCon || l_WriteFile))
		{
			if (g_patcher.WriteJmp(LogErrDfnctDetour, assertWarn)) // lol
			{
				if (l_CreateCon)
					fputs("Patched assert function.\n", CON);
			}
			else
				print("Failed to patch assert function.\n");
		}
		if (l_AllSuccess)
			if (l_CreateCon)
				fputs("Success!\n\n", CON);
	}
}