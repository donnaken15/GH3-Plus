#include "core\Patcher.h"
#include "gh3\GH3Keys.h"
#include "gh3\GH3GlobalAddresses.h"
#include "gh3\QbKey.h"
#include "gh3\QbImage.h"
#include "gh3\QbMap.h"
#include "gh3\TextureMetadata.h"
#include "tapHopoChord.h"
#include <map>

// ACTUALLY, .SCN FILES MIGHT BE ALL I NEEDED
// it half works
#define DoTextureTakeover 1

#if (DoTextureTakeover == 1)
#define ArrayLength(x) (sizeof(x)/sizeof(*(x)))

static GH3::QbKey tmpTextureKey;
static GH3::TextureMetadata *tmpMetadata;
static GH3::QbImage *tmpQbImage;


// Probably don't need to understand what's in here as long as we have it
struct WeirdTextureStruct
{
	GH3::QbKey key;
	uint32_t unk4_0;
	GH3::TextureMetadata *metadata;
	uint32_t unkC_0;

	uint32_t unk10_0;
	uint32_t unk14_0;
	uint32_t unk18_0;
	uint32_t unk1C_0;
	uint32_t unk20_0;
	uint32_t unk24_0;
	uint32_t unk28_0;
	uint32_t unk2C_0;
	uint32_t unk30_0;
	uint32_t unk34_0;
	uint32_t unk38_0;
	uint32_t unk3C_0;
	uint32_t unk40_0;
	uint32_t unk44_0;
	uint32_t unk48_0;
	uint32_t unk4C_0;
	uint32_t unk50_0;
	uint32_t unk54_0;
	void * unk58;
	uint32_t unk5C_0;

	uint32_t unk60_0;
	uint32_t unk64_0;
	uint32_t unk68_0;
	uint32_t unk6C_0;

	uint32_t unk70_0;
	uint32_t unk74_0;
	uint32_t unk78_0;
	uint32_t unk7C_0;

	uint32_t unk80_0;
	uint32_t unk84_0;
	uint32_t unk88_0;
	uint32_t unk8C_0;

	uint32_t unk90_0;
	uint32_t unk94_0;
	uint32_t unk98_0;
	uint32_t unk9C_0;

	uint32_t unkA0_weird;
	void * unkA4_boolCountOrStruct;

	uint32_t unkA8_2;
	float * unkAC_PTR; // animation data is here i think

	uint32_t unkB0_1;
	float * unkB4_PTR;

	uint32_t unkB8_1;
	float * unkBC_PTR;

	uint32_t unkC0_0;
	GH3::QbKey * unkC4_PTR;

	uint32_t unkC8_0;
	uint32_t unkCC_4;

	uint32_t unkD0_0;
	uint32_t unkD4_0;
	uint32_t unkD8_3;
	uint32_t unkDC_0;

	uint32_t unkF0_0;
	uint32_t unkF4_maybeHeight320;
	uint32_t unkF8_maybeWidth512;
	uint32_t unkFC_0;

	uint32_t unk100_0;
	uint32_t unk104_0;
	uint32_t unk108_0;
	uint32_t unk10C_0;
	uint32_t unk110_0; //float
	uint32_t unk114_0;
	uint32_t unk118_0;
	uint32_t unk11C_0;
	uint32_t unk120_0; //float
	uint32_t unk124_0;
	uint32_t unk128_0;
	uint32_t unk12C_0;
};




static std::map<uint32_t, GH3::QbImage *> g_imageMap;
static std::map<uint32_t, GH3::TextureMetadata *> g_metadataMap;
static std::map<uint32_t, WeirdTextureStruct> g_weirdMap;

