#include "outputPlayedSong.h"
#include "core/Patcher.h"
#include "gh3\QbKey.h"
#include "gh3\QbStruct.h"
#include <fstream>
#include <string>

static GH3P::Patcher g_patcher = GH3P::Patcher(__FILE__);

constexpr GH3::QbKey IdKey = "id";
constexpr GH3::QbKey TextKey = "text";
constexpr GH3::QbKey IntroSongInfoText = "intro_song_info_text";
constexpr GH3::QbKey IntroArtistInfoText = "intro_artist_info_text";

static int pluginState = 0;

static std::wstring songName;
static std::wstring songArtist;

std::wstring getString(GH3::QbStruct* args, GH3::QbKey key)
{
	GH3::QbStructItem* item  = args->GetItem(key);
	auto cstr = reinterpret_cast<const wchar_t*>(item->value);
	
	return cstr;
}

void __cdecl setScreenElementPropsOverride(GH3::QbStruct* args)
{
	GH3::QbKey targetKey = (uint32_t)0;
	args->GetQbKey(IdKey, targetKey);

	if(targetKey == IntroSongInfoText)
	{
		pluginState |= 1;
		songName = getString(args, TextKey);
	}
	else if(targetKey == IntroArtistInfoText)
	{
		pluginState |= 2;
		songArtist = getString(args, TextKey);
	}

	if((pluginState & 3) == 3)
	{
		pluginState = 0;

		std::wofstream outFile("songinfo.txt", std::ios::out | std::ios::trunc);
		outFile << songName << std::endl;
		outFile << songArtist << std::endl;
	}
}

static void* const setScreenElementPropsDetour = (void*)0x491D24;
__declspec(naked) void setScreenElementPropsOverrideNaked()
{
	static void* const g_noteFxRelated1 = (void*)0xB54440;
	static void* const returnAddress = (void*)0x491D2A;

	__asm {
		pushad;
		push esi;
		call setScreenElementPropsOverride;
		pop esi;
		popad;
		mov ecx, g_noteFxRelated1;
		jmp returnAddress;
	}
}

void ApplyHack()
{
	g_patcher.WriteJmp(setScreenElementPropsDetour, setScreenElementPropsOverrideNaked);
}