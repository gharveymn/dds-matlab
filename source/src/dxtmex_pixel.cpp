#include "DirectXTex.h"
#include "dxtmex_maps.hpp"
#include "dxtmex_mexerror.hpp"
#include "dxtmex_pixel.hpp"

namespace DXTMEX
{
	void DXGIPixel::SetChannels(DXGI_FORMAT fmt)
	{
		/* fail if compressed, packed, video, planar, palettized or typeless */
		if(DirectX::IsCompressed(fmt))
		{
			MEXError::PrintMexError(MEU_FL,
			                        MEU_SEVERITY_USER |
			                        MEU_SEVERITY_INTERNAL,
			                        "UnsupportedFormatError",
			                        "Compressed format '%s' is not supported for this operation.",
			                        GetFormatStringFromID(fmt).c_str());
		}
		
		if(DirectX::IsPacked(fmt))
		{
			MEXError::PrintMexError(MEU_FL,
			                        MEU_SEVERITY_USER |
			                        MEU_SEVERITY_INTERNAL,
			                        "UnsupportedFormatError",
			                        "Packed format '%s' is not supported for this operation.",
			                        GetFormatStringFromID(fmt).c_str());
		}
		
		if(DirectX::IsVideo(fmt))
		{
			MEXError::PrintMexError(MEU_FL,
			                        MEU_SEVERITY_USER |
			                        MEU_SEVERITY_INTERNAL,
			                        "UnsupportedFormatError",
			                        "Video format '%s' is not supported for this operation.",
			                        GetFormatStringFromID(fmt).c_str());
		}
		
		if(DirectX::IsPlanar(fmt))
		{
			MEXError::PrintMexError(MEU_FL,
			                        MEU_SEVERITY_USER |
			                        MEU_SEVERITY_INTERNAL,
			                        "UnsupportedFormatError",
			                        "Planar format '%s' is not supported for this operation.",
			                        GetFormatStringFromID(fmt).c_str());
		}
		
		if(DirectX::IsPalettized(fmt))
		{
			MEXError::PrintMexError(MEU_FL,
			                        MEU_SEVERITY_USER |
			                        MEU_SEVERITY_INTERNAL,
			                        "UnsupportedFormatError",
			                        "Palettized format '%s' is not supported for this operation.",
			                        GetFormatStringFromID(fmt).c_str());
		}
		
		if(DirectX::IsTypeless(fmt))
		{
			MEXError::PrintMexError(MEU_FL,
			                        MEU_SEVERITY_USER |
			                        MEU_SEVERITY_INTERNAL,
			                        "UnsupportedFormatError",
			                        "Typeless format '%s' is not supported for this operation.",
			                        GetFormatStringFromID(fmt).c_str());
		}
		
		switch(fmt)
		{
			case DXGI_FORMAT_R32G32B32A32_FLOAT:
			{
				this->_num_channels = 4;
				this->_channels[0] = {32,  0, 0, FLOAT, 'R'};
				this->_channels[1] = {32, 32, 1, FLOAT, 'G'};
				this->_channels[2] = {32, 64, 2, FLOAT, 'B'};
				this->_channels[3] = {32, 96, 3, FLOAT, 'A'};
				break;
			}
			case DXGI_FORMAT_R32G32B32A32_UINT:
			{
				this->_num_channels = 4;
				this->_channels[0] = {32,  0, 0, UINT, 'R'};
				this->_channels[1] = {32, 32, 1, UINT, 'G'};
				this->_channels[2] = {32, 64, 2, UINT, 'B'};
				this->_channels[3] = {32, 96, 3, UINT, 'A'};
				break;
			}
			case DXGI_FORMAT_R32G32B32A32_SINT:
			{
				this->_num_channels = 4;
				this->_channels[0] = {32,  0, 0, SINT, 'R'};
				this->_channels[1] = {32, 32, 1, SINT, 'G'};
				this->_channels[2] = {32, 64, 2, SINT, 'B'};
				this->_channels[3] = {32, 96, 3, SINT, 'A'};
				break;
			}
			case DXGI_FORMAT_R32G32B32_FLOAT:
			{
				this->_num_channels = 3;
				this->_channels[0] = {32,  0, 0, FLOAT, 'R'};
				this->_channels[1] = {32, 32, 1, FLOAT, 'G'};
				this->_channels[2] = {32, 64, 2, FLOAT, 'B'};
				break;
			}
			case DXGI_FORMAT_R32G32B32_UINT:
			{
				this->_num_channels = 3;
				this->_channels[0] = {32,  0, 0, UINT, 'R'};
				this->_channels[1] = {32, 32, 1, UINT, 'G'};
				this->_channels[2] = {32, 64, 2, UINT, 'B'};
				break;
			}
			case DXGI_FORMAT_R32G32B32_SINT:
			{
				this->_num_channels = 3;
				this->_channels[0] = {32,  0, 0, SINT, 'R'};
				this->_channels[1] = {32, 32, 1, SINT, 'G'};
				this->_channels[2] = {32, 64, 2, SINT, 'B'};
				break;
			}
			case DXGI_FORMAT_R16G16B16A16_FLOAT:
			{
				this->_num_channels = 4;
				this->_channels[0] = {16,  0, 0, FLOAT, 'R'};
				this->_channels[1] = {16, 16, 1, FLOAT, 'G'};
				this->_channels[2] = {16, 32, 2, FLOAT, 'B'};
				this->_channels[3] = {16, 48, 3, FLOAT, 'A'};
				break;
			}
			case DXGI_FORMAT_R16G16B16A16_UNORM:
			{
				this->_num_channels = 4;
				this->_channels[0] = {16,  0, 0, UNORM, 'R'};
				this->_channels[1] = {16, 16, 1, UNORM, 'G'};
				this->_channels[2] = {16, 32, 2, UNORM, 'B'};
				this->_channels[3] = {16, 48, 3, UNORM, 'A'};
				break;
			}
			case DXGI_FORMAT_R16G16B16A16_UINT:
			{
				this->_num_channels = 4;
				this->_channels[0] = {16,  0, 0, UINT, 'R'};
				this->_channels[1] = {16, 16, 1, UINT, 'G'};
				this->_channels[2] = {16, 32, 2, UINT, 'B'};
				this->_channels[3] = {16, 48, 3, UINT, 'A'};
				break;
			}
			case DXGI_FORMAT_R16G16B16A16_SNORM:
			{
				this->_num_channels = 4;
				this->_channels[0] = {16,  0, 0, SNORM, 'R'};
				this->_channels[1] = {16, 16, 1, SNORM, 'G'};
				this->_channels[2] = {16, 32, 2, SNORM, 'B'};
				this->_channels[3] = {16, 48, 3, SNORM, 'A'};
				break;
			}
			case DXGI_FORMAT_R16G16B16A16_SINT:
			{
				this->_num_channels = 4;
				this->_channels[0] = {16,  0, 0, SINT, 'R'};
				this->_channels[1] = {16, 16, 1, SINT, 'G'};
				this->_channels[2] = {16, 32, 2, SINT, 'B'};
				this->_channels[3] = {16, 48, 3, SINT, 'A'};
				break;
			}
			case DXGI_FORMAT_R32G32_FLOAT:
			{
				this->_num_channels = 2;
				this->_channels[0] = {32,  0, 0, FLOAT, 'R'};
				this->_channels[1] = {32, 32, 1, FLOAT, 'G'};
				break;
			}
			case DXGI_FORMAT_R32G32_UINT:
			{
				this->_num_channels = 2;
				this->_channels[0] = {32,  0, 0, UINT, 'R'};
				this->_channels[1] = {32, 32, 1, UINT, 'G'};
				break;
			}
			case DXGI_FORMAT_R32G32_SINT:
			{
				this->_num_channels = 2;
				this->_channels[0] = {32,  0, 0, SINT, 'R'};
				this->_channels[1] = {32, 32, 1, SINT, 'G'};
				break;
			}
			case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
			{
				this->_num_channels = 2;
				this->_channels[0] = {32,  0, 0, FLOAT, 'D'};
				this->_channels[1] = {8,  32, 1, UINT,  'S'};
				break;
			}
			case DXGI_FORMAT_R10G10B10A2_UNORM:
			{
				this->_num_channels = 4;
				this->_channels[0] = {10,  0, 0, UNORM, 'R'};
				this->_channels[1] = {10, 10, 1, UNORM, 'G'};
				this->_channels[2] = {10, 20, 2, UNORM, 'B'};
				this->_channels[3] = { 2, 30, 3, UNORM, 'A'};
				break;
			}
			case DXGI_FORMAT_R10G10B10A2_UINT:
			{
				this->_num_channels = 4;
				this->_channels[0] = {10,  0, 0, UINT, 'R'};
				this->_channels[1] = {10, 10, 1, UINT, 'G'};
				this->_channels[2] = {10, 20, 2, UINT, 'B'};
				this->_channels[3] = { 2, 30, 3, UINT, 'A'};
				break;
			}
			case DXGI_FORMAT_R11G11B10_FLOAT:
			{
				this->_num_channels = 3;
				this->_channels[0] = {11,  0, 0, FLOAT, 'R'};
				this->_channels[1] = {11, 11, 1, FLOAT, 'G'};
				this->_channels[2] = {10, 22, 2, FLOAT, 'B'};
				break;
			}
			case DXGI_FORMAT_R8G8B8A8_UNORM:
			{
				this->_num_channels = 4;
				this->_channels[0] = {8,  0, 0, UNORM, 'R'};
				this->_channels[1] = {8,  8, 1, UNORM, 'G'};
				this->_channels[2] = {8, 16, 2, UNORM, 'B'};
				this->_channels[3] = {8, 24, 3, UNORM, 'A'};
				break;
			}
			case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
			{
				this->_num_channels = 4;
				this->_channels[0] = {8,  0, 0, SRGB, 'R'};
				this->_channels[1] = {8,  8, 1, SRGB, 'G'};
				this->_channels[2] = {8, 16, 2, SRGB, 'B'};
				this->_channels[3] = {8, 24, 3, SRGB, 'A'};
				break;
			}
			case DXGI_FORMAT_R8G8B8A8_UINT:
			{
				this->_num_channels = 4;
				this->_channels[0] = {8,  0, 0, UINT, 'R'};
				this->_channels[1] = {8,  8, 1, UINT, 'G'};
				this->_channels[2] = {8, 16, 2, UINT, 'B'};
				this->_channels[3] = {8, 24, 3, UINT, 'A'};
				break;
			}
			case DXGI_FORMAT_R8G8B8A8_SNORM:
			{
				this->_num_channels = 4;
				this->_channels[0] = {8,  0, 0, SNORM, 'R'};
				this->_channels[1] = {8,  8, 1, SNORM, 'G'};
				this->_channels[2] = {8, 16, 2, SNORM, 'B'};
				this->_channels[3] = {8, 24, 3, SNORM, 'A'};
				break;
			}
			case DXGI_FORMAT_R8G8B8A8_SINT:
			{
				this->_num_channels = 4;
				this->_channels[0] = {8,  0, 0, SINT, 'R'};
				this->_channels[1] = {8,  8, 1, SINT, 'G'};
				this->_channels[2] = {8, 16, 2, SINT, 'B'};
				this->_channels[3] = {8, 24, 3, SINT, 'A'};
				break;
			}
			case DXGI_FORMAT_R16G16_FLOAT:
			{
				this->_num_channels = 2;
				this->_channels[0] = {16,  0, 0, FLOAT, 'R'};
				this->_channels[1] = {16, 16, 1, FLOAT, 'G'};
				break;
			}
			case DXGI_FORMAT_R16G16_UNORM:
			{
				this->_num_channels = 2;
				this->_channels[0] = {16,  0, 0, UNORM, 'R'};
				this->_channels[1] = {16, 16, 1, UNORM, 'G'};
				break;
			}
			case DXGI_FORMAT_R16G16_UINT:
			{
				this->_num_channels = 2;
				this->_channels[0] = {16,  0, 0, UINT, 'R'};
				this->_channels[1] = {16, 16, 1, UINT, 'G'};
				break;
			}
			case DXGI_FORMAT_R16G16_SNORM:
			{
				this->_num_channels = 2;
				this->_channels[0] = {16,  0, 0, SNORM, 'R'};
				this->_channels[1] = {16, 16, 1, SNORM, 'G'};
				break;
			}
			case DXGI_FORMAT_R16G16_SINT:
			{
				this->_num_channels = 2;
				this->_channels[0] = {16,  0, 0, SINT, 'R'};
				this->_channels[1] = {16, 16, 1, SINT, 'G'};
				break;
			}
			case DXGI_FORMAT_D32_FLOAT:
			{
				this->_num_channels = 1;
				this->_channels[0] = {32, 0, 0, FLOAT, 'D'};
				break;
			}
			case DXGI_FORMAT_R32_FLOAT:
			{
				this->_num_channels = 1;
				this->_channels[0] = {32, 0, 0, FLOAT, 'R'};
				break;
			}
			case DXGI_FORMAT_R32_UINT:
			{
				this->_num_channels = 1;
				this->_channels[0] = {32, 0, 0, UINT, 'R'};
				break;
			}
			case DXGI_FORMAT_R32_SINT:
			{
				this->_num_channels = 1;
				this->_channels[0] = {32, 0, 0, SINT, 'R'};
				break;
			}
			case DXGI_FORMAT_D24_UNORM_S8_UINT:
			{
				this->_num_channels = 2;
				this->_channels[0] = {24, 0, 0, UNORM, 'D'};
				this->_channels[1] = {8, 24, 1,  UINT, 'S'};
				break;
			}
			case DXGI_FORMAT_R8G8_UNORM:
			{
				this->_num_channels = 2;
				this->_channels[0] = {8, 0, 0, UNORM, 'R'};
				this->_channels[1] = {8, 8, 1, UNORM, 'G'};
				break;
			}
			case DXGI_FORMAT_R8G8_UINT:
			{
				this->_num_channels = 2;
				this->_channels[0] = {8, 0, 0, UINT, 'R'};
				this->_channels[1] = {8, 8, 1, UINT, 'G'};
				break;
			}
			case DXGI_FORMAT_R8G8_SNORM:
			{
				this->_num_channels = 2;
				this->_channels[0] = {8, 0, 0, SNORM, 'R'};
				this->_channels[1] = {8, 8, 1, SNORM, 'G'};
				break;
			}
			case DXGI_FORMAT_R8G8_SINT:
			{
				this->_num_channels = 2;
				this->_channels[0] = {8, 0, 0, SINT, 'R'};
				this->_channels[1] = {8, 8, 1, SINT, 'G'};
				break;
			}
			case DXGI_FORMAT_R16_FLOAT:
			{
				this->_num_channels = 1;
				this->_channels[0] = {16, 0, 0, FLOAT, 'R'};
				break;
			}
			case DXGI_FORMAT_D16_UNORM:
			{
				this->_num_channels = 1;
				this->_channels[0] = {16, 0, 0, UNORM, 'D'};
				break;
			}
			case DXGI_FORMAT_R16_UNORM:
			{
				this->_num_channels = 1;
				this->_channels[0] = {16, 0, 0, UNORM, 'R'};
				break;
			}
			case DXGI_FORMAT_R16_UINT:
			{
				this->_num_channels = 1;
				this->_channels[0] = {16, 0, 0, UNORM, 'R'};
				break;
			}
			case DXGI_FORMAT_R16_SNORM:
			{
				this->_num_channels = 1;
				this->_channels[0] = {16, 0, 0, SNORM, 'R'};
				break;
			}
			case DXGI_FORMAT_R16_SINT:
			{
				this->_num_channels = 1;
				this->_channels[0] = {16, 0, 0, SINT, 'R'};
				break;
			}
			case DXGI_FORMAT_R8_UNORM:
			{
				this->_num_channels = 1;
				this->_channels[0] = {8, 0, 0, UNORM, 'R'};
				break;
			}
			case DXGI_FORMAT_R8_UINT:
			{
				this->_num_channels = 1;
				this->_channels[0] = {8, 0, 0, UINT, 'R'};
				break;
			}
			case DXGI_FORMAT_R8_SNORM:
			{
				this->_num_channels = 1;
				this->_channels[0] = {8, 0, 0, SNORM, 'R'};
				break;
			}
			case DXGI_FORMAT_R8_SINT:
			{
				this->_num_channels = 1;
				this->_channels[0] = {8, 0, 0, SINT, 'R'};
				break;
			}
			case DXGI_FORMAT_A8_UNORM:
			{
				this->_num_channels = 1;
				this->_channels[0] = {8, 0, 0, UNORM, 'A'};
				break;
			}
			case DXGI_FORMAT_R1_UNORM:
			{
				this->_num_channels = 1;
				this->_channels[0] = {1, 0, 0, UNORM, 'R'};
				break;
			}
			case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
			{
				this->_num_channels = 4;
				this->_channels[0] = {9,  0, 0, SHAREDEXP, 'R'};
				this->_channels[1] = {9,  9, 1, SHAREDEXP, 'G'};
				this->_channels[2] = {9, 18, 2, SHAREDEXP, 'B'};
				this->_channels[3] = {5, 27, 3, SHAREDEXP, 'E'};
				break;
			}
			case DXGI_FORMAT_B5G6R5_UNORM:
			{
				this->_num_channels = 3;
				this->_channels[0] = {5,  0, 2, UNORM, 'B'};
				this->_channels[1] = {6,  5, 1, UNORM, 'G'};
				this->_channels[2] = {5, 11, 0, UNORM, 'R'};
				break;
			}
			case DXGI_FORMAT_B5G5R5A1_UNORM:
			{
				this->_num_channels = 4;
				this->_channels[0] = {5,  0, 2, UNORM, 'B'};
				this->_channels[1] = {5,  5, 1, UNORM, 'G'};
				this->_channels[2] = {5, 10, 0, UNORM, 'R'};
				this->_channels[3] = {1, 15, 3, UNORM, 'A'};
				break;
			}
			case DXGI_FORMAT_B8G8R8A8_UNORM:
			{
				this->_num_channels = 4;
				this->_channels[0] = {8,  0, 2, UNORM, 'B'};
				this->_channels[1] = {8,  8, 1, UNORM, 'G'};
				this->_channels[2] = {8, 16, 0, UNORM, 'R'};
				this->_channels[3] = {8, 24, 3, UNORM, 'A'};
				break;
			}
			case DXGI_FORMAT_B8G8R8X8_UNORM:
			{
				this->_num_channels = 3;
				this->_channels[0] = {8,  0, 2, UNORM, 'B'};
				this->_channels[1] = {8,  8, 1, UNORM, 'G'};
				this->_channels[2] = {8, 16, 0, UNORM, 'R'};
				break;
			}
			case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
			{
				this->_num_channels = 4;
				this->_channels[0] = {10,  0, 0, XR_BIAS, 'R'};
				this->_channels[1] = {10, 10, 1, XR_BIAS, 'G'};
				this->_channels[2] = {10, 20, 2, XR_BIAS, 'B'};
				this->_channels[3] = { 2, 30, 3, UNORM,   'A'};
				break;
			}
			case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
			{
				this->_num_channels = 4;
				this->_channels[0] = {8,  0, 2, SRGB, 'B'};
				this->_channels[1] = {8,  8, 1, SRGB, 'G'};
				this->_channels[2] = {8, 16, 0, SRGB, 'R'};
				this->_channels[3] = {8, 24, 3, SRGB, 'A'};
				break;
			}
			case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
			{
				this->_num_channels = 3;
				this->_channels[0] = {8,  0, 2, SRGB, 'B'};
				this->_channels[1] = {8,  8, 1, SRGB, 'G'};
				this->_channels[2] = {8, 16, 0, SRGB, 'R'};
				break;
			}
			case DXGI_FORMAT_B4G4R4A4_UNORM:
			{
				this->_num_channels = 4;
				this->_channels[0] = {4,  0, 2, UNORM, 'B'};
				this->_channels[1] = {4,  4, 1, UNORM, 'G'};
				this->_channels[2] = {4,  8, 0, UNORM, 'R'};
				this->_channels[3] = {4, 12, 3, UNORM, 'A'};
			}
			case DXGI_FORMAT_BC1_TYPELESS:               /* COMPRESSED */
			case DXGI_FORMAT_BC1_UNORM:
			case DXGI_FORMAT_BC1_UNORM_SRGB:
			case DXGI_FORMAT_BC2_TYPELESS:
			case DXGI_FORMAT_BC2_UNORM:
			case DXGI_FORMAT_BC2_UNORM_SRGB:
			case DXGI_FORMAT_BC3_TYPELESS:
			case DXGI_FORMAT_BC3_UNORM:
			case DXGI_FORMAT_BC3_UNORM_SRGB:
			case DXGI_FORMAT_BC4_TYPELESS:
			case DXGI_FORMAT_BC4_UNORM:
			case DXGI_FORMAT_BC4_SNORM:
			case DXGI_FORMAT_BC5_TYPELESS:
			case DXGI_FORMAT_BC5_UNORM:
			case DXGI_FORMAT_BC5_SNORM:
			case DXGI_FORMAT_BC6H_TYPELESS:
			case DXGI_FORMAT_BC6H_UF16:
			case DXGI_FORMAT_BC6H_SF16:
			case DXGI_FORMAT_BC7_TYPELESS:
			case DXGI_FORMAT_BC7_UNORM:
			case DXGI_FORMAT_BC7_UNORM_SRGB:
			
			case DXGI_FORMAT_R8G8_B8G8_UNORM:            /* PACKED */
			case DXGI_FORMAT_G8R8_G8B8_UNORM:
			case DXGI_FORMAT_YUY2:
			case DXGI_FORMAT_Y210:
			case DXGI_FORMAT_Y216:
			
			case DXGI_FORMAT_AYUV:                       /* VIDEO */
			case DXGI_FORMAT_Y410:
			case DXGI_FORMAT_Y416:
			case DXGI_FORMAT_NV12:
			case DXGI_FORMAT_P010:
			case DXGI_FORMAT_P016:
			case DXGI_FORMAT_NV11:
			case DXGI_FORMAT_420_OPAQUE:
			case DXGI_FORMAT_AI44:
			case DXGI_FORMAT_IA44:
			case DXGI_FORMAT_P8:
			case DXGI_FORMAT_A8P8:
			
			case DXGI_FORMAT_P208:                       /* UNDOCUMENTED BY DIRECTXTEX, PROBABLY VIDEO */
			case DXGI_FORMAT_V208:
			case DXGI_FORMAT_V408:
			
			case DXGI_FORMAT_R32G32B32A32_TYPELESS:      /* TYPELESS */
			case DXGI_FORMAT_R32G32B32_TYPELESS:
			case DXGI_FORMAT_R16G16B16A16_TYPELESS:
			case DXGI_FORMAT_R32G32_TYPELESS:
			case DXGI_FORMAT_R32G8X24_TYPELESS:
			case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
			case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
			case DXGI_FORMAT_R10G10B10A2_TYPELESS:
			case DXGI_FORMAT_R8G8B8A8_TYPELESS:
			case DXGI_FORMAT_R16G16_TYPELESS:
			case DXGI_FORMAT_R32_TYPELESS:
			case DXGI_FORMAT_R24G8_TYPELESS:
			case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
			case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
			case DXGI_FORMAT_R8G8_TYPELESS:
			case DXGI_FORMAT_R16_TYPELESS:
			case DXGI_FORMAT_R8_TYPELESS:
			case DXGI_FORMAT_B8G8R8A8_TYPELESS:
			case DXGI_FORMAT_B8G8R8X8_TYPELESS:
			
			case DXGI_FORMAT_UNKNOWN:
			default:
			{
				MEXError::PrintMexError(MEU_FL,
				                        MEU_SEVERITY_INTERNAL,
				                        "UnsupportedFormatError",
				                        "Unsupported format '%s' is not supported for this operation.",
				                        GetFormatStringFromID(fmt).c_str());
			}
		}
	}
	
	template <typename T>
	T DXGIPixel::ExtractPixelChannel(size_t idx)
	{
	
	}
}

