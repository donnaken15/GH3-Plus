#pragma once
#include <d3d9types.h>
#define DDSF_FOURCC 0x00000004l

//#ifndef MAKEFOURCC // wtf
#define MAKEFOURCC(ch0, ch1, ch2, ch3)										\
				((unsigned int)(unsigned char)(ch0) | ((unsigned int)(unsigned char)(ch1) << 8) |	\
				((unsigned int)(unsigned char)(ch2) << 16) | ((unsigned int)(unsigned char)(ch3) << 24 ))
//#endif //defined(MAKEFOURCC)
//#define FOURCC_DXT1(MAKEFOURCC('D','X','T','1')) // why
//#define FOURCC_DXT5(MAKEFOURCC('D','X','T','5'))

typedef struct _DDCOLORKEY
{
	unsigned int		 dwColorSpaceLowValue;
	unsigned int		 dwColorSpaceHighValue;
} DDCOLORKEY;

typedef struct _DDPIXELFORMAT
{
	unsigned int		 dwSize;					// size of structure
	unsigned int		 dwFlags;					 // pixel format flags
	unsigned int		 dwFourCC;					// (FOURCC code)
	union
	{
		unsigned int	dwRGBBitCount;			 // how many bits per pixel
		unsigned int	dwYUVBitCount;			 // how many bits per pixel
		unsigned int	dwZBufferBitDepth;		// how many total bits/pixel in z buffer (including any stencil bits)
		unsigned int	dwAlphaBitDepth;		// how many bits for alpha channels
		unsigned int	dwLuminanceBitCount;	 // how many bits per pixel
		unsigned int	dwBumpBitCount;			// how many bits per "buxel", total
		unsigned int	dwPrivateFormatBitCount;// Bits per pixel of private driver formats. Only valid in texture format list and if DDPF_D3DFORMAT is set
	};
	union
	{
		unsigned int	dwRBitMask;				 // mask for red bit
		unsigned int	dwYBitMask;				 // mask for Y bits
		unsigned int	dwStencilBitDepth;		// how many stencil bits (note: dwZBufferBitDepth-dwStencilBitDepth is total Z-only bits)
		unsigned int	dwLuminanceBitMask;	// mask for luminance bits
		unsigned int	dwBumpDuBitMask;		// mask for bump map U delta bits
		unsigned int	dwOperations;			// DDPF_D3DFORMAT Operations
	};
	union
	{
		unsigned int	dwGBitMask;				 // mask for green bits
		unsigned int	dwUBitMask;				 // mask for U bits
		unsigned int	dwZBitMask;				 // mask for Z bits
		unsigned int	dwBumpDvBitMask;		// mask for bump map V delta bits
		struct
		{
			unsigned short	 wFlipMSTypes;		 // Multisample methods supported via flip for this D3DFORMAT
			unsigned short	 wBltMSTypes;		// Multisample methods supported via blt for this D3DFORMAT
		} MultiSampleCaps;

	};
	union
	{
		unsigned int	dwBBitMask;				 // mask for blue bits
		unsigned int	dwVBitMask;				 // mask for V bits
		unsigned int	dwStencilBitMask;		 // mask for stencil bits
		unsigned int	dwBumpLuminanceBitMask; // mask for luminance in bump map
	};
	union
	{
		unsigned int	dwRGBAlphaBitMask;		// mask for alpha channel
		unsigned int	dwYUVAlphaBitMask;		// mask for alpha channel
		unsigned int	dwLuminanceAlphaBitMask;// mask for alpha channel
		unsigned int	dwRGBZBitMask;			 // mask for Z channel
		unsigned int	dwYUVZBitMask;			 // mask for Z channel
	};
} DDPIXELFORMAT;
typedef struct _DDSURFACEDESC2
{
	unsigned int			dwMagic;
	unsigned int				dwSize;
	unsigned int				dwFlags;
	unsigned int				dwHeight;
	unsigned int				dwWidth;
	union
	{
		int					lPitch;
		unsigned int		dwLinearSize;
	};
	union
	{
		unsigned int		dwBackBufferCount;
		unsigned int		dwDepth;
	};
	union
	{
		unsigned int		dwMipMapCount;
		unsigned int		dwRefreshRate;
		unsigned int		dwSrcVBHandle;
	};
	unsigned int					dwAlphaBitDepth;
	unsigned int					dwReserved;
	unsigned int					lpSurface;
	union
	{
		DDCOLORKEY				ddckCKDestOverlay;
		unsigned int			dwEmptyFaceColor;
	};
	DDCOLORKEY					ddckCKDestBlt;
	DDCOLORKEY					ddckCKSrcOverlay;
	DDCOLORKEY					ddckCKSrcBlt;
	union
	{
		DDPIXELFORMAT			ddpfPixelFormat;
		unsigned int			dwFVF;
	};

	struct _DDSCAPS2
	{
		unsigned int		 dwCaps;
		unsigned int		 dwCaps2;
		unsigned int		 dwCaps3;
		union
		{
			unsigned int		 dwCaps4;
			unsigned int		 dwVolumeDepth;
		};
	} ddsCaps;// direct draw surface capabilities

	unsigned int					dwTextureStage;			// stage in multitexture cascade
} DDSURFACEDESC2;