static constexpr GH3::QbKey g_rawKeys[] = {
	RawOpenTextureKey,
	RawOpenHammerTextureKey,
	RawOpenStarTextureKey,
	RawOpenStarHammerTextureKey,
	RawOpenStarpowerTextureKey,
	RawOpenStarpowerHammerTextureKey,
#if OPEN_NOTEFX
	RawOpenWhammyTextureKey,
	RawOpenWhammyStarTextureKey,
	RawOpenWhammyDeadTextureKey,
	RawOpenHitFx1TextureKey,
	RawOpenHitFx2TextureKey,
#endif
	RawGreenTapTextureKey,
	RawRedTapTextureKey,
	RawYellowTapTextureKey,
	RawBlueTapTextureKey,
	RawOrangeTapTextureKey,
	RawGreenTapStarTextureKey,
	RawRedTapStarTextureKey,
	RawYellowTapStarTextureKey,
	RawBlueTapStarTextureKey,
	RawOrangeTapStarTextureKey,
	RawStarpowerTapTextureKey,
#ifdef RAINBOW
	RawRainbow1TextureKey,
	RawRainbow2TextureKey,
	RawRainbow3TextureKey,
	RawRainbow4TextureKey,
	RawRainbow5TextureKey,
	RawRainbow1HammerTextureKey,
	RawRainbow2HammerTextureKey,
	RawRainbow3HammerTextureKey,
	RawRainbow4HammerTextureKey,
	RawRainbow5HammerTextureKey,
	RawRainbow1StarTextureKey,
	RawRainbow2StarTextureKey,
	RawRainbow3StarTextureKey,
	RawRainbow4StarTextureKey,
	RawRainbow5StarTextureKey,
	RawRainbow1StarHammerTextureKey,
	RawRainbow2StarHammerTextureKey,
	RawRainbow3StarHammerTextureKey,
	RawRainbow4StarHammerTextureKey,
	RawRainbow5StarHammerTextureKey
#endif
};

static constexpr GH3::QbKey g_textureKeys[] = {
	OpenTextureKey,
	OpenHammerTextureKey,
	OpenStarTextureKey,
	OpenStarHammerTextureKey,
	OpenStarpowerTextureKey,
	OpenStarpowerHammerTextureKey,
#if OPEN_NOTEFX
	OpenWhammyTextureKey,
	OpenWhammyStarTextureKey,
	OpenWhammyDeadTextureKey,
	OpenHitFx1TextureKey,
	OpenHitFx2TextureKey,
#endif
	GreenTapTextureKey,
	RedTapTextureKey,
	YellowTapTextureKey,
	BlueTapTextureKey,
	OrangeTapTextureKey,
	GreenTapStarTextureKey,
	RedTapStarTextureKey,
	YellowTapStarTextureKey,
	BlueTapStarTextureKey,
	OrangeTapStarTextureKey,
	StarpowerTapTextureKey,
#ifdef RAINBOW
	Rainbow1TextureKey,
	Rainbow2TextureKey,
	Rainbow3TextureKey,
	Rainbow4TextureKey,
	Rainbow5TextureKey,
	Rainbow1HammerTextureKey,
	Rainbow2HammerTextureKey,
	Rainbow3HammerTextureKey,
	Rainbow4HammerTextureKey,
	Rainbow5HammerTextureKey,
	Rainbow1StarTextureKey,
	Rainbow2StarTextureKey,
	Rainbow3StarTextureKey,
	Rainbow4StarTextureKey,
	Rainbow5StarTextureKey,
	Rainbow1StarHammerTextureKey,
	Rainbow2StarHammerTextureKey,
	Rainbow3StarHammerTextureKey,
	Rainbow4StarHammerTextureKey,
	Rainbow5StarHammerTextureKey
#endif
};


///////////////////////////
// Retrieve our textures //
///////////////////////////

