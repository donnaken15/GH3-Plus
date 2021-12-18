#include "gh3\GH3Keys.h"
#include "gh3\GlobalMap.h"
#include "gh3\QbValueType.h"
#include "core\Patcher.h"
#include "Logger.h"
#include <stdint.h>
#include <stdio.h>
#include <Windows.h>
#include "gh3\GH3Functions.h"

/// code by wesley and not zedek
///
///   #################################
/// ##         ## # ####### ## # ######
///             ## # ######  ## # #### 
///              ## # ####    #######  
///               #######              
///
/// https://donnaken15.tk/
/// https://youtube.com/donnaken15
/// https://github.com/donnaken15

static char inipath[MAX_PATH],
			logfile[MAX_PATH],
			dbgpath[MAX_PATH];
BYTE l_CreateCon, l_WriteFile, l_FixKeys,
l_PrintStruct, l_AllSuccess = 1,
l_CreateConFail = 0, l_DbgLoaded = 0,
l_FmtTxtLkup;
FILE * log, * CON;

static GH3P::Patcher g_patcher = GH3P::Patcher(__FILE__);

static void *ScrAsrtDetour = (void *)0x00532DFE;
__declspec(naked) void RedirectOutput()
{
	static const uint32_t returnAddress = 0x00532E03;
	static const char*logtext;
	__asm {
		mov logtext, edx;
		call PrintsubIthink;
	}
	if (l_CreateCon)
		fputs(logtext, CON);
	if (l_WriteFile)
		fputs(logtext, log);
	__asm {
		jmp returnAddress;
	}
}

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
	__asm {
		mov eax, keytmp;
		jmp returnAddress;
	}
}

#define WTF(x) (*(UINT*)(x))

