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
		
		switch(fmt)
		{
			case DXGI_FORMAT_R32G32B32A32_TYPELESS:
			{
				this->_num_channels = 4;
				this->_channels[0] = {32, 0,  0, DXGIPixel::TYPELESS, 'R'};
				this->_channels[1] = {32, 32, 1, DXGIPixel::TYPELESS, 'G'};
				this->_channels[2] = {32, 64, 2, DXGIPixel::TYPELESS, 'B'};
				this->_channels[3] = {32, 96, 3, DXGIPixel::TYPELESS, 'A'};
				break;
			}
			case DXGI_FORMAT_R32G32B32A32_FLOAT: // XMFLOAT4A
			{
				this->_num_channels = 4;
				this->_channels[0] = {32, 0,  0, DXGIPixel::FLOAT, 'R'};
				this->_channels[1] = {32, 32, 1, DXGIPixel::FLOAT, 'G'};
				this->_channels[2] = {32, 64, 2, DXGIPixel::FLOAT, 'B'};
				this->_channels[3] = {32, 96, 3, DXGIPixel::FLOAT, 'A'};
				break;
			}
			case DXGI_FORMAT_R32G32B32A32_UINT: // XMUINT4
			{
				this->_num_channels = 4;
				this->_channels[0] = {32, 0,  0, DXGIPixel::UINT, 'R'};
				this->_channels[1] = {32, 32, 1, DXGIPixel::UINT, 'G'};
				this->_channels[2] = {32, 64, 2, DXGIPixel::UINT, 'B'};
				this->_channels[3] = {32, 96, 3, DXGIPixel::UINT, 'A'};
				break;
			}
			case DXGI_FORMAT_R32G32B32A32_SINT: // XMINT4
			{
				this->_num_channels = 4;
				this->_channels[0] = {32, 0,  0, DXGIPixel::SINT, 'R'};
				this->_channels[1] = {32, 32, 1, DXGIPixel::SINT, 'G'};
				this->_channels[2] = {32, 64, 2, DXGIPixel::SINT, 'B'};
				this->_channels[3] = {32, 96, 3, DXGIPixel::SINT, 'A'};
				break;
			}
			case DXGI_FORMAT_R32G32B32_TYPELESS:
			{
				this->_num_channels = 3;
				this->_channels[0] = {32, 0,  0, DXGIPixel::TYPELESS, 'R'};
				this->_channels[1] = {32, 32, 1, DXGIPixel::TYPELESS, 'G'};
				this->_channels[2] = {32, 64, 2, DXGIPixel::TYPELESS, 'B'};
				break;
			}
			case DXGI_FORMAT_R32G32B32_FLOAT: //XMFLOAT3 or XMFLOAT3A
			{
				this->_num_channels = 3;
				this->_channels[0] = {32, 0,  0, DXGIPixel::FLOAT, 'R'};
				this->_channels[1] = {32, 32, 1, DXGIPixel::FLOAT, 'G'};
				this->_channels[2] = {32, 64, 2, DXGIPixel::FLOAT, 'B'};
				break;
			}
			case DXGI_FORMAT_R32G32B32_UINT: // XMUINT3
			{
				this->_num_channels = 3;
				this->_channels[0] = {32, 0,  0, DXGIPixel::UINT, 'R'};
				this->_channels[1] = {32, 32, 1, DXGIPixel::UINT, 'G'};
				this->_channels[2] = {32, 64, 2, DXGIPixel::UINT, 'B'};
				break;
			}
			case DXGI_FORMAT_R32G32B32_SINT: // XMINT3
			{
				this->_num_channels = 3;
				this->_channels[0] = {32, 0,  0, DXGIPixel::SINT, 'R'};
				this->_channels[1] = {32, 32, 1, DXGIPixel::SINT, 'G'};
				this->_channels[2] = {32, 64, 2, DXGIPixel::SINT, 'B'};
				break;
			}
			case DXGI_FORMAT_R16G16B16A16_TYPELESS:
			{
				this->_num_channels = 4;
				this->_channels[0] = {16, 0,  0, DXGIPixel::TYPELESS, 'R'};
				this->_channels[1] = {16, 16, 1, DXGIPixel::TYPELESS, 'G'};
				this->_channels[2] = {16, 32, 2, DXGIPixel::TYPELESS, 'B'};
				this->_channels[3] = {16, 48, 3, DXGIPixel::TYPELESS, 'A'};
				break;
			}
			case DXGI_FORMAT_R16G16B16A16_FLOAT: // XMHALF4
			{
				this->_num_channels = 4;
				this->_channels[0] = {16, 0,  0, DXGIPixel::FLOAT, 'R'};
				this->_channels[1] = {16, 16, 1, DXGIPixel::FLOAT, 'G'};
				this->_channels[2] = {16, 32, 2, DXGIPixel::FLOAT, 'B'};
				this->_channels[3] = {16, 48, 3, DXGIPixel::FLOAT, 'A'};
				break;
			}
			case DXGI_FORMAT_R16G16B16A16_UNORM: // XMUSHORTN4
			{
				this->_num_channels = 4;
				this->_channels[0] = {16, 0,  0, DXGIPixel::UNORM, 'R'};
				this->_channels[1] = {16, 16, 1, DXGIPixel::UNORM, 'G'};
				this->_channels[2] = {16, 32, 2, DXGIPixel::UNORM, 'B'};
				this->_channels[3] = {16, 48, 3, DXGIPixel::UNORM, 'A'};
				break;
			}
			case DXGI_FORMAT_R16G16B16A16_UINT: // XMUSHORT4
			{
				this->_num_channels = 4;
				this->_channels[0] = {16, 0,  0, DXGIPixel::UINT, 'R'};
				this->_channels[1] = {16, 16, 1, DXGIPixel::UINT, 'G'};
				this->_channels[2] = {16, 32, 2, DXGIPixel::UINT, 'B'};
				this->_channels[3] = {16, 48, 3, DXGIPixel::UINT, 'A'};
				break;
			}
			case DXGI_FORMAT_R16G16B16A16_SNORM: // XMSHORTN4
			{
				this->_num_channels = 4;
				this->_channels[0] = {16, 0,  0, DXGIPixel::SNORM, 'R'};
				this->_channels[1] = {16, 16, 1, DXGIPixel::SNORM, 'G'};
				this->_channels[2] = {16, 32, 2, DXGIPixel::SNORM, 'B'};
				this->_channels[3] = {16, 48, 3, DXGIPixel::SNORM, 'A'};
				break;
			}
			case DXGI_FORMAT_R16G16B16A16_SINT: // XMSHORT4
			{
				this->_num_channels = 4;
				this->_channels[0] = {16, 0,  0, DXGIPixel::SINT, 'R'};
				this->_channels[1] = {16, 16, 1, DXGIPixel::SINT, 'G'};
				this->_channels[2] = {16, 32, 2, DXGIPixel::SINT, 'B'};
				this->_channels[3] = {16, 48, 3, DXGIPixel::SINT, 'A'};
				break;
			}
			case DXGI_FORMAT_R32G32_TYPELESS:
			{
				this->_num_channels = 2;
				this->_channels[0] = {32, 0,  0, DXGIPixel::TYPELESS, 'R'};
				this->_channels[1] = {32, 32, 1, DXGIPixel::TYPELESS, 'G'};
				break;
			}
			case DXGI_FORMAT_R32G32_FLOAT: // XMFLOAT2 or XMFLOAT2A
			{
				this->_num_channels = 2;
				this->_channels[0] = {32, 0,  0, DXGIPixel::FLOAT, 'R'};
				this->_channels[1] = {32, 32, 1, DXGIPixel::FLOAT, 'G'};
				break;
			}
			case DXGI_FORMAT_R32G32_UINT: // XMUINT2
			{
				this->_num_channels = 2;
				this->_channels[0] = {32, 0,  0, DXGIPixel::UINT, 'R'};
				this->_channels[1] = {32, 32, 1, DXGIPixel::UINT, 'G'};
				break;
			}
			case DXGI_FORMAT_R32G32_SINT: // XMINT2
			{
				this->_num_channels = 2;
				this->_channels[0] = {32, 0,  0, DXGIPixel::SINT, 'R'};
				this->_channels[1] = {32, 32, 1, DXGIPixel::SINT, 'G'};
				break;
			}
			case DXGI_FORMAT_R32G8X24_TYPELESS:
			{
				this->_num_channels = 2;
				this->_channels[0] = {32, 0, 0, DXGIPixel::TYPELESS, 'R'};
				this->_channels[1] = {8, 32, 1, DXGIPixel::TYPELESS, 'G'};
				break;
			}
			case DXGI_FORMAT_D32_FLOAT_S8X24_UINT: // UNSUPPORTED
			{
				this->_num_channels = 2;
				this->_channels[0] = {32, 0, 0, DXGIPixel::FLOAT, 'D'};
				this->_channels[1] = {8, 32, 1, DXGIPixel::UINT,  'S'};
				break;
			}
			case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
			{
				this->_num_channels = 1;
				this->_channels[0] = {32, 0, 0, DXGIPixel::FLOAT, 'R'};
				break;
			}
			case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
			{
				this->_num_channels = 1;
				this->_channels[0] = {8, 32, 1, DXGIPixel::UINT, 'G'};
				break;
			}
			case DXGI_FORMAT_R10G10B10A2_TYPELESS:
			{
				this->_num_channels = 4;
				this->_channels[0] = {10, 0,  0, DXGIPixel::TYPELESS, 'R'};
				this->_channels[1] = {10, 10, 1, DXGIPixel::TYPELESS, 'G'};
				this->_channels[2] = {10, 20, 2, DXGIPixel::TYPELESS, 'B'};
				this->_channels[3] = {2,  30, 3, DXGIPixel::TYPELESS, 'A'};
				break;
			}
			case DXGI_FORMAT_R10G10B10A2_UNORM: // XMUDECN4
			{
				this->_num_channels = 4;
				this->_channels[0] = {10, 0,  0, DXGIPixel::UNORM, 'R'};
				this->_channels[1] = {10, 10, 1, DXGIPixel::UNORM, 'G'};
				this->_channels[2] = {10, 20, 2, DXGIPixel::UNORM, 'B'};
				this->_channels[3] = {2,  30, 3, DXGIPixel::UNORM, 'A'};
				break;
			}
			case DXGI_FORMAT_R10G10B10A2_UINT: // XMUDEC4
			{
				this->_num_channels = 4;
				this->_channels[0] = {10, 0,  0, DXGIPixel::UINT, 'R'};
				this->_channels[1] = {10, 10, 1, DXGIPixel::UINT, 'G'};
				this->_channels[2] = {10, 20, 2, DXGIPixel::UINT, 'B'};
				this->_channels[3] = {2,  30, 3, DXGIPixel::UINT, 'A'};
				break;
			}
			case DXGI_FORMAT_R11G11B10_FLOAT: // XMFLOAT3PK
			{
				this->_num_channels = 3;
				this->_channels[0] = {11, 0,  0, DXGIPixel::FLOAT, 'R'};
				this->_channels[1] = {11, 11, 1, DXGIPixel::FLOAT, 'G'};
				this->_channels[2] = {10, 22, 2, DXGIPixel::FLOAT, 'B'};
				break;
			}
			case DXGI_FORMAT_R8G8B8A8_TYPELESS:
			{
				this->_num_channels = 4;
				this->_channels[0] = {8,  0, 0, DXGIPixel::TYPELESS, 'R'};
				this->_channels[1] = {8,  8, 1, DXGIPixel::TYPELESS, 'G'};
				this->_channels[2] = {8, 16, 2, DXGIPixel::TYPELESS, 'B'};
				this->_channels[3] = {8, 24, 3, DXGIPixel::TYPELESS, 'A'};
				break;
			}
			case DXGI_FORMAT_R8G8B8A8_UNORM: // XMUBYTEN4
			{
				this->_num_channels = 4;
				this->_channels[0] = {8,  0, 0, DXGIPixel::UNORM, 'R'};
				this->_channels[1] = {8,  8, 1, DXGIPixel::UNORM, 'G'};
				this->_channels[2] = {8, 16, 2, DXGIPixel::UNORM, 'B'};
				this->_channels[3] = {8, 24, 3, DXGIPixel::UNORM, 'A'};
				break;
			}
			case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB: // UNSUPPORTED
			{
				this->_num_channels = 4;
				this->_channels[0] = {8,  0, 0, DXGIPixel::SRGB, 'R'};
				this->_channels[1] = {8,  8, 1, DXGIPixel::SRGB, 'G'};
				this->_channels[2] = {8, 16, 2, DXGIPixel::SRGB, 'B'};
				this->_channels[3] = {8, 24, 3, DXGIPixel::SRGB, 'A'};
				break;
			}
			case DXGI_FORMAT_R8G8B8A8_UINT: // XMUBYTE4
			{
				this->_num_channels = 4;
				this->_channels[0] = {8,  0, 0, DXGIPixel::UINT, 'R'};
				this->_channels[1] = {8,  8, 1, DXGIPixel::UINT, 'G'};
				this->_channels[2] = {8, 16, 2, DXGIPixel::UINT, 'B'};
				this->_channels[3] = {8, 24, 3, DXGIPixel::UINT, 'A'};
				break;
			}
			case DXGI_FORMAT_R8G8B8A8_SNORM: // XMBYTEN4
			{
				this->_num_channels = 4;
				this->_channels[0] = {8,  0, 0, DXGIPixel::SNORM, 'R'};
				this->_channels[1] = {8,  8, 1, DXGIPixel::SNORM, 'G'};
				this->_channels[2] = {8, 16, 2, DXGIPixel::SNORM, 'B'};
				this->_channels[3] = {8, 24, 3, DXGIPixel::SNORM, 'A'};
				break;
			}
			case DXGI_FORMAT_R8G8B8A8_SINT: // XMBYTE4
			{
				this->_num_channels = 4;
				this->_channels[0] = {8,  0, 0, DXGIPixel::SINT, 'R'};
				this->_channels[1] = {8,  8, 1, DXGIPixel::SINT, 'G'};
				this->_channels[2] = {8, 16, 2, DXGIPixel::SINT, 'B'};
				this->_channels[3] = {8, 24, 3, DXGIPixel::SINT, 'A'};
				break;
			}
			case DXGI_FORMAT_R16G16_TYPELESS:
			{
				this->_num_channels = 2;
				this->_channels[0] = {16,  0, 0, DXGIPixel::TYPELESS, 'R'};
				this->_channels[1] = {16, 16, 1, DXGIPixel::TYPELESS, 'G'};
				break;
			}
			case DXGI_FORMAT_R16G16_FLOAT: // XMHALF2
			{
				this->_num_channels = 2;
				this->_channels[0] = {16,  0, 0, DXGIPixel::FLOAT, 'R'};
				this->_channels[1] = {16, 16, 1, DXGIPixel::FLOAT, 'G'};
				break;
			}
			case DXGI_FORMAT_R16G16_UNORM: // XMUSHORTN2
			{
				this->_num_channels = 2;
				this->_channels[0] = {16,  0, 0, DXGIPixel::UNORM, 'R'};
				this->_channels[1] = {16, 16, 1, DXGIPixel::UNORM, 'G'};
				break;
			}
			case DXGI_FORMAT_R16G16_UINT: // XMUSHORT2
			{
				this->_num_channels = 2;
				this->_channels[0] = {16,  0, 0, DXGIPixel::UINT, 'R'};
				this->_channels[1] = {16, 16, 1, DXGIPixel::UINT, 'G'};
				break;
			}
			case DXGI_FORMAT_R16G16_SNORM: // XMSHORTN2
			{
				this->_num_channels = 2;
				this->_channels[0] = {16,  0, 0, DXGIPixel::SNORM, 'R'};
				this->_channels[1] = {16, 16, 1, DXGIPixel::SNORM, 'G'};
				break;
			}
			case DXGI_FORMAT_R16G16_SINT: // XMSHORT2
			{
				this->_num_channels = 2;
				this->_channels[0] = {16,  0, 0, DXGIPixel::SINT, 'R'};
				this->_channels[1] = {16, 16, 1, DXGIPixel::SINT, 'G'};
				break;
			}
			case DXGI_FORMAT_R32_TYPELESS:
			{
				this->_num_channels = 1;
				this->_channels[0] = {32, 0, 0, DXGIPixel::TYPELESS, 'R'};
				break;
			}
			case DXGI_FORMAT_D32_FLOAT: // UNSUPPORTED
			{
				this->_num_channels = 1;
				this->_channels[0] = {32, 0, 0, DXGIPixel::FLOAT, 'D'};
				break;
			}
			case DXGI_FORMAT_R32_FLOAT: // UNSUPPORTED
			{
				this->_num_channels = 1;
				this->_channels[0] = {32, 0, 0, DXGIPixel::FLOAT, 'R'};
				break;
			}
			case DXGI_FORMAT_R32_UINT: // UNSUPPORTED
			{
				this->_num_channels = 1;
				this->_channels[0] = {32, 0, 0, DXGIPixel::UINT, 'R'};
				break;
			}
			case DXGI_FORMAT_R32_SINT: // UNSUPPORTED
			{
				this->_num_channels = 1;
				this->_channels[0] = {32, 0, 0, DXGIPixel::SINT, 'R'};
				break;
			}
			case DXGI_FORMAT_R24G8_TYPELESS:
			{
				this->_num_channels = 2;
				this->_channels[0] = {24, 0, 0, DXGIPixel::TYPELESS, 'R'};
				this->_channels[1] = {8, 24, 1, DXGIPixel::TYPELESS, 'G'};
				break;
			}
			case DXGI_FORMAT_D24_UNORM_S8_UINT: // UNSUPPORTED
			{
				this->_num_channels = 2;
				this->_channels[0] = {24, 0, 0, DXGIPixel::UNORM, 'D'};
				this->_channels[1] = {8, 24, 1, DXGIPixel::UINT,  'S'};
				break;
			}
			case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
			{
				this->_num_channels = 1;
				this->_channels[0] = {24, 0, 0, DXGIPixel::UNORM, 'R'};
				break;
			}
			case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
			{
				this->_num_channels = 1;
				this->_channels[0] = {8, 24, 1, DXGIPixel::UINT, 'G'};
				break;
			}
			case DXGI_FORMAT_R8G8_TYPELESS:
			{
				this->_num_channels = 2;
				this->_channels[0] = {8, 0, 0, DXGIPixel::TYPELESS, 'R'};
				this->_channels[1] = {8, 8, 1, DXGIPixel::TYPELESS, 'G'};
				break;
			}
			case DXGI_FORMAT_R8G8_UNORM: // XMUBYTEN2
			{
				this->_num_channels = 2;
				this->_channels[0] = {8, 0, 0, DXGIPixel::UNORM, 'R'};
				this->_channels[1] = {8, 8, 1, DXGIPixel::UNORM, 'G'};
				break;
			}
			case DXGI_FORMAT_R8G8_UINT: // XMUBYTE2
			{
				this->_num_channels = 2;
				this->_channels[0] = {8, 0, 0, DXGIPixel::UINT, 'R'};
				this->_channels[1] = {8, 8, 1, DXGIPixel::UINT, 'G'};
				break;
			}
			case DXGI_FORMAT_R8G8_SNORM: // XMBYTEN2
			{
				this->_num_channels = 2;
				this->_channels[0] = {8, 0, 0, DXGIPixel::SNORM, 'R'};
				this->_channels[1] = {8, 8, 1, DXGIPixel::SNORM, 'G'};
				break;
			}
			case DXGI_FORMAT_R8G8_SINT: // XMBYTE2
			{
				this->_num_channels = 2;
				this->_channels[0] = {8, 0, 0, DXGIPixel::SINT, 'R'};
				this->_channels[1] = {8, 8, 1, DXGIPixel::SINT, 'G'};
				break;
			}
			case DXGI_FORMAT_R16_TYPELESS:
			{
				this->_num_channels = 1;
				this->_channels[0] = {16, 0, 0, DXGIPixel::TYPELESS, 'R'};
				break;
			}
			case DXGI_FORMAT_R16_FLOAT: // UNSUPPORTED
			{
				this->_num_channels = 1;
				this->_channels[0] = {16, 0, 0, DXGIPixel::FLOAT, 'R'};
				break;
			}
			case DXGI_FORMAT_D16_UNORM: // UNSUPPORTED
			{
				this->_num_channels = 1;
				this->_channels[0] = {16, 0, 0, DXGIPixel::UNORM, 'D'};
				break;
			}
			case DXGI_FORMAT_R16_UNORM: // UNSUPPORTED
			{
				this->_num_channels = 1;
				this->_channels[0] = {16, 0, 0, DXGIPixel::UNORM, 'R'};
				break;
			}
			case DXGI_FORMAT_R16_UINT: // UNSUPPORTED
			{
				this->_num_channels = 1;
				this->_channels[0] = {16, 0, 0, DXGIPixel::UNORM, 'R'};
				break;
			}
			case DXGI_FORMAT_R16_SNORM: // UNSUPPORTED
			{
				this->_num_channels = 1;
				this->_channels[0] = {16, 0, 0, DXGIPixel::SNORM, 'R'};
				break;
			}
			case DXGI_FORMAT_R16_SINT: // UNSUPPORTED
			{
				this->_num_channels = 1;
				this->_channels[0] = {16, 0, 0, DXGIPixel::SINT, 'R'};
				break;
			}
			case DXGI_FORMAT_R8_TYPELESS:
			{
				this->_num_channels = 1;
				this->_channels[0] = {8, 0, 0, DXGIPixel::TYPELESS, 'R'};
				break;
			}
			case DXGI_FORMAT_R8_UNORM: // UNSUPPORTED
			{
				this->_num_channels = 1;
				this->_channels[0] = {8, 0, 0, DXGIPixel::UNORM, 'R'};
				break;
			}
			case DXGI_FORMAT_R8_UINT: // UNSUPPORTED
			{
				this->_num_channels = 1;
				this->_channels[0] = {8, 0, 0, DXGIPixel::UINT, 'R'};
				break;
			}
			case DXGI_FORMAT_R8_SNORM: // UNSUPPORTED
			{
				this->_num_channels = 1;
				this->_channels[0] = {8, 0, 0, DXGIPixel::SNORM, 'R'};
				break;
			}
			case DXGI_FORMAT_R8_SINT: // UNSUPPORTED
			{
				this->_num_channels = 1;
				this->_channels[0] = {8, 0, 0, DXGIPixel::SINT, 'R'};
				break;
			}
			case DXGI_FORMAT_A8_UNORM: // UNSUPPORTED
			{
				this->_num_channels = 1;
				this->_channels[0] = {8, 0, 0, DXGIPixel::UNORM, 'A'};
				break;
			}
			case DXGI_FORMAT_R1_UNORM: // UNSUPPORTED
			{
				this->_num_channels = 1;
				this->_channels[0] = {1, 0, 0, DXGIPixel::UNORM, 'R'};
				break;
			}
			case DXGI_FORMAT_R9G9B9E5_SHAREDEXP: // XMFLOAT3SE
			{
				this->_num_channels = 4;
				this->_channels[0] = {9,  0, 0, DXGIPixel::SHAREDEXP, 'R'};
				this->_channels[1] = {9,  9, 1, DXGIPixel::SHAREDEXP, 'G'};
				this->_channels[2] = {9, 18, 2, DXGIPixel::SHAREDEXP, 'B'};
				this->_channels[3] = {5, 27, 3, DXGIPixel::SHAREDEXP, 'E'};
				break;
			}
			case DXGI_FORMAT_B5G6R5_UNORM: // XMU565
			{
				this->_num_channels = 3;
				this->_channels[0] = {5,  0, 2, DXGIPixel::UNORM, 'B'};
				this->_channels[1] = {6,  5, 1, DXGIPixel::UNORM, 'G'};
				this->_channels[2] = {5, 11, 0, DXGIPixel::UNORM, 'R'};
				break;
			}
			case DXGI_FORMAT_B5G5R5A1_UNORM: // XMU555
			{
				this->_num_channels = 4;
				this->_channels[0] = {5,  0, 2, DXGIPixel::UNORM, 'B'};
				this->_channels[1] = {5,  5, 1, DXGIPixel::UNORM, 'G'};
				this->_channels[2] = {5, 10, 0, DXGIPixel::UNORM, 'R'};
				this->_channels[3] = {1, 15, 3, DXGIPixel::UNORM, 'A'};
				break;
			}
			case DXGI_FORMAT_B8G8R8A8_UNORM: // XMCOLOR
			{
				this->_num_channels = 4;
				this->_channels[0] = {8,  0, 2, DXGIPixel::UNORM, 'B'};
				this->_channels[1] = {8,  8, 1, DXGIPixel::UNORM, 'G'};
				this->_channels[2] = {8, 16, 0, DXGIPixel::UNORM, 'R'};
				this->_channels[3] = {8, 24, 3, DXGIPixel::UNORM, 'A'};
				break;
			}
			case DXGI_FORMAT_B8G8R8X8_UNORM: // XMCOLOR
			{
				this->_num_channels = 3;
				this->_channels[0] = {8,  0, 2, DXGIPixel::UNORM, 'B'};
				this->_channels[1] = {8,  8, 1, DXGIPixel::UNORM, 'G'};
				this->_channels[2] = {8, 16, 0, DXGIPixel::UNORM, 'R'};
				break;
			}
			case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM: // XMUBYTEN4 and use XMLoadUDecN4_XR and XMStoreUDecN4_XR
			{
				this->_num_channels = 4;
				this->_channels[0] = {10,  0, 0, DXGIPixel::XR_BIAS, 'R'};
				this->_channels[1] = {10, 10, 1, DXGIPixel::XR_BIAS, 'G'};
				this->_channels[2] = {10, 20, 2, DXGIPixel::XR_BIAS, 'B'};
				this->_channels[3] = { 2, 30, 3, DXGIPixel::UNORM,   'A'};
				break;
			}
			case DXGI_FORMAT_B8G8R8A8_TYPELESS:
			{
				this->_num_channels = 4;
				this->_channels[0] = {8,  0, 2, DXGIPixel::TYPELESS, 'B'};
				this->_channels[1] = {8,  8, 1, DXGIPixel::TYPELESS, 'G'};
				this->_channels[2] = {8, 16, 0, DXGIPixel::TYPELESS, 'R'};
				this->_channels[3] = {8, 24, 3, DXGIPixel::TYPELESS, 'A'};
				break;
			}
			case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB: // UNSUPPORTED
			{
				this->_num_channels = 4;
				this->_channels[0] = {8,  0, 2, DXGIPixel::SRGB, 'B'};
				this->_channels[1] = {8,  8, 1, DXGIPixel::SRGB, 'G'};
				this->_channels[2] = {8, 16, 0, DXGIPixel::SRGB, 'R'};
				this->_channels[3] = {8, 24, 3, DXGIPixel::SRGB, 'A'};
				break;
			}
			case DXGI_FORMAT_B8G8R8X8_TYPELESS:
			{
				this->_num_channels = 3;
				this->_channels[0] = {8,  0, 2, DXGIPixel::TYPELESS, 'B'};
				this->_channels[1] = {8,  8, 1, DXGIPixel::TYPELESS, 'G'};
				this->_channels[2] = {8, 16, 0, DXGIPixel::TYPELESS, 'R'};
				break;
			}
			case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB: // UNSUPPORTED
			{
				this->_num_channels = 3;
				this->_channels[0] = {8,  0, 2, DXGIPixel::SRGB, 'B'};
				this->_channels[1] = {8,  8, 1, DXGIPixel::SRGB, 'G'};
				this->_channels[2] = {8, 16, 0, DXGIPixel::SRGB, 'R'};
				break;
			}
			case DXGI_FORMAT_B4G4R4A4_UNORM: // XMUNIBBLE4
			{
				this->_num_channels = 4;
				this->_channels[0] = {4,  0, 2, DXGIPixel::UNORM, 'B'};
				this->_channels[1] = {4,  4, 1, DXGIPixel::UNORM, 'G'};
				this->_channels[2] = {4,  8, 0, DXGIPixel::UNORM, 'R'};
				this->_channels[3] = {4, 12, 3, DXGIPixel::UNORM, 'A'};
				break;
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

		/* check for uniform datatype and width */
		this->_has_uniform_width = true;
		this->_has_uniform_datatype = true;
		for(size_t i = 1; i < this->_num_channels; i++)
		{
			if(this->_channels[i].width != this->_channels[0].width)
			{
				this->_has_uniform_width = false;
			}
			if(this->_channels[i].datatype != this->_channels[0].datatype)
			{
				this->_has_uniform_datatype = false;
			}
		}

		/* make sure all channel widths are <= 32 bits wide */
		/* assumptions are made later which depend on this */
		for(size_t i = 0; i < this->_num_channels; i++)
		{
			if(this->_channels[i].width > 32)
			{
				MEXError::PrintMexError(MEU_FL,
								    MEU_SEVERITY_INTERNAL,
								    "UnexpectedFormatError",
								    "A channel was unexpectedly %llu bits wide.", this->_channels[i].width);
			}
		}

	}
	
	void DXGIPixel::ExtractChannels(const size_t* ch_indices, const size_t* out_idx, size_t num_idx, mxArray*&out, mxClassID out_class)
	{
		DXGIPixel::DATATYPE input_datatype;
		
		StorageFunction storage_function = nullptr;
		
		/* find max output index */
		size_t max_out_idx = 0;
		for(size_t i = 0; i < num_idx; i++)
		{
			max_out_idx = (out_idx[i] > max_out_idx)? out_idx[i] : max_out_idx;
		}
		
		const mwSize out_dims[] = {this->_image->height, this->_image->width, max_out_idx + 1};
		mwSize ndim = ARRAYSIZE(out_dims);
		
		/* check channels requested are inside bounds */
		for(size_t i = 0; i < num_idx; i++)
		{
			if(ch_indices[i] >= this->_num_channels)
			{
				MEXError::PrintMexError(MEU_FL,
				                        MEU_SEVERITY_USER,
				                        "InvalidChannelError",
				                        "Channel index %llu exceeds the bounds of available channels. Restrict "
					                   "your selection to between 0 and %llu.",
				                        ch_indices[i], this->_num_channels);
			}
		}
		
		/* check expected image data size */
		size_t expected_rowpitch;
		size_t expected_slicepitch;
		DirectX::ComputePitch(this->_format, this->_image->width, this->_image->height, expected_rowpitch, expected_slicepitch);
		if(expected_rowpitch != this->_image->rowPitch)
		{
			MEXError::PrintMexError(MEU_FL,
			                        MEU_SEVERITY_INTERNAL,
			                        "InvalidSizeError",
			                        "The expected row pitch of the image (%llu) was different than the nominal "
					              "row pitch (%llu).", expected_rowpitch, this->_image->rowPitch);
		}
		if(expected_slicepitch != this->_image->slicePitch)
		{
			MEXError::PrintMexError(MEU_FL,
			                        MEU_SEVERITY_INTERNAL,
			                        "InvalidSizeError",
			                        "The expected slice pitch of the image (%llu) was different than the nominal "
			                        "slice pitch (%llu).", expected_slicepitch, this->_image->slicePitch);
		}
		
		
		if(this->_has_uniform_datatype)
		{
			input_datatype = this->_channels[0].datatype;
		}
		else
		{
			input_datatype = this->_channels[ch_indices[0]].datatype;
			for(size_t i = 0; i < num_idx; i++)
			{
				if(this->_channels[ch_indices[i]].datatype != input_datatype)
				{
					MEXError::PrintMexError(MEU_FL,
					                        MEU_SEVERITY_USER,
					                        "IncompatibleDatatypesError",
					                        "The datatypes of the format must be uniform to output to a single matrix.");
					break;
				}
			}
		}
		
		/* Determine datatype of MATLAB output.
		 * Try to do this losslessly unless otherwise specified. */
		switch(input_datatype)
		{
			case DXGIPixel::SNORM:
			{
				out = mxCreateNumericArray(ndim, out_dims, mxSINGLE_CLASS, mxREAL);
				storage_function = &DXGIPixel::ChannelElement<DXGIPixel::SNORM, mxSingle>::Store;
				break;
			}
			case DXGIPixel::UNORM:
			{
				out = mxCreateNumericArray(ndim, out_dims, mxSINGLE_CLASS, mxREAL);
				storage_function = &DXGIPixel::ChannelElement<DXGIPixel::UNORM, mxSingle>::Store;
				break;
			}
			case DXGIPixel::FLOAT:
			{
				if(this->_format == DXGI_FORMAT_R11G11B10_FLOAT)
				{
				
				}
				else
				{
					out = mxCreateNumericArray(ndim, out_dims, mxSINGLE_CLASS, mxREAL);
					storage_function = &DXGIPixel::ChannelElement<DXGIPixel::FLOAT, mxSingle>::Store;
				}
				break;
			}
			case DXGIPixel::SRGB:
			{
				out = mxCreateNumericArray(ndim, out_dims, mxSINGLE_CLASS, mxREAL);
				storage_function = &DXGIPixel::ChannelElement<DXGIPixel::SRGB, mxSingle>::Store;
				break;
			}
			case DXGIPixel::SHAREDEXP:
			{
				/* special case */
				
				if(this->_format != DXGI_FORMAT_R9G9B9E5_SHAREDEXP)
				{
					MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_INTERNAL, "UnexpectedFormatError", "Unexpected shared exponent format. Cannot continue.");
				}
				
				/* recalculate max output index ignoring indices referring to the shared exponent */
				max_out_idx = 0;
				for(size_t i = 0; i < num_idx; i++)
				{
					if(ch_indices[i] != 3)
					{
						max_out_idx = (out_idx[i] > max_out_idx)? out_idx[i] : max_out_idx;
					}
				}
				
				out = mxCreateNumericArray(ndim, out_dims, mxSINGLE_CLASS, mxREAL);

				float debiased_exp;
				auto data = (mxSingle*)mxGetData(out);
				auto pixels = (uint32_t*)this->_image->pixels;
				uint32_t masks[3] = {SHAREDEXP_R_MASK, SHAREDEXP_G_MASK, SHAREDEXP_B_MASK};
				
				/* NOTES
				 * This format supports denormalized numbers,
				 * so if exponent is 0 then use 1 - SHAREDEXP_BIAS.
				 *
				 * There is no sign bit.
				 *
				 * The mantissa is NOT offset by 1.
				 */
				
				for(size_t i = 0; i < this->_num_pixels(); i++)
				{
					uint32_t pixel_data = *(pixels + i);
					int32_t  exp_bits = (pixel_data & SHAREDEXP_E_MASK) >> 27u;
					if(exp_bits == 0)
					{
						/* this format has denorm support */
						debiased_exp = 1 - SHAREDEXP_BIAS;
					}
					else
					{
						debiased_exp = exp_bits - SHAREDEXP_BIAS;
					}
					float scalar = std::pow(2.0f, debiased_exp);
					for(size_t j = 0; j < num_idx; j++)
					{
						if(ch_indices[i] != 3)
						{
							mwIndex dst_idx = i / this->_image->width + (i % this->_image->width) * this->_image->height + out_idx[j] * this->_num_pixels();
							data[dst_idx] = ((pixel_data & masks[ch_indices[j]]) >> this->_channels[ch_indices[j]].offset) * scalar;
						}
					}
				}
				return; // EARLY RETURN
			}
			case DXGIPixel::XR_BIAS:
			{
				out = mxCreateNumericArray(ndim, out_dims, mxSINGLE_CLASS, mxREAL);
				storage_function = &DXGIPixel::ChannelElement<DXGIPixel::XR_BIAS, mxSingle>::Store;
				break;
			}
			case DXGIPixel::SINT:
			{
				/* Keep max width consistent with max channel width
				 * even if we don't select the max channel. This is
				 * so we don't confuse a user with different types
				 * on the same data. */
				size_t max_width = DirectX::BitsPerColor(this->_format);
				
				/* determine MATLAB class width */
				if(max_width == 1)
				{
					out = mxCreateLogicalArray(ndim, out_dims);
					storage_function = &DXGIPixel::ChannelElement<DXGIPixel::SINT, mxLogical>::Store;
				}
				else if(max_width <= 8)
				{
					out = mxCreateNumericArray(ndim, out_dims, mxINT8_CLASS, mxREAL);
					storage_function = &DXGIPixel::ChannelElement<DXGIPixel::SINT, mxInt8>::Store;
				}
				else if(max_width <= 16)
				{
					out = mxCreateNumericArray(ndim, out_dims, mxINT16_CLASS, mxREAL);
					storage_function = &DXGIPixel::ChannelElement<DXGIPixel::SINT, mxInt16>::Store;
				}
				else
				{
					out = mxCreateNumericArray(ndim, out_dims, mxINT32_CLASS, mxREAL);
					storage_function = &DXGIPixel::ChannelElement<DXGIPixel::SINT, mxInt32>::Store;
				}
				break;
			}
			case DXGIPixel::UINT:
			{
				size_t max_width = DirectX::BitsPerColor(this->_format);
				
				/* determine MATLAB class width */
				if(max_width == 1)
				{
					out = mxCreateLogicalArray(ndim, out_dims);
					storage_function = &DXGIPixel::ChannelElement<DXGIPixel::UINT, mxLogical>::Store;
				}
				else if(max_width <= 8)
				{
					out = mxCreateNumericArray(ndim, out_dims, mxUINT8_CLASS, mxREAL);
					storage_function = &DXGIPixel::ChannelElement<DXGIPixel::UINT, mxUint8>::Store;
				}
				else if(max_width <= 16)
				{
					out = mxCreateNumericArray(ndim, out_dims, mxUINT16_CLASS, mxREAL);
					storage_function = &DXGIPixel::ChannelElement<DXGIPixel::UINT, mxUint16>::Store;
				}
				else
				{
					out = mxCreateNumericArray(ndim, out_dims, mxUINT32_CLASS, mxREAL);
					storage_function = &DXGIPixel::ChannelElement<DXGIPixel::UINT, mxUint32>::Store;
				}
				break;
			}
			case DXGIPixel::TYPELESS:
			{
				size_t max_width = DirectX::BitsPerColor(this->_format);
				
				/* determine MATLAB class width */
				if(max_width == 1)
				{
					out = mxCreateLogicalArray(ndim, out_dims);
					storage_function = &DXGIPixel::ChannelElement<DXGIPixel::TYPELESS, mxLogical>::Store;
				}
				else if(max_width <= 8)
				{
					out = mxCreateNumericArray(ndim, out_dims, mxUINT8_CLASS, mxREAL);
					storage_function = &DXGIPixel::ChannelElement<DXGIPixel::TYPELESS, mxUint8>::Store;
				}
				else if(max_width <= 16)
				{
					out = mxCreateNumericArray(ndim, out_dims, mxUINT16_CLASS, mxREAL);
					storage_function = &DXGIPixel::ChannelElement<DXGIPixel::TYPELESS, mxUint16>::Store;
				}
				else
				{
					out = mxCreateNumericArray(ndim, out_dims, mxUINT32_CLASS, mxREAL);
					storage_function = &DXGIPixel::ChannelElement<DXGIPixel::TYPELESS, mxUint32>::Store;
				}
				break;
			}
			default:
			{
				/* must have uniform type */
				MEXError::PrintMexError(MEU_FL,
				                        MEU_SEVERITY_USER,
				                        "IncompatibleDatatypesError",
				                        "The datatypes of the format must be uniform to output to a single matrix.");
			}
		}
		
		void* data = mxGetData(out);
		switch(this->_pixel_width)
		{
			case 128: /* always uniform width with 4 channels */
			case 96:  /* always uniform width with 3 channels*/
			{
				this->StoreUniformChannels<uint32_t>(ch_indices, out_idx, num_idx, data, storage_function);
				break;
			}
			case 64:
			{
				
				if(this->_has_uniform_width)
				{
					switch(this->_num_channels)
					{
						case 2:
						{
							this->StoreUniformChannels<uint32_t>(ch_indices, out_idx, num_idx, data, storage_function);
							break;
						}
						case 4:
						{
							this->StoreUniformChannels<uint16_t>(ch_indices, out_idx, num_idx, data, storage_function);
							break;
						}
						default:
						{
							MEXError::PrintMexError(MEU_FL,
							                        MEU_SEVERITY_INTERNAL,
							                        "UnexpectedChannelsError",
							                        "Unexpected number of channels with uniform width. The pixel was 64 bits wide with %llu channels.", this->_num_channels);
						}
					}
				}
				else
				{
					auto pixels = (uint64_t*)this->_image->pixels;
					uint64_t masks[4];
					for(size_t i = 0; i < this->_num_channels; i++)
					{
						masks[i] =(((uint64_t)1 << this->_channels[i].width) - (uint64_t)1) << this->_channels[i].offset;
					}
					
					for(size_t i = 0; i < this->_num_pixels(); i++)
					{
						uint32_t pixel_data = *(pixels + i);
						for(size_t j = 0; j < num_idx; j++)
						{
							mwIndex dst_idx = i / this->_image->width + (i % this->_image->width) * this->_image->height + out_idx[j] * this->_num_pixels();
							auto channel_data = (uint32_t)((pixel_data & masks[ch_indices[j]]) >> this->_channels[ch_indices[j]].offset);
							storage_function(data, dst_idx, channel_data, (uint32_t)this->_channels[ch_indices[j]].width);
						}
					}
				}
				break;
			}
			case 32:
			{
				if(this->_has_uniform_width)
				{
					switch(this->_num_channels)
					{
						case 1:
						{
							this->StoreUniformChannels<uint32_t>(ch_indices, out_idx, num_idx, data, storage_function);
							break;
						}
						case 2:
						{
							this->StoreUniformChannels<uint16_t>(ch_indices, out_idx, num_idx, data, storage_function);
							break;
						}
						case 4:
						{
							this->StoreUniformChannels<uint8_t>(ch_indices, out_idx, num_idx, data, storage_function);
							break;
						}
						default:
						{
							MEXError::PrintMexError(MEU_FL,
							                        MEU_SEVERITY_INTERNAL,
							                        "UnexpectedChannelsError",
							                        "Unexpected number of channels with uniform width. The pixel was 32 bits wide with %llu channels.", this->_num_channels);
						}
					}
				}
				else
				{
					auto pixels = (uint32_t*)this->_image->pixels;
					uint32_t masks[4];
					for(size_t i = 0; i < this->_num_channels; i++)
					{
						masks[i] =(((uint32_t)1 << this->_channels[i].width) - (uint32_t)1) << this->_channels[i].offset;
					}
					
					for(size_t i = 0; i < this->_num_pixels(); i++)
					{
						uint32_t pixel_data = *(pixels + i);
						for(size_t j = 0; j < num_idx; j++)
						{
							mwIndex dst_idx = i / this->_image->width + (i % this->_image->width) * this->_image->height + out_idx[j] * this->_num_pixels();
							uint32_t channel_data = (pixel_data & masks[ch_indices[j]]) >> this->_channels[ch_indices[j]].offset;
							storage_function(data, dst_idx, channel_data, (uint32_t)this->_channels[ch_indices[j]].width);
						}
					}
				}
				break;
			}
			case 16:
			{
				if(this->_has_uniform_width)
				{
					switch(this->_num_channels)
					{
						case 1:
						{
							this->StoreUniformChannels<uint16_t>(ch_indices, out_idx, num_idx, data, storage_function);
							break;
						}
						case 2:
						{
							this->StoreUniformChannels<uint8_t>(ch_indices, out_idx, num_idx, data, storage_function);
							break;
						}
						default:
						{
							MEXError::PrintMexError(MEU_FL,
							                        MEU_SEVERITY_INTERNAL,
							                        "UnexpectedChannelsError",
							                        "Unexpected number of channels with uniform width. The pixel was 16 bits wide with %llu channels.", this->_num_channels);
						}
					}
				}
				else
				{
					auto pixels = (uint16_t*)this->_image->pixels;
					uint16_t masks[4];
					for(size_t i = 0; i < this->_num_channels; i++)
					{
						masks[i] = (uint16_t)(((uint16_t)1 << (uint16_t)this->_channels[i].width) - (uint16_t)1) << this->_channels[i].offset;
					}
					
					for(size_t i = 0; i < this->_num_pixels(); i++)
					{
						uint32_t pixel_data = *(pixels + i);
						for(size_t j = 0; j < num_idx; j++)
						{
							mwIndex dst_idx = i / this->_image->width + (i % this->_image->width) * this->_image->height + out_idx[j] * this->_num_pixels();
							uint32_t channel_data = (uint16_t)(pixel_data & masks[ch_indices[j]]) >> this->_channels[ch_indices[j]].offset;
							storage_function(data, dst_idx, channel_data, (uint32_t)this->_channels[ch_indices[j]].width);
						}
					}
				}
				break;
			}
			case 8:
			{
				if(this->_has_uniform_width)
				{
					switch(this->_num_channels)
					{
						case 1:
						{
							this->StoreUniformChannels<uint8_t>(ch_indices, out_idx, num_idx, data, storage_function);
							break;
						}
						default:
						{
							MEXError::PrintMexError(MEU_FL,
							                        MEU_SEVERITY_INTERNAL,
							                        "UnexpectedChannelsError",
							                        "Unexpected number of channels with uniform width. The pixel was 8 bits wide with %llu channels.", this->_num_channels);
						}
					}
				}
				else
				{
					MEXError::PrintMexError(MEU_FL,
					                        MEU_SEVERITY_INTERNAL,
					                        "UnexpectedChannelsError",
					                        "Unexpected channel width. The pixel was 8 bits wide and did not have uniform width.");
				}
				break;
			}
			case 1:
			{
				auto data_l = (mxLogical*)data;
				size_t i, j, k, l;
				size_t px_idx = 0;
				for(i = 0; i < this->_image->height; i++)
				{
					
					/* 32-bit width is fastest on x86-64 and i686 */
					auto pixels32 = (uint32_t*)(this->_image->pixels + i*this->_image->rowPitch);
					for(j = 0; j < this->_image->rowPitch / 4; j++)
					{
						uint32_t page = pixels32[j];
						for(k = 32; k > 0; k--, px_idx++)
						{
							/* there should only be one channel, but it may be repeatedly stored */
							uint32_t channel_data = (uint32_t)(page >> (k - 1)) & 1u;
							for(l = 0; l < num_idx; l++)
							{
								mwIndex dst_idx = px_idx / this->_image->width + (px_idx % this->_image->width) * this->_image->height + out_idx[l] * this->_num_pixels();
								data_l[dst_idx] = channel_data;
							}
						}
					}
					auto pixels8 = (uint8_t*)pixels32;
					for(j *= 4; j < this->_image->rowPitch - 1; j++)
					{
						uint8_t page = pixels8[j];
						for(k = 8; k > 0; k--, px_idx++)
						{
							/* there should only be one channel, but it may be repeatedly stored */
							uint8_t channel_data = (uint8_t)(page >> (k - 1)) & 1u;
							for(l = 0; l < num_idx; l++)
							{
								mwIndex dst_idx = px_idx / this->_image->width + (px_idx % this->_image->width) * this->_image->height + out_idx[l] * this->_num_pixels();
								data_l[dst_idx] = channel_data;
							}
						}
					}
					uint8_t page = pixels8[j];
					for(k = this->_image->width - ((this->_image->rowPitch - 1) * 8); k > 0; k--, px_idx++)
					{
						/* there should only be one channel, but it may be repeatedly stored */
						uint8_t channel_data = (uint8_t)(page >> (k - 1)) & 1u;
						for(l = 0; l < num_idx; l++)
						{
							mwIndex dst_idx = px_idx / this->_image->width + (px_idx % this->_image->width) * this->_image->height + out_idx[l] * this->_num_pixels();
							data_l[dst_idx] = channel_data;
						}
					}
				}
			}
		}
	}
	
	void DXGIPixel::ExtractRGB(mxArray*& mx_rgb)
	{
		size_t ch_indices[MAX_CHANNELS];
		size_t out_idx[MAX_CHANNELS];
		if(DirectX::HasAlpha(this->_format))
		{
			for(size_t i = 0, j = 0; i < this->_num_channels; i++)
			{
				if(this->_channels[i].name != 'A')
				{
					ch_indices[j] = i;
					out_idx[j] = this->_channels[i].standard_idx;
					j++;
				}
			}
			this->ExtractChannels(ch_indices, out_idx, this->_num_channels - 1, mx_rgb);
		}
		else
		{
			for(size_t i = 0; i < this->_num_channels; i++)
			{
				ch_indices[i] = i;
				out_idx[i] = this->_channels[i].standard_idx;
			}
			this->ExtractChannels(ch_indices, out_idx, this->_num_channels, mx_rgb);
		}
	}
	
	void DXGIPixel::ExtractRGBA(mxArray*& mx_rgba)
	{
		size_t ch_indices[MAX_CHANNELS];
		size_t out_idx[MAX_CHANNELS];
		for(size_t i = 0; i < this->_num_channels; i++)
		{
			ch_indices[i] = i;
			out_idx[i] = this->_channels[i].standard_idx;
		}
		this->ExtractChannels(ch_indices, out_idx, this->_num_channels, mx_rgba);
	}
	
	void DXGIPixel::ExtractRGBA(mxArray*& mx_rgb, mxArray*& mx_a)
	{
		size_t ch_indices[MAX_CHANNELS];
		size_t out_idx[MAX_CHANNELS];
		if(DirectX::HasAlpha(this->_format))
		{
			for(size_t i = 0, j = 0; i < this->_num_channels; i++)
			{
				if(this->_channels[i].name != 'A')
				{
					ch_indices[j] = i;
					out_idx[j] = this->_channels[i].standard_idx;
					j++;
				}
				else
				{
					this->ExtractChannels(i, 0, 1, mx_a);
				}
			}
			this->ExtractChannels(ch_indices, out_idx, this->_num_channels - 1, mx_rgb);
		}
		else
		{
			for(size_t i = 0; i < this->_num_channels; i++)
			{
				ch_indices[i] = i;
				out_idx[i] = this->_channels[i].standard_idx;
			}
			this->ExtractChannels(ch_indices, out_idx, this->_num_channels, mx_rgb);
			mx_a = mxCreateDoubleMatrix(0, 0, mxREAL);
		}
	}
}