static GH3::QbKey getYellowEquivalent(GH3::QbKey openKey)
{
	switch (openKey)
	{
	case static_cast<uint32_t>(OpenTextureKey) :
	case static_cast<uint32_t>(GreenTapTextureKey) :
	case static_cast<uint32_t>(RedTapTextureKey) :
	case static_cast<uint32_t>(YellowTapTextureKey) :
	case static_cast<uint32_t>(BlueTapTextureKey) :
	case static_cast<uint32_t>(OrangeTapTextureKey) :
	case static_cast<uint32_t>(StarpowerTapTextureKey) :
		return YellowTextureKey;

	case static_cast<uint32_t>(OpenHammerTextureKey):
		return YellowHammerTextureKey;

	case static_cast<uint32_t>(OpenStarTextureKey) :
	case static_cast<uint32_t>(GreenTapStarTextureKey) :
	case static_cast<uint32_t>(RedTapStarTextureKey) :
	case static_cast<uint32_t>(YellowTapStarTextureKey) :
	case static_cast<uint32_t>(BlueTapStarTextureKey) :
	case static_cast<uint32_t>(OrangeTapStarTextureKey) :
#ifdef RAINBOW
	case static_cast<uint32_t>(Rainbow1TextureKey) :
	case static_cast<uint32_t>(Rainbow2TextureKey) :
	case static_cast<uint32_t>(Rainbow3TextureKey) :
	case static_cast<uint32_t>(Rainbow4TextureKey) :
	case static_cast<uint32_t>(Rainbow5TextureKey) :
	case static_cast<uint32_t>(Rainbow1HammerTextureKey) :
	case static_cast<uint32_t>(Rainbow2HammerTextureKey) :
	case static_cast<uint32_t>(Rainbow3HammerTextureKey) :
	case static_cast<uint32_t>(Rainbow4HammerTextureKey) :
	case static_cast<uint32_t>(Rainbow5HammerTextureKey) :
	case static_cast<uint32_t>(Rainbow1StarTextureKey) :
	case static_cast<uint32_t>(Rainbow2StarTextureKey) :
	case static_cast<uint32_t>(Rainbow3StarTextureKey) :
	case static_cast<uint32_t>(Rainbow4StarTextureKey) :
	case static_cast<uint32_t>(Rainbow5StarTextureKey) :
	case static_cast<uint32_t>(Rainbow1StarHammerTextureKey) :
	case static_cast<uint32_t>(Rainbow2StarHammerTextureKey) :
	case static_cast<uint32_t>(Rainbow3StarHammerTextureKey) :
	case static_cast<uint32_t>(Rainbow4StarHammerTextureKey) :
	case static_cast<uint32_t>(Rainbow5StarHammerTextureKey) :
#endif
		return YellowStarTextureKey;

	case static_cast<uint32_t>(OpenStarHammerTextureKey) :
		return YellowStarHammerTextureKey;

	case static_cast<uint32_t>(OpenStarpowerTextureKey) :
		return YellowStarpowerTextureKey;

	case static_cast<uint32_t>(OpenStarpowerHammerTextureKey) :
		return YellowStarpowerHammerTextureKey;

#if OPEN_NOTEFX
		// the magic trick
		// used in function where textures are actually applied: frankerzFix
	case static_cast<uint32_t>(OpenWhammyTextureKey) :
		return YellowTextureKey;

	case static_cast<uint32_t>(OpenWhammyStarTextureKey) :
		return YellowTextureKey;

	case static_cast<uint32_t>(OpenWhammyDeadTextureKey) :
		return YellowTextureKey;

	case static_cast<uint32_t>(OpenHitFx1TextureKey) :
	case static_cast<uint32_t>(OpenHitFx2TextureKey) :
		return GH3::QbKey(SYSTEX("sys_Particle_Star02")); // additive texture
#endif

	default:
		return GH3::QbKey(); // <---- probably explains blank texture
	}
}

bool constexpr isTextureKey(GH3::QbKey key)
{
	return	key == OpenTextureKey ||
		key == OpenHammerTextureKey ||
		key == OpenStarTextureKey ||
		key == OpenStarHammerTextureKey ||
		key == OpenStarpowerTextureKey ||
		key == OpenStarpowerHammerTextureKey
#if OPEN_NOTEFX
		|| key == OpenWhammyTextureKey ||
		key == OpenWhammyStarTextureKey ||
		key == OpenWhammyDeadTextureKey ||
		key == OpenHitFx1TextureKey ||
		key == OpenHitFx2TextureKey
#endif
		
		|| key == GreenTapTextureKey ||
		key == RedTapTextureKey ||
		key == YellowTapTextureKey ||
		key == BlueTapTextureKey ||
		key == OrangeTapTextureKey ||
		key == GreenTapStarTextureKey ||
		key == RedTapStarTextureKey ||
		key == YellowTapStarTextureKey ||
		key == BlueTapStarTextureKey ||
		key == OrangeTapStarTextureKey ||
		key == StarpowerTapTextureKey
#ifdef RAINBOW
		|| key == Rainbow1TextureKey ||
		key == Rainbow2TextureKey ||
		key == Rainbow3TextureKey ||
		key == Rainbow4TextureKey ||
		key == Rainbow5TextureKey ||
		key == Rainbow1HammerTextureKey ||
		key == Rainbow2HammerTextureKey ||
		key == Rainbow3HammerTextureKey ||
		key == Rainbow4HammerTextureKey ||
		key == Rainbow5HammerTextureKey ||
		key == Rainbow1StarTextureKey ||
		key == Rainbow2StarTextureKey ||
		key == Rainbow3StarTextureKey ||
		key == Rainbow4StarTextureKey ||
		key == Rainbow5StarTextureKey ||
		key == Rainbow1StarHammerTextureKey ||
		key == Rainbow2StarHammerTextureKey ||
		key == Rainbow3StarHammerTextureKey ||
		key == Rainbow4StarHammerTextureKey ||
		key == Rainbow5StarHammerTextureKey
#endif
		;
}