const char*qbstructOpen = "QbStruct {\n";
int qbstrindent = 0;
void printStructStructIndent()
{
	for (int i = 0; i < qbstrindent + 1; i++) {
		if (l_CreateCon) {
			fputs("    ", CON);
		}
		if (l_WriteFile) {
			fputs("    ", log);
		}
	}
}
void printStructItem(QbKey key, DWORD value, QbValueType type);
void printStructBase(QbStruct*qs)
{
	if (!qbstrindent)
	{
		if (l_CreateCon)
			fputs(qbstructOpen, CON);
		if (l_WriteFile)
			fputs(qbstructOpen, log);
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
		if (l_CreateCon)
			fputs("}\n", CON);
		if (l_WriteFile)
			fputs("}\n", log);
	}
}
void printStructItem(QbKey key, DWORD value, QbValueType type)
{
	char qbstrstr[0x80];
	int qbstrstr2;
	if (l_DbgLoaded)
	{
		if (key)
			qbstrstr2 = sprintf(qbstrstr, "%02X %-16s = ", type, QbKeyPrintFix(key));
		else
			qbstrstr2 = sprintf(qbstrstr, "%02X %-16s = ", type, "0x00000000");
	}
	else
		qbstrstr2 = sprintf(qbstrstr, "%02X %08X = ", type, (int)key);
	// fallback thing, if no later type matches, print as qbkey
	if (l_DbgLoaded)
		sprintf(qbstrstr + qbstrstr2, "%s\n", QbKeyPrintFix(value));
	else
		sprintf(qbstrstr + qbstrstr2, "%08X\n", value);
	switch (type)
	{
	case TypeInt:
		sprintf(qbstrstr + qbstrstr2, "%d\n", (signed int)value);
		break;
	case TypeFloat:
		sprintf(qbstrstr + qbstrstr2, "%f\n", (*(FLOAT*)&(value)));
		break;
	case TypeCString:
	case TypeWString:
		sprintf(qbstrstr + qbstrstr2, "");
		break;
	case TypePair:
		sprintf(qbstrstr + qbstrstr2, "(%f, %f)\n",
			*(float*)(&WTF(value) + 1),
			*(float*)(&WTF(value) + 2));
		break;
	case TypeVector:
		sprintf(qbstrstr + qbstrstr2, "(%f, %f, %f)\n",
			*(float*)(&(WTF(value)) + 1),
			*(float*)(&(WTF(value)) + 2),
			*(float*)(&(WTF(value)) + 3));
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
		sprintf(qbstrstr + qbstrstr2, "WHAT IS THIS: 0x%p: %08X %08X %08X %08X\n",
			(DWORD*)(value),
			*(DWORD*)(value), *(DWORD*)(value + 4),
			*(DWORD*)(value + 8), *(DWORD*)(value + 12));
		break;
	}
	if (type != TypeStringPointer &&
		type != TypeCString &&
		type != TypeWString &&
		type != TypeQbStruct &&
		type != TypeQbArray)
		{
		if (l_CreateCon) {
			fputs(qbstrstr, CON);
		}
		if (l_WriteFile) {
			fputs(qbstrstr, log);
		}
	}
	else if (type == TypeCString) {
		if (l_CreateCon) {
			fputs(qbstrstr, CON);
			fprintf(CON, "'%s'\n", (char*)value);
		}
		if (l_WriteFile) {
			fputs(qbstrstr, log);
			fprintf(log, "'%s'\n", (char*)value);
		}
	}
	else if (type == TypeWString) {
		if (l_CreateCon) {
			fputs(qbstrstr, CON);
			fprintf(CON, "\"%ls\"\n", (wchar_t*)value);
		}
		if (l_WriteFile) {
			fputs(qbstrstr, log);
			fprintf(log, "\"%ls\"\n", (wchar_t*)value);
		}
	}
	else if (type == TypeQbArray) {
		sprintf(qbstrstr + qbstrstr2, "\n");
		if (l_CreateCon) {
			fputs(qbstrstr, CON);
		}
		if (l_WriteFile) {
			fputs(qbstrstr, log);
		}
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
		DWORD qalnl = 0;
		if (qal > 16)
		{
			if (l_CreateCon) {
				fputs("\n", CON);
			}
			if (l_WriteFile) {
				fputs("\n", log);
			}
			qbstrindent++;
			printStructStructIndent();
			qalnl = 8;
			if (qal > 128)
			{
				qalnl = 1;
			}
		}
		for (UINT i = 0; i < qal; i++)
		{
			switch (qatype)
			{
			case TypeInt:
				if (l_CreateCon)
					fprintf(CON, "%d", (signed int)qa->Get(i));
				if (l_WriteFile)
					fprintf(log, "%d", (signed int)qa->Get(i));
				break;
			case TypeFloat:
				weird = qa->Get(i);
				if (l_CreateCon)
					fprintf(CON, "%f", (*(FLOAT*)&weird));
				if (l_WriteFile)
					fprintf(log, "%f", (*(FLOAT*)&weird));
				break;
			case TypeCString:
				if (l_CreateCon)
					fprintf(CON, "\"%s\"", (char*)qa->Get(i));
				if (l_WriteFile)
					fprintf(log, "\"%s\"", (char*)qa->Get(i));
				break;
			case TypeWString:
				if (l_CreateCon)
					fprintf(CON, "\"%ls\"", (wchar_t*)qa->Get(i));
				if (l_WriteFile)
					fprintf(log, "\"%ls\"", (wchar_t*)qa->Get(i));
				break;
			case TypeQbKey:
				if (l_DbgLoaded) {
					if (l_CreateCon) {
						fprintf(CON, "%s", QbKeyPrintFix(qa->Get(i)));
					}
					if (l_WriteFile) {
						fprintf(log, "%s", QbKeyPrintFix(qa->Get(i)));
					}
				}
				else {
					if (l_CreateCon) {
						fprintf(CON, "%08X", qa->Get(i));
					}
					if (l_WriteFile) {
						fprintf(log, "%08X", qa->Get(i));
					}
				}
				break;
			case TypeQbStruct:
				if (l_CreateCon) {
					fputs("\n", CON);
				}
				if (l_WriteFile) {
					fputs("\n", log);
				}
				qbstrindent++;
				printStructStructIndent();
				if (l_CreateCon) {
					fputs(qbstructOpen, CON);
				}
				if (l_WriteFile) {
					fputs(qbstructOpen, log);
				}
				qbstrindent++;
				printStructBase((QbStruct*)(qa->Get(i)));
				qbstrindent--;
				printStructStructIndent();
				if (i != qa->Length() - 1) {
					if (l_CreateCon)
						fputs("}", CON);
					if (l_WriteFile)
						fputs("}", log);
				}
				else
				{
					qbstrindent--;
					if (l_CreateCon)
						fputs("}\n", CON);
					if (l_WriteFile)
						fputs("}\n", log);
					printStructStructIndent();
					qbstrindent++;
				}
				qbstrindent--;
				break;
			}
			if (i != qa->Length() - 1) {
				if (l_CreateCon) {
					fputs(", ", CON);
				}
				if (l_WriteFile) {
					fputs(", ", log);
				}
			}
			else if (i == qa->Length() - 1)
			{
				if (qal > 8)
				{
					qbstrindent--;
				}
			}
			if (qalnl > 0)
				if (i % qalnl == qalnl - 1 && qatype != TypeQbStruct)
				{
					if (l_CreateCon)
						fputs("\n", CON);
					if (l_WriteFile)
						fputs("\n", log);
					printStructStructIndent();
				}
		}
		if (l_CreateCon) {
			fputs(qbstrstr, CON);
			fputs("]\n", CON);
		}
		if (l_WriteFile) {
			fputs(qbstrstr, log);
			fputs("]\n", log);
		}
		if (qal <= 8)
		{
			qbstrindent--;
		}
	}
	else {
		sprintf(qbstrstr + qbstrstr2, "\n");
		if (l_CreateCon) {
			fputs(qbstrstr, CON);
		}
		if (l_WriteFile) {
			fputs(qbstrstr, log);
		}
		printStructStructIndent();
		if (l_CreateCon) {
			fputs(qbstructOpen, CON);
		}
		if (l_WriteFile) {
			fputs(qbstructOpen, log);
		}
		sprintf(qbstrstr, "");
		qbstrindent++;
		printStructBase((QbStruct*)(value));
		qbstrindent--;
		printStructStructIndent();
		if (l_CreateCon)
			fputs("}\n", CON);
		if (l_WriteFile)
			fputs("}\n", log);
	}
}
QbStruct*a1;
static void *PrintStructDetour = (void *)0x00530970;
__declspec(naked) void MyPrintStruct()
{
	static const uint32_t returnAddress = 0x00530975;
	DWORD a, c;
	__asm {
		push ebp;
		mov ebp, esp;
		mov a, eax;
		mov c, ecx;
		mov[a1], ecx;
	}
	if (c >= 0xF0000000) // stupid thing
	{
		__asm mov eax, a;
		__asm mov[a1], eax;
	}
	printStructBase(a1);
	__asm {
		mov al, 1;	// zedek told me to put this
					// game still works without it lol
					// apparently required in cfuncs
		pop ebp;
		ret;
	}
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
static void *AddToLookupDetour = (void *)0x00DA22A4;
__declspec(naked) void* AddToMyLookup()
{
	static const uint32_t returnAddress = 0x00DA22AE;
	char*keyname;
	DWORD checksum;
	if (kIndex < MAX_KEYS)
	{
		__asm {
			mov[keyname], edx;
			mov checksum, esi;
		}
		for (DWORD i = kIndex - 1; i > 0; i--)
		{
			if (checksum == dbgKeys[i])
			{
				__asm {
					jmp returnAddress;
				}
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
	__asm {
		jmp returnAddress;
	}
}

void ApplyHack()
{
	GetCurrentDirectoryA(MAX_PATH, inipath);
	memcpy(logfile, inipath, MAX_PATH);
	memcpy(dbgpath, inipath, MAX_PATH);
	strcat_s(inipath, MAX_PATH, "\\settings.ini");
	strcat_s(dbgpath, MAX_PATH, "\\DATA\\PAK\\dbg.pak.xen");
	l_CreateCon = GetPrivateProfileIntA("Logger", "Console", 0, inipath);
	l_WriteFile = GetPrivateProfileIntA("Logger", "WriteFile", 1, inipath);
	l_FixKeys = GetPrivateProfileIntA("Logger", "FixKeys", 1, inipath);
	l_PrintStruct = GetPrivateProfileIntA("Logger", "PrintStruct", 1, inipath);
	l_FmtTxtLkup = GetPrivateProfileIntA("Logger", "FmtTxtAddToLkup", 1, inipath);
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
		strcat_s(logfile, MAX_PATH, "\\output.txt");
		l_WriteFile = 0;
		if (!fopen_s(&log, logfile, "w"))
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
	if (!g_patcher.WriteJmp(ScrAsrtDetour, RedirectOutput) ||
		!g_patcher.WriteJmp((void*)0x00530940, (void*)0x00532DD0) || // Printf jump to my function
		!g_patcher.WriteJmp((void*)0x00532E2A, RedirectOutput)) // SoftAsrt jump to my function
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
		return;
	}
	else {
		if (l_FixKeys) {
			if (l_CreateCon)
				fputs("Loading debug pak.\n", CON);
			dbgpak = fopen(dbgpath, "rb");
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
								// https://donnaken15.tk/dbg.pak.xen
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
					if (l_CreateCon)
						fputs("Failed to patch debug key formatter.\n", CON);
					if (l_WriteFile)
						fputs("Failed to patch debug key formatter.\n", log);
					l_AllSuccess = 0;
				}
				if (l_FmtTxtLkup)
					if (g_patcher.WriteJmp(AddToLookupDetour, AddToMyLookup))
					{
						if (l_CreateCon)
							fputs("Patched FormatText's AddToLookup code.\n", CON);
					}
					else {
						if (l_CreateCon)
							fputs("Failed to patch FormatText's AddToLookup code.\n", CON);
						if (l_WriteFile)
							fputs("Failed to patch FormatText's AddToLookup code.\n", log);
						l_AllSuccess = 0;
					}
					fclose(dbgpak);
			}
			else {
				if (l_CreateCon)
					fputs("Failed to open debug pak.\nDoes it exist?\n", CON);
				if (l_WriteFile)
					fputs("Failed to open debug pak.\nDoes it exist?\n", log);
				l_AllSuccess = 0;
			}
		}
		if (l_PrintStruct) {
			if (g_patcher.WriteJmp(PrintStructDetour, MyPrintStruct) &&
				g_patcher.WriteInt8((void*)(&PrintStructDetour + 5), 0xC3)) // lol
			{
				if (l_CreateCon)
					fputs("Patched PrintStruct.\n", CON);
			}
			else {
				if (l_CreateCon)
					fputs("Failed to patch PrintStruct.\n", CON);
				if (l_WriteFile)
					fputs("Failed to patch PrintStruct.\n", log);
				l_AllSuccess = 0;
			}
		}
		if (l_AllSuccess)
			if (l_CreateCon)
				fputs("Success!\n\n", CON);
		if (l_CreateCon)
			if (!g_patcher.WriteJmp(RandomDetour2, PrintGH3Plog))
			{
				fputs("Cannot access GH3+ log.\n\n", CON);
			}
	}
}