// TODO: MAKE THIS AVAILABLE IN CORE STUFF
// TODO: OR JUST USE THIS IN MUHZONES

void CopyExistingWeirdStruct(WeirdTextureStruct *dest, const WeirdTextureStruct *src, GH3::QbKey newKey, GH3::TextureMetadata *newMetadata)
{
	memcpy(dest, src, sizeof(WeirdTextureStruct));
	dest->key = newKey;
	dest->metadata = newMetadata;
}

WeirdTextureStruct * __stdcall frankerzFix(GH3::QbMap *map, GH3::QbKey *key)
{
	WeirdTextureStruct * node = reinterpret_cast<WeirdTextureStruct *>(map->Get(*key));
	if (node == nullptr && isTextureKey(*key))
	{
		GH3::QbKey yellowKey = getYellowEquivalent(*key);
		WeirdTextureStruct *yellowWeird = reinterpret_cast<WeirdTextureStruct *>(map->Get(yellowKey));
		WeirdTextureStruct *openWeird = &g_weirdMap[*key];

		GH3::TextureMetadata *metadata = g_metadataMap[*key];
		CopyExistingWeirdStruct(openWeird, yellowWeird, *key, metadata);

		map->Insert(*key, reinterpret_cast<uint32_t>(openWeird));
		node = reinterpret_cast<WeirdTextureStruct *>(map->Get(*key));
	}
	return node;
}

static void * const frankerzDetour = (void *)0x0062D9A1;
void __declspec(naked) frankerzFixNaked()
{
	static const void * const returnAddress = (void *)0x0062D9A6;
	__asm
	{
		push	ecx;
		call	frankerzFix;
		add		esp, 4;		//normal map call has an extra argument we're not using
		jmp		returnAddress;
	}
}











////////////////////////
// Store our textures //
////////////////////////

bool isRawTextureKey(GH3::QbKey key, int &i)
{
	for (i = 0; i < ArrayLength(g_rawKeys); ++i)
	{
		if (key == g_rawKeys[i])
			return true;
	}

	return false;
}

bool  __stdcall storeSpecialTextures(GH3::QbImage *img, GH3::TextureMetadata *metadata)
{
	int index;

	if (isRawTextureKey(metadata->key, index))
	{
		GH3::QbKey textureKey = g_textureKeys[index];
		g_metadataMap[textureKey] = metadata;
		g_imageMap[textureKey] = img;
		g_weirdMap[textureKey] = WeirdTextureStruct();
	}

	return false;
}


static void * const storeSpecialTexturesDetour = (void *)0x005F8AA2;
void __declspec(naked) storeSpecialTexturesNaked()
{
	static const void * const returnAddress = (void *)0x005F8AA8;
	__asm
	{
		pushad;
		push	esi; //metadata
		push	eax; //QbImage
		call	storeSpecialTextures;
		popad;

		mov		[eax + 10h], esi;
		mov     edx, [esi + 4];
		jmp		returnAddress;
	}
}
#endif

bool TryApplyTextureTakeoverPatches()
{
#if (DoTextureTakeover == 1)
	if (!g_patcher.WriteJmp(storeSpecialTexturesDetour, storeSpecialTexturesNaked) ||
		!g_patcher.WriteJmp(frankerzDetour, frankerzFixNaked))
		return false;
#endif

    return true;
}