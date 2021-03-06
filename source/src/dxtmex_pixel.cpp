#include "DirectXTex.h"
#include "dxtmex_maps.hpp"
#include "dxtmex_mexerror.hpp"
#include "dxtmex_pixel.hpp"

using namespace DXTMEX;
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
				                   g_format_map.FindStringFromID(fmt).c_str());
		}
		
		if(DirectX::IsPacked(fmt))
		{
			MEXError::PrintMexError(MEU_FL,
			                        MEU_SEVERITY_USER |
			                        MEU_SEVERITY_INTERNAL,
			                        "UnsupportedFormatError",
			                        "Packed format '%s' is not supported for this operation.",
				                   g_format_map.FindStringFromID(fmt).c_str());
		}
		
		if(DirectX::IsVideo(fmt))
		{
			MEXError::PrintMexError(MEU_FL,
			                        MEU_SEVERITY_USER |
			                        MEU_SEVERITY_INTERNAL,
			                        "UnsupportedFormatError",
			                        "Video format '%s' is not supported for this operation.",
				                   g_format_map.FindStringFromID(fmt).c_str());
		}
		
		if(DirectX::IsPlanar(fmt))
		{
			MEXError::PrintMexError(MEU_FL,
			                        MEU_SEVERITY_USER |
			                        MEU_SEVERITY_INTERNAL,
			                        "UnsupportedFormatError",
			                        "Planar format '%s' is not supported for this operation.",
				                   g_format_map.FindStringFromID(fmt).c_str());
		}
		
		if(DirectX::IsPalettized(fmt))
		{
			MEXError::PrintMexError(MEU_FL,
			                        MEU_SEVERITY_USER |
			                        MEU_SEVERITY_INTERNAL,
			                        "UnsupportedFormatError",
			                        "Palettized format '%s' is not supported for this operation.",
			                        g_format_map.FindStringFromID(fmt).c_str());
		}
		
		switch(fmt)
		{
			case DXGI_FORMAT_R32G32B32A32_TYPELESS:
			{
				this->_num_channels = 4;
				this->_channels[0] = {32, 0,  0, DATATYPE::TYPELESS, 'R'};
				this->_channels[1] = {32, 32, 1, DATATYPE::TYPELESS, 'G'};
				this->_channels[2] = {32, 64, 2, DATATYPE::TYPELESS, 'B'};
				this->_channels[3] = {32, 96, 3, DATATYPE::TYPELESS, 'A'};
				break;
			}
			case DXGI_FORMAT_R32G32B32A32_FLOAT: // XMFLOAT4A
			{
				this->_num_channels = 4;
				this->_channels[0] = {32, 0,  0, DATATYPE::FLOAT, 'R'};
				this->_channels[1] = {32, 32, 1, DATATYPE::FLOAT, 'G'};
				this->_channels[2] = {32, 64, 2, DATATYPE::FLOAT, 'B'};
				this->_channels[3] = {32, 96, 3, DATATYPE::FLOAT, 'A'};
				break;
			}
			case DXGI_FORMAT_R32G32B32A32_UINT: // XMUINT4
			{
				this->_num_channels = 4;
				this->_channels[0] = {32, 0,  0, DATATYPE::UINT, 'R'};
				this->_channels[1] = {32, 32, 1, DATATYPE::UINT, 'G'};
				this->_channels[2] = {32, 64, 2, DATATYPE::UINT, 'B'};
				this->_channels[3] = {32, 96, 3, DATATYPE::UINT, 'A'};
				break;
			}
			case DXGI_FORMAT_R32G32B32A32_SINT: // XMINT4
			{
				this->_num_channels = 4;
				this->_channels[0] = {32, 0,  0, DATATYPE::SINT, 'R'};
				this->_channels[1] = {32, 32, 1, DATATYPE::SINT, 'G'};
				this->_channels[2] = {32, 64, 2, DATATYPE::SINT, 'B'};
				this->_channels[3] = {32, 96, 3, DATATYPE::SINT, 'A'};
				break;
			}
			case DXGI_FORMAT_R32G32B32_TYPELESS:
			{
				this->_num_channels = 3;
				this->_channels[0] = {32, 0,  0, DATATYPE::TYPELESS, 'R'};
				this->_channels[1] = {32, 32, 1, DATATYPE::TYPELESS, 'G'};
				this->_channels[2] = {32, 64, 2, DATATYPE::TYPELESS, 'B'};
				break;
			}
			case DXGI_FORMAT_R32G32B32_FLOAT: //XMFLOAT3 or XMFLOAT3A
			{
				this->_num_channels = 3;
				this->_channels[0] = {32, 0,  0, DATATYPE::FLOAT, 'R'};
				this->_channels[1] = {32, 32, 1, DATATYPE::FLOAT, 'G'};
				this->_channels[2] = {32, 64, 2, DATATYPE::FLOAT, 'B'};
				break;
			}
			case DXGI_FORMAT_R32G32B32_UINT: // XMUINT3
			{
				this->_num_channels = 3;
				this->_channels[0] = {32, 0,  0, DATATYPE::UINT, 'R'};
				this->_channels[1] = {32, 32, 1, DATATYPE::UINT, 'G'};
				this->_channels[2] = {32, 64, 2, DATATYPE::UINT, 'B'};
				break;
			}
			case DXGI_FORMAT_R32G32B32_SINT: // XMINT3
			{
				this->_num_channels = 3;
				this->_channels[0] = {32, 0,  0, DATATYPE::SINT, 'R'};
				this->_channels[1] = {32, 32, 1, DATATYPE::SINT, 'G'};
				this->_channels[2] = {32, 64, 2, DATATYPE::SINT, 'B'};
				break;
			}
			case DXGI_FORMAT_R16G16B16A16_TYPELESS:
			{
				this->_num_channels = 4;
				this->_channels[0] = {16, 0,  0, DATATYPE::TYPELESS, 'R'};
				this->_channels[1] = {16, 16, 1, DATATYPE::TYPELESS, 'G'};
				this->_channels[2] = {16, 32, 2, DATATYPE::TYPELESS, 'B'};
				this->_channels[3] = {16, 48, 3, DATATYPE::TYPELESS, 'A'};
				break;
			}
			case DXGI_FORMAT_R16G16B16A16_FLOAT: // XMHALF4
			{
				this->_num_channels = 4;
				this->_channels[0] = {16, 0,  0, DATATYPE::FLOAT, 'R'};
				this->_channels[1] = {16, 16, 1, DATATYPE::FLOAT, 'G'};
				this->_channels[2] = {16, 32, 2, DATATYPE::FLOAT, 'B'};
				this->_channels[3] = {16, 48, 3, DATATYPE::FLOAT, 'A'};
				break;
			}
			case DXGI_FORMAT_R16G16B16A16_UNORM: // XMUSHORTN4
			{
				this->_num_channels = 4;
				this->_channels[0] = {16, 0,  0, DATATYPE::UNORM, 'R'};
				this->_channels[1] = {16, 16, 1, DATATYPE::UNORM, 'G'};
				this->_channels[2] = {16, 32, 2, DATATYPE::UNORM, 'B'};
				this->_channels[3] = {16, 48, 3, DATATYPE::UNORM, 'A'};
				break;
			}
			case DXGI_FORMAT_R16G16B16A16_UINT: // XMUSHORT4
			{
				this->_num_channels = 4;
				this->_channels[0] = {16, 0,  0, DATATYPE::UINT, 'R'};
				this->_channels[1] = {16, 16, 1, DATATYPE::UINT, 'G'};
				this->_channels[2] = {16, 32, 2, DATATYPE::UINT, 'B'};
				this->_channels[3] = {16, 48, 3, DATATYPE::UINT, 'A'};
				break;
			}
			case DXGI_FORMAT_R16G16B16A16_SNORM: // XMSHORTN4
			{
				this->_num_channels = 4;
				this->_channels[0] = {16, 0,  0, DATATYPE::SNORM, 'R'};
				this->_channels[1] = {16, 16, 1, DATATYPE::SNORM, 'G'};
				this->_channels[2] = {16, 32, 2, DATATYPE::SNORM, 'B'};
				this->_channels[3] = {16, 48, 3, DATATYPE::SNORM, 'A'};
				break;
			}
			case DXGI_FORMAT_R16G16B16A16_SINT: // XMSHORT4
			{
				this->_num_channels = 4;
				this->_channels[0] = {16, 0,  0, DATATYPE::SINT, 'R'};
				this->_channels[1] = {16, 16, 1, DATATYPE::SINT, 'G'};
				this->_channels[2] = {16, 32, 2, DATATYPE::SINT, 'B'};
				this->_channels[3] = {16, 48, 3, DATATYPE::SINT, 'A'};
				break;
			}
			case DXGI_FORMAT_R32G32_TYPELESS:
			{
				this->_num_channels = 2;
				this->_channels[0] = {32, 0,  0, DATATYPE::TYPELESS, 'R'};
				this->_channels[1] = {32, 32, 1, DATATYPE::TYPELESS, 'G'};
				break;
			}
			case DXGI_FORMAT_R32G32_FLOAT: // XMFLOAT2 or XMFLOAT2A
			{
				this->_num_channels = 2;
				this->_channels[0] = {32, 0,  0, DATATYPE::FLOAT, 'R'};
				this->_channels[1] = {32, 32, 1, DATATYPE::FLOAT, 'G'};
				break;
			}
			case DXGI_FORMAT_R32G32_UINT: // XMUINT2
			{
				this->_num_channels = 2;
				this->_channels[0] = {32, 0,  0, DATATYPE::UINT, 'R'};
				this->_channels[1] = {32, 32, 1, DATATYPE::UINT, 'G'};
				break;
			}
			case DXGI_FORMAT_R32G32_SINT: // XMINT2
			{
				this->_num_channels = 2;
				this->_channels[0] = {32, 0,  0, DATATYPE::SINT, 'R'};
				this->_channels[1] = {32, 32, 1, DATATYPE::SINT, 'G'};
				break;
			}
			case DXGI_FORMAT_R32G8X24_TYPELESS:
			{
				this->_num_channels = 2;
				this->_channels[0] = {32, 0, 0, DATATYPE::TYPELESS, 'R'};
				this->_channels[1] = {8, 32, 1, DATATYPE::TYPELESS, 'G'};
				break;
			}
			case DXGI_FORMAT_D32_FLOAT_S8X24_UINT: // UNSUPPORTED
			{
				this->_num_channels = 2;
				this->_channels[0] = {32, 0, 0, DATATYPE::FLOAT, 'D'};
				this->_channels[1] = {8, 32, 1, DATATYPE::UINT,  'S'};
				break;
			}
			case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
			{
				this->_num_channels = 1;
				this->_channels[0] = {32, 0, 0, DATATYPE::FLOAT, 'R'};
				break;
			}
			case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
			{
				this->_num_channels = 1;
				this->_channels[0] = {8, 32, 1, DATATYPE::UINT, 'G'};
				break;
			}
			case DXGI_FORMAT_R10G10B10A2_TYPELESS:
			{
				this->_num_channels = 4;
				this->_channels[0] = {10, 0,  0, DATATYPE::TYPELESS, 'R'};
				this->_channels[1] = {10, 10, 1, DATATYPE::TYPELESS, 'G'};
				this->_channels[2] = {10, 20, 2, DATATYPE::TYPELESS, 'B'};
				this->_channels[3] = {2,  30, 3, DATATYPE::TYPELESS, 'A'};
				break;
			}
			case DXGI_FORMAT_R10G10B10A2_UNORM: // XMUDECN4
			{
				this->_num_channels = 4;
				this->_channels[0] = {10, 0,  0, DATATYPE::UNORM, 'R'};
				this->_channels[1] = {10, 10, 1, DATATYPE::UNORM, 'G'};
				this->_channels[2] = {10, 20, 2, DATATYPE::UNORM, 'B'};
				this->_channels[3] = {2,  30, 3, DATATYPE::UNORM, 'A'};
				break;
			}
			case DXGI_FORMAT_R10G10B10A2_UINT: // XMUDEC4
			{
				this->_num_channels = 4;
				this->_channels[0] = {10, 0,  0, DATATYPE::UINT, 'R'};
				this->_channels[1] = {10, 10, 1, DATATYPE::UINT, 'G'};
				this->_channels[2] = {10, 20, 2, DATATYPE::UINT, 'B'};
				this->_channels[3] = {2,  30, 3, DATATYPE::UINT, 'A'};
				break;
			}
			case DXGI_FORMAT_R11G11B10_FLOAT: // XMFLOAT3PK
			{
				this->_num_channels = 3;
				this->_channels[0] = {11, 0,  0, DATATYPE::FLOAT, 'R'};
				this->_channels[1] = {11, 11, 1, DATATYPE::FLOAT, 'G'};
				this->_channels[2] = {10, 22, 2, DATATYPE::FLOAT, 'B'};
				break;
			}
			case DXGI_FORMAT_R8G8B8A8_TYPELESS:
			{
				this->_num_channels = 4;
				this->_channels[0] = {8,  0, 0, DATATYPE::TYPELESS, 'R'};
				this->_channels[1] = {8,  8, 1, DATATYPE::TYPELESS, 'G'};
				this->_channels[2] = {8, 16, 2, DATATYPE::TYPELESS, 'B'};
				this->_channels[3] = {8, 24, 3, DATATYPE::TYPELESS, 'A'};
				break;
			}
			case DXGI_FORMAT_R8G8B8A8_UNORM: // XMUBYTEN4
			{
				this->_num_channels = 4;
				this->_channels[0] = {8,  0, 0, DATATYPE::UNORM, 'R'};
				this->_channels[1] = {8,  8, 1, DATATYPE::UNORM, 'G'};
				this->_channels[2] = {8, 16, 2, DATATYPE::UNORM, 'B'};
				this->_channels[3] = {8, 24, 3, DATATYPE::UNORM, 'A'};
				break;
			}
			case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB: // UNSUPPORTED
			{
				this->_num_channels = 4;
				this->_channels[0] = {8,  0, 0, DATATYPE::SRGB, 'R'};
				this->_channels[1] = {8,  8, 1, DATATYPE::SRGB, 'G'};
				this->_channels[2] = {8, 16, 2, DATATYPE::SRGB, 'B'};
				this->_channels[3] = {8, 24, 3, DATATYPE::SRGB, 'A'};
				break;
			}
			case DXGI_FORMAT_R8G8B8A8_UINT: // XMUBYTE4
			{
				this->_num_channels = 4;
				this->_channels[0] = {8,  0, 0, DATATYPE::UINT, 'R'};
				this->_channels[1] = {8,  8, 1, DATATYPE::UINT, 'G'};
				this->_channels[2] = {8, 16, 2, DATATYPE::UINT, 'B'};
				this->_channels[3] = {8, 24, 3, DATATYPE::UINT, 'A'};
				break;
			}
			case DXGI_FORMAT_R8G8B8A8_SNORM: // XMBYTEN4
			{
				this->_num_channels = 4;
				this->_channels[0] = {8,  0, 0, DATATYPE::SNORM, 'R'};
				this->_channels[1] = {8,  8, 1, DATATYPE::SNORM, 'G'};
				this->_channels[2] = {8, 16, 2, DATATYPE::SNORM, 'B'};
				this->_channels[3] = {8, 24, 3, DATATYPE::SNORM, 'A'};
				break;
			}
			case DXGI_FORMAT_R8G8B8A8_SINT: // XMBYTE4
			{
				this->_num_channels = 4;
				this->_channels[0] = {8,  0, 0, DATATYPE::SINT, 'R'};
				this->_channels[1] = {8,  8, 1, DATATYPE::SINT, 'G'};
				this->_channels[2] = {8, 16, 2, DATATYPE::SINT, 'B'};
				this->_channels[3] = {8, 24, 3, DATATYPE::SINT, 'A'};
				break;
			}
			case DXGI_FORMAT_R16G16_TYPELESS:
			{
				this->_num_channels = 2;
				this->_channels[0] = {16,  0, 0, DATATYPE::TYPELESS, 'R'};
				this->_channels[1] = {16, 16, 1, DATATYPE::TYPELESS, 'G'};
				break;
			}
			case DXGI_FORMAT_R16G16_FLOAT: // XMHALF2
			{
				this->_num_channels = 2;
				this->_channels[0] = {16,  0, 0, DATATYPE::FLOAT, 'R'};
				this->_channels[1] = {16, 16, 1, DATATYPE::FLOAT, 'G'};
				break;
			}
			case DXGI_FORMAT_R16G16_UNORM: // XMUSHORTN2
			{
				this->_num_channels = 2;
				this->_channels[0] = {16,  0, 0, DATATYPE::UNORM, 'R'};
				this->_channels[1] = {16, 16, 1, DATATYPE::UNORM, 'G'};
				break;
			}
			case DXGI_FORMAT_R16G16_UINT: // XMUSHORT2
			{
				this->_num_channels = 2;
				this->_channels[0] = {16,  0, 0, DATATYPE::UINT, 'R'};
				this->_channels[1] = {16, 16, 1, DATATYPE::UINT, 'G'};
				break;
			}
			case DXGI_FORMAT_R16G16_SNORM: // XMSHORTN2
			{
				this->_num_channels = 2;
				this->_channels[0] = {16,  0, 0, DATATYPE::SNORM, 'R'};
				this->_channels[1] = {16, 16, 1, DATATYPE::SNORM, 'G'};
				break;
			}
			case DXGI_FORMAT_R16G16_SINT: // XMSHORT2
			{
				this->_num_channels = 2;
				this->_channels[0] = {16,  0, 0, DATATYPE::SINT, 'R'};
				this->_channels[1] = {16, 16, 1, DATATYPE::SINT, 'G'};
				break;
			}
			case DXGI_FORMAT_R32_TYPELESS:
			{
				this->_num_channels = 1;
				this->_channels[0] = {32, 0, 0, DATATYPE::TYPELESS, 'R'};
				break;
			}
			case DXGI_FORMAT_D32_FLOAT: // UNSUPPORTED
			{
				this->_num_channels = 1;
				this->_channels[0] = {32, 0, 0, DATATYPE::FLOAT, 'D'};
				break;
			}
			case DXGI_FORMAT_R32_FLOAT: // UNSUPPORTED
			{
				this->_num_channels = 1;
				this->_channels[0] = {32, 0, 0, DATATYPE::FLOAT, 'R'};
				break;
			}
			case DXGI_FORMAT_R32_UINT: // UNSUPPORTED
			{
				this->_num_channels = 1;
				this->_channels[0] = {32, 0, 0, DATATYPE::UINT, 'R'};
				break;
			}
			case DXGI_FORMAT_R32_SINT: // UNSUPPORTED
			{
				this->_num_channels = 1;
				this->_channels[0] = {32, 0, 0, DATATYPE::SINT, 'R'};
				break;
			}
			case DXGI_FORMAT_R24G8_TYPELESS:
			{
				this->_num_channels = 2;
				this->_channels[0] = {24, 0, 0, DATATYPE::TYPELESS, 'R'};
				this->_channels[1] = {8, 24, 1, DATATYPE::TYPELESS, 'G'};
				break;
			}
			case DXGI_FORMAT_D24_UNORM_S8_UINT: // UNSUPPORTED
			{
				this->_num_channels = 2;
				this->_channels[0] = {24, 0, 0, DATATYPE::UNORM, 'D'};
				this->_channels[1] = {8, 24, 1, DATATYPE::UINT,  'S'};
				break;
			}
			case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
			{
				this->_num_channels = 1;
				this->_channels[0] = {24, 0, 0, DATATYPE::UNORM, 'R'};
				break;
			}
			case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
			{
				this->_num_channels = 1;
				this->_channels[0] = {8, 24, 1, DATATYPE::UINT, 'G'};
				break;
			}
			case DXGI_FORMAT_R8G8_TYPELESS:
			{
				this->_num_channels = 2;
				this->_channels[0] = {8, 0, 0, DATATYPE::TYPELESS, 'R'};
				this->_channels[1] = {8, 8, 1, DATATYPE::TYPELESS, 'G'};
				break;
			}
			case DXGI_FORMAT_R8G8_UNORM: // XMUBYTEN2
			{
				this->_num_channels = 2;
				this->_channels[0] = {8, 0, 0, DATATYPE::UNORM, 'R'};
				this->_channels[1] = {8, 8, 1, DATATYPE::UNORM, 'G'};
				break;
			}
			case DXGI_FORMAT_R8G8_UINT: // XMUBYTE2
			{
				this->_num_channels = 2;
				this->_channels[0] = {8, 0, 0, DATATYPE::UINT, 'R'};
				this->_channels[1] = {8, 8, 1, DATATYPE::UINT, 'G'};
				break;
			}
			case DXGI_FORMAT_R8G8_SNORM: // XMBYTEN2
			{
				this->_num_channels = 2;
				this->_channels[0] = {8, 0, 0, DATATYPE::SNORM, 'R'};
				this->_channels[1] = {8, 8, 1, DATATYPE::SNORM, 'G'};
				break;
			}
			case DXGI_FORMAT_R8G8_SINT: // XMBYTE2
			{
				this->_num_channels = 2;
				this->_channels[0] = {8, 0, 0, DATATYPE::SINT, 'R'};
				this->_channels[1] = {8, 8, 1, DATATYPE::SINT, 'G'};
				break;
			}
			case DXGI_FORMAT_R16_TYPELESS:
			{
				this->_num_channels = 1;
				this->_channels[0] = {16, 0, 0, DATATYPE::TYPELESS, 'R'};
				break;
			}
			case DXGI_FORMAT_R16_FLOAT: // UNSUPPORTED
			{
				this->_num_channels = 1;
				this->_channels[0] = {16, 0, 0, DATATYPE::FLOAT, 'R'};
				break;
			}
			case DXGI_FORMAT_D16_UNORM: // UNSUPPORTED
			{
				this->_num_channels = 1;
				this->_channels[0] = {16, 0, 0, DATATYPE::UNORM, 'D'};
				break;
			}
			case DXGI_FORMAT_R16_UNORM: // UNSUPPORTED
			{
				this->_num_channels = 1;
				this->_channels[0] = {16, 0, 0, DATATYPE::UNORM, 'R'};
				break;
			}
			case DXGI_FORMAT_R16_UINT: // UNSUPPORTED
			{
				this->_num_channels = 1;
				this->_channels[0] = {16, 0, 0, DATATYPE::UNORM, 'R'};
				break;
			}
			case DXGI_FORMAT_R16_SNORM: // UNSUPPORTED
			{
				this->_num_channels = 1;
				this->_channels[0] = {16, 0, 0, DATATYPE::SNORM, 'R'};
				break;
			}
			case DXGI_FORMAT_R16_SINT: // UNSUPPORTED
			{
				this->_num_channels = 1;
				this->_channels[0] = {16, 0, 0, DATATYPE::SINT, 'R'};
				break;
			}
			case DXGI_FORMAT_R8_TYPELESS:
			{
				this->_num_channels = 1;
				this->_channels[0] = {8, 0, 0, DATATYPE::TYPELESS, 'R'};
				break;
			}
			case DXGI_FORMAT_R8_UNORM: // UNSUPPORTED
			{
				this->_num_channels = 1;
				this->_channels[0] = {8, 0, 0, DATATYPE::UNORM, 'R'};
				break;
			}
			case DXGI_FORMAT_R8_UINT: // UNSUPPORTED
			{
				this->_num_channels = 1;
				this->_channels[0] = {8, 0, 0, DATATYPE::UINT, 'R'};
				break;
			}
			case DXGI_FORMAT_R8_SNORM: // UNSUPPORTED
			{
				this->_num_channels = 1;
				this->_channels[0] = {8, 0, 0, DATATYPE::SNORM, 'R'};
				break;
			}
			case DXGI_FORMAT_R8_SINT: // UNSUPPORTED
			{
				this->_num_channels = 1;
				this->_channels[0] = {8, 0, 0, DATATYPE::SINT, 'R'};
				break;
			}
			case DXGI_FORMAT_A8_UNORM: // UNSUPPORTED
			{
				this->_num_channels = 1;
				this->_channels[0] = {8, 0, 0, DATATYPE::UNORM, 'A'};
				break;
			}
			case DXGI_FORMAT_R1_UNORM: // UNSUPPORTED
			{
				this->_num_channels = 1;
				this->_channels[0] = {1, 0, 0, DATATYPE::UNORM, 'R'};
				break;
			}
			case DXGI_FORMAT_R9G9B9E5_SHAREDEXP: // XMFLOAT3SE
			{
				this->_num_channels = 4;
				this->_channels[0] = {9,  0, 0, DATATYPE::SHAREDEXP, 'R'};
				this->_channels[1] = {9,  9, 1, DATATYPE::SHAREDEXP, 'G'};
				this->_channels[2] = {9, 18, 2, DATATYPE::SHAREDEXP, 'B'};
				this->_channels[3] = {5, 27, 3, DATATYPE::SHAREDEXP, 'E'};
				break;
			}
			case DXGI_FORMAT_B5G6R5_UNORM: // XMU565
			{
				this->_num_channels = 3;
				this->_channels[0] = {5,  0, 2, DATATYPE::UNORM, 'B'};
				this->_channels[1] = {6,  5, 1, DATATYPE::UNORM, 'G'};
				this->_channels[2] = {5, 11, 0, DATATYPE::UNORM, 'R'};
				break;
			}
			case DXGI_FORMAT_B5G5R5A1_UNORM: // XMU555
			{
				this->_num_channels = 4;
				this->_channels[0] = {5,  0, 2, DATATYPE::UNORM, 'B'};
				this->_channels[1] = {5,  5, 1, DATATYPE::UNORM, 'G'};
				this->_channels[2] = {5, 10, 0, DATATYPE::UNORM, 'R'};
				this->_channels[3] = {1, 15, 3, DATATYPE::UNORM, 'A'};
				break;
			}
			case DXGI_FORMAT_B8G8R8A8_UNORM: // XMCOLOR
			{
				this->_num_channels = 4;
				this->_channels[0] = {8,  0, 2, DATATYPE::UNORM, 'B'};
				this->_channels[1] = {8,  8, 1, DATATYPE::UNORM, 'G'};
				this->_channels[2] = {8, 16, 0, DATATYPE::UNORM, 'R'};
				this->_channels[3] = {8, 24, 3, DATATYPE::UNORM, 'A'};
				break;
			}
			case DXGI_FORMAT_B8G8R8X8_UNORM: // XMCOLOR
			{
				this->_num_channels = 3;
				this->_channels[0] = {8,  0, 2, DATATYPE::UNORM, 'B'};
				this->_channels[1] = {8,  8, 1, DATATYPE::UNORM, 'G'};
				this->_channels[2] = {8, 16, 0, DATATYPE::UNORM, 'R'};
				break;
			}
			case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM: // XMUBYTEN4 and use XMLoadUDecN4_XR and XMStoreUDecN4_XR
			{
				this->_num_channels = 4;
				this->_channels[0] = {10,  0, 0, DATATYPE::XR_BIAS, 'R'};
				this->_channels[1] = {10, 10, 1, DATATYPE::XR_BIAS, 'G'};
				this->_channels[2] = {10, 20, 2, DATATYPE::XR_BIAS, 'B'};
				this->_channels[3] = { 2, 30, 3, DATATYPE::UNORM,   'A'};
				break;
			}
			case DXGI_FORMAT_B8G8R8A8_TYPELESS:
			{
				this->_num_channels = 4;
				this->_channels[0] = {8,  0, 2, DATATYPE::TYPELESS, 'B'};
				this->_channels[1] = {8,  8, 1, DATATYPE::TYPELESS, 'G'};
				this->_channels[2] = {8, 16, 0, DATATYPE::TYPELESS, 'R'};
				this->_channels[3] = {8, 24, 3, DATATYPE::TYPELESS, 'A'};
				break;
			}
			case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB: // UNSUPPORTED
			{
				this->_num_channels = 4;
				this->_channels[0] = {8,  0, 2, DATATYPE::SRGB, 'B'};
				this->_channels[1] = {8,  8, 1, DATATYPE::SRGB, 'G'};
				this->_channels[2] = {8, 16, 0, DATATYPE::SRGB, 'R'};
				this->_channels[3] = {8, 24, 3, DATATYPE::SRGB, 'A'};
				break;
			}
			case DXGI_FORMAT_B8G8R8X8_TYPELESS:
			{
				this->_num_channels = 3;
				this->_channels[0] = {8,  0, 2, DATATYPE::TYPELESS, 'B'};
				this->_channels[1] = {8,  8, 1, DATATYPE::TYPELESS, 'G'};
				this->_channels[2] = {8, 16, 0, DATATYPE::TYPELESS, 'R'};
				break;
			}
			case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB: // UNSUPPORTED
			{
				this->_num_channels = 3;
				this->_channels[0] = {8,  0, 2, DATATYPE::SRGB, 'B'};
				this->_channels[1] = {8,  8, 1, DATATYPE::SRGB, 'G'};
				this->_channels[2] = {8, 16, 0, DATATYPE::SRGB, 'R'};
				break;
			}
			case DXGI_FORMAT_B4G4R4A4_UNORM: // XMUNIBBLE4
			{
				this->_num_channels = 4;
				this->_channels[0] = {4,  0, 2, DATATYPE::UNORM, 'B'};
				this->_channels[1] = {4,  4, 1, DATATYPE::UNORM, 'G'};
				this->_channels[2] = {4,  8, 0, DATATYPE::UNORM, 'R'};
				this->_channels[3] = {4, 12, 3, DATATYPE::UNORM, 'A'};
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
				                        g_format_map.FindStringFromID(fmt).c_str());
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

	
	void DXGIPixel::ExtractChannels(const size_t* ch_idx, const size_t* out_idx, size_t num_idx, mxArray*& out, mxClassID out_class)
	{
		size_t i, j, k, l;
		size_t src_idx;
		mwIndex dst_idx;
		
		DXGIPixel::DATATYPE input_datatype;
		
		StorageFunction storage_function = nullptr;
		
		/* find max output index */
		size_t max_out_idx = 0;
		for(i = 0; i < num_idx; i++)
		{
			max_out_idx = (out_idx[i] > max_out_idx)? out_idx[i] : max_out_idx;
		}
		
		const mwSize out_dims[] = {this->_image->height, this->_image->width, max_out_idx + 1};
		mwSize ndim = ARRAYSIZE(out_dims);
		
		/* check channels requested are inside bounds */
		for(i = 0; i < num_idx; i++)
		{
			if(ch_idx[i] >= this->_num_channels)
			{
				MEXError::PrintMexError(MEU_FL,
				                        MEU_SEVERITY_USER,
				                        "InvalidChannelError",
				                        "Channel index %llu exceeds the bounds of available channels. Restrict "
					                   "your selection to between 0 and %llu.",
				                        ch_idx[i], this->_num_channels);
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
			input_datatype = this->_channels[ch_idx[0]].datatype;
			for(i = 0; i < num_idx; i++)
			{
				if(this->_channels[ch_idx[i]].datatype != input_datatype)
				{
					MEXError::PrintMexError(MEU_FL,
					                        MEU_SEVERITY_USER,
					                        "IncompatibleDatatypesError",
					                        "The datatypes of the format must be uniform to output to a single matrix.");
					break;
				}
			}
		}
		
		if(out_class != mxUNKNOWN_CLASS)
		{
			switch(out_class)
			{
				case mxINT8_CLASS:
				case mxINT16_CLASS:
				case mxINT32_CLASS:
				case mxUINT8_CLASS:
				case mxUINT16_CLASS:
				case mxUINT32_CLASS:
				case mxSINGLE_CLASS:
				case mxDOUBLE_CLASS:
				{
					out = mxCreateNumericArray(ndim, out_dims, out_class, mxREAL);
					break;
				}
				case mxLOGICAL_CLASS:
				{
					out = mxCreateLogicalArray(ndim, out_dims);
					break;
				}
				case mxINT64_CLASS:
				case mxUINT64_CLASS:
				{
					MEXError::PrintMexError(MEU_FL,
					                        MEU_SEVERITY_USER,
					                        "InvalidOutputClassError",
					                        "Invalid output class. 64-bit integers are not supported.");
				}
				default:
				{
					MEXError::PrintMexError(MEU_FL,
					                        MEU_SEVERITY_USER,
					                        "InvalidOutputClassError",
					                        "Invalid output class (%u)",
					                        out_class);
				}
			}
			storage_function = GetStorageFunction(input_datatype, out_class);
		}
		else
		{
			/* Determine datatype of MATLAB output.
			 * Try to do this losslessly unless otherwise specified. */
			switch(input_datatype)
			{
				case DATATYPE::SNORM:
				{
					size_t max_width = DirectX::BitsPerColor(this->_format);

					/* determine MATLAB class width */
					if(max_width <= 8)
					{
						out = mxCreateNumericArray(ndim, out_dims, mxINT8_CLASS, mxREAL);
						storage_function = &DXGIPixel::ChannelElement<DATATYPE::SNORM, mxInt8>::StoreMX;
					}
					else if(max_width <= 16)
					{
						out = mxCreateNumericArray(ndim, out_dims, mxINT16_CLASS, mxREAL);
						storage_function = &DXGIPixel::ChannelElement<DATATYPE::SNORM, mxInt16>::StoreMX;
					}
					else
					{
						out = mxCreateNumericArray(ndim, out_dims, mxINT32_CLASS, mxREAL);
						storage_function = &DXGIPixel::ChannelElement<DATATYPE::SNORM, mxInt32>::StoreMX;
					}
					break;
				}
				case DATATYPE::UNORM:
				{
					size_t max_width = DirectX::BitsPerColor(this->_format);

					/* determine MATLAB class width */
					if(max_width == 1)
					{
						out = mxCreateLogicalArray(ndim, out_dims);
						storage_function = &DXGIPixel::ChannelElement<DATATYPE::UNORM, mxLogical>::StoreMX;
					}
					else if(max_width <= 8)
					{
						out = mxCreateNumericArray(ndim, out_dims, mxUINT8_CLASS, mxREAL);
						storage_function = &DXGIPixel::ChannelElement<DATATYPE::UNORM, mxUint8>::StoreMX;
					}
					else if(max_width <= 16)
					{
						out = mxCreateNumericArray(ndim, out_dims, mxUINT16_CLASS, mxREAL);
						storage_function = &DXGIPixel::ChannelElement<DATATYPE::UNORM, mxUint16>::StoreMX;
					}
					else
					{
						out = mxCreateNumericArray(ndim, out_dims, mxUINT32_CLASS, mxREAL);
						storage_function = &DXGIPixel::ChannelElement<DATATYPE::UNORM, mxUint32>::StoreMX;
					}
					break;
				}
				case DATATYPE::FLOAT:
				{
					out = mxCreateNumericArray(ndim, out_dims, mxSINGLE_CLASS, mxREAL);
					storage_function = &DXGIPixel::ChannelElement<DATATYPE::FLOAT, mxSingle>::StoreMX;
					break;
				}
				case DATATYPE::SRGB:
				{
					out = mxCreateNumericArray(ndim, out_dims, mxUINT8_CLASS, mxREAL);
					storage_function = &DXGIPixel::ChannelElement<DATATYPE::SRGB, mxUint8>::StoreMX;
					break;
				}
				case DATATYPE::SHAREDEXP:
				{
					/* special case */
					
					if(this->_format != DXGI_FORMAT_R9G9B9E5_SHAREDEXP)
					{
						MEXError::PrintMexError(MEU_FL,
						                        MEU_SEVERITY_INTERNAL,
						                        "UnexpectedFormatError",
						                        "Unexpected shared exponent format (%u). Cannot continue.",
						                        this->_format);
					}
					
					/* recalculate max output index ignoring indices referring to the shared exponent */
					max_out_idx = 0;
					for(i = 0; i < num_idx; i++)
					{
						if(ch_idx[i] != 3)
						{
							max_out_idx = (out_idx[i] > max_out_idx)? out_idx[i] : max_out_idx;
						}
					}
					
					out = mxCreateNumericArray(ndim, out_dims, mxSINGLE_CLASS, mxREAL);
					
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
					float scalar;
					for(src_idx = 0, dst_idx = 0; src_idx < this->_num_pixels; src_idx++)
					{
						uint32_t pixel_data = *(pixels + src_idx);
						int32_t exp_bits = (pixel_data & SHAREDEXP_E_MASK) >> 27u;
						if(exp_bits == 0)
						{
							/* this format has denorm support */
							scalar = (float)std::pow(2.0f, 1 - SHAREDEXP_BIAS);
						}
						else
						{
							scalar = (float)std::pow(2.0f, exp_bits - SHAREDEXP_BIAS);
						}
						
						for(j = 0; j < num_idx; j++)
						{
							if(ch_idx[src_idx] != 3)
							{
								data[dst_idx + out_idx[j]*this->_num_pixels] = ((pixel_data & masks[ch_idx[j]]) >> this->_channels[ch_idx[j]].offset)*scalar;
							}
						}
						dst_idx += this->_image->height;
						if(dst_idx >= this->_num_pixels)
						{
							dst_idx = src_idx / this->_image->width;
						}
					}
					return; // EARLY RETURN
				}
				case DATATYPE::XR_BIAS:
				{
					out = mxCreateNumericArray(ndim, out_dims, mxSINGLE_CLASS, mxREAL);
					storage_function = &DXGIPixel::ChannelElement<DATATYPE::XR_BIAS, mxSingle>::StoreMX;
					break;
				}
				case DATATYPE::SINT:
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
						storage_function = &DXGIPixel::ChannelElement<DATATYPE::SINT, mxLogical>::StoreMX;
					}
					else if(max_width <= 8)
					{
						out = mxCreateNumericArray(ndim, out_dims, mxINT8_CLASS, mxREAL);
						storage_function = &DXGIPixel::ChannelElement<DATATYPE::SINT, mxInt8>::StoreMX;
					}
					else if(max_width <= 16)
					{
						out = mxCreateNumericArray(ndim, out_dims, mxINT16_CLASS, mxREAL);
						storage_function = &DXGIPixel::ChannelElement<DATATYPE::SINT, mxInt16>::StoreMX;
					}
					else
					{
						out = mxCreateNumericArray(ndim, out_dims, mxINT32_CLASS, mxREAL);
						storage_function = &DXGIPixel::ChannelElement<DATATYPE::SINT, mxInt32>::StoreMX;
					}
					break;
				}
				case DATATYPE::UINT:
				{
					size_t max_width = DirectX::BitsPerColor(this->_format);
					
					/* determine MATLAB class width */
					if(max_width == 1)
					{
						out = mxCreateLogicalArray(ndim, out_dims);
						storage_function = &DXGIPixel::ChannelElement<DATATYPE::UINT, mxLogical>::StoreMX;
					}
					else if(max_width <= 8)
					{
						out = mxCreateNumericArray(ndim, out_dims, mxUINT8_CLASS, mxREAL);
						storage_function = &DXGIPixel::ChannelElement<DATATYPE::UINT, mxUint8>::StoreMX;
					}
					else if(max_width <= 16)
					{
						out = mxCreateNumericArray(ndim, out_dims, mxUINT16_CLASS, mxREAL);
						storage_function = &DXGIPixel::ChannelElement<DATATYPE::UINT, mxUint16>::StoreMX;
					}
					else
					{
						out = mxCreateNumericArray(ndim, out_dims, mxUINT32_CLASS, mxREAL);
						storage_function = &DXGIPixel::ChannelElement<DATATYPE::UINT, mxUint32>::StoreMX;
					}
					break;
				}
				case DATATYPE::TYPELESS:
				{
					size_t max_width = DirectX::BitsPerColor(this->_format);
					
					/* determine MATLAB class width */
					if(max_width == 1)
					{
						out = mxCreateLogicalArray(ndim, out_dims);
						storage_function = &DXGIPixel::ChannelElement<DATATYPE::TYPELESS, mxLogical>::StoreMX;
					}
					else if(max_width <= 8)
					{
						out = mxCreateNumericArray(ndim, out_dims, mxUINT8_CLASS, mxREAL);
						storage_function = &DXGIPixel::ChannelElement<DATATYPE::TYPELESS, mxUint8>::StoreMX;
					}
					else if(max_width <= 16)
					{
						out = mxCreateNumericArray(ndim, out_dims, mxUINT16_CLASS, mxREAL);
						storage_function = &DXGIPixel::ChannelElement<DATATYPE::TYPELESS, mxUint16>::StoreMX;
					}
					else
					{
						out = mxCreateNumericArray(ndim, out_dims, mxUINT32_CLASS, mxREAL);
						storage_function = &DXGIPixel::ChannelElement<DATATYPE::TYPELESS, mxUint32>::StoreMX;
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
		}
		
		if(out == nullptr)
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_SYSTEM, "NullOutputError", "Could not allocate the image matrix. Your system may be out of memory.");
		}
		
		void* data = mxGetData(out);
		switch(this->_pixel_bit_width)
		{
			case 128: /* always uniform width with 4 channels */
			case 96:  /* always uniform width with 3 channels*/
			{
				this->StoreUniformChannels<uint32_t>(ch_idx, out_idx, num_idx, data, storage_function);
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
							this->StoreUniformChannels<uint32_t>(ch_idx, out_idx, num_idx, data, storage_function);
							break;
						}
						case 4:
						{
							this->StoreUniformChannels<uint16_t>(ch_idx, out_idx, num_idx, data, storage_function);
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
					ChannelExtractor<uint64_t> extractor(reinterpret_cast<uint64_t*>(this->_image->pixels), this->_channels, this->_num_channels);
					for(src_idx = 0, dst_idx = 0; src_idx < this->_num_pixels; src_idx++, dst_idx += this->_image->height)
					{
						if(dst_idx >= this->_num_pixels)
						{
							dst_idx = src_idx / this->_image->width;
						}
						for(j = 0; j < num_idx; j++)
						{
							storage_function(data, 
										  dst_idx + out_idx[j] * this->_num_pixels, 
										  extractor.Extract(src_idx, ch_idx[j]),
										  this->_channels[ch_idx[j]].width);
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
							this->StoreUniformChannels<uint32_t>(ch_idx, out_idx, num_idx, data, storage_function);
							break;
						}
						case 2:
						{
							this->StoreUniformChannels<uint16_t>(ch_idx, out_idx, num_idx, data, storage_function);
							break;
						}
						case 3:
						case 4:
						{
							this->StoreUniformChannels<uint8_t>(ch_idx, out_idx, num_idx, data, storage_function);
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
					ChannelExtractor<uint32_t> extractor(reinterpret_cast<uint32_t*>(this->_image->pixels), this->_channels, this->_num_channels);
					for(src_idx = 0, dst_idx = 0; src_idx < this->_num_pixels; src_idx++, dst_idx += this->_image->height)
					{
						if(dst_idx >= this->_num_pixels)
						{
							dst_idx = src_idx / this->_image->width;
						}
						for(j = 0; j < num_idx; j++)
						{
							storage_function(data, 
										  dst_idx + out_idx[j] * this->_num_pixels, 
										  extractor.Extract(src_idx, ch_idx[j]),
										  this->_channels[ch_idx[j]].width);
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
							this->StoreUniformChannels<uint16_t>(ch_idx, out_idx, num_idx, data, storage_function);
							break;
						}
						case 2:
						{
							this->StoreUniformChannels<uint8_t>(ch_idx, out_idx, num_idx, data, storage_function);
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
					ChannelExtractor<uint16_t> extractor(reinterpret_cast<uint16_t*>(this->_image->pixels), this->_channels, this->_num_channels);
					for(src_idx = 0, dst_idx = 0; src_idx < this->_num_pixels; src_idx++, dst_idx += this->_image->height)
					{
						if(dst_idx >= this->_num_pixels)
						{
							dst_idx = src_idx / this->_image->width;
						}
						for(j = 0; j < num_idx; j++)
						{
							storage_function(data, 
										  dst_idx + out_idx[j] * this->_num_pixels, 
										  extractor.Extract(src_idx, ch_idx[j]),
										  this->_channels[ch_idx[j]].width);
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
							this->StoreUniformChannels<uint8_t>(ch_idx, out_idx, num_idx, data, storage_function);
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
				mxLogical channel_data;
				
				src_idx = 0;
				dst_idx = 0;
				for(i = 0; i < this->_image->height; i++)
				{
					/* 32-bit width is fastest on x86-64 and i686 */
					auto pixels32 = (uint32_t*)(this->_image->pixels + i*this->_image->rowPitch);
					for(j = 0; j < this->_image->rowPitch / 4; j++)
					{
						uint32_t page = pixels32[j];
						for(k = 32; k > 0; k--, src_idx++, dst_idx += this->_image->height)
						{
							if(dst_idx >= this->_num_pixels)
							{
								dst_idx = src_idx / this->_image->width;
							}
							/* there should only be one channel, but it may be repeatedly stored */
							channel_data = ((page >> (k - 1)) & 1u) != 0;
							for(l = 0; l < num_idx; l++)
							{
								data_l[dst_idx + out_idx[l] * this->_num_pixels] = channel_data;
							}
						}
					}
					auto pixels8 = (uint8_t*)pixels32;
					for(j *= 4; j < this->_image->rowPitch - 1; j++)
					{
						uint8_t page = pixels8[j];
						for(k = 8; k > 0; k--, src_idx++, dst_idx += this->_image->height)
						{
							if(dst_idx >= this->_num_pixels)
							{
								dst_idx = src_idx / this->_image->width;
							}
							/* there should only be one channel, but it may be repeatedly stored */
							channel_data = ((uint8_t)(page >> (k - 1)) & 1u) != 0;
							for(l = 0; l < num_idx; l++)
							{
								data_l[dst_idx + out_idx[l] * this->_num_pixels] = channel_data;
							}
						}
					}
					uint8_t page = pixels8[j];
					for(k = this->_image->width - ((this->_image->rowPitch - 1) * 8); k > 0; k--, src_idx++, dst_idx += this->_image->height)
					{
						if(dst_idx >= this->_num_pixels)
						{
							dst_idx = src_idx / this->_image->width;
						}

						/* there should only be one channel, but it may be repeatedly stored */
						channel_data = ((uint8_t)(page >> (k - 1)) & 1u) != 0;
						for(l = 0; l < num_idx; l++)
						{
							data_l[dst_idx + out_idx[l] * this->_num_pixels] = channel_data;
						}
					}
				}
			}
			default:
			{
				MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_INTERNAL, "PixelWidthError", "Unexpected pixel width %u was encountered. Cannot continue.", this->_pixel_bit_width);
			}
		}
	}
	
	void DXGIPixel::ExtractRGB(mxArray*& mx_rgb)
	{
		size_t ch_idx[MAX_CHANNELS];
		size_t out_idx[MAX_CHANNELS];
		if(DirectX::HasAlpha(this->_format))
		{
			for(size_t i = 0, j = 0; i < this->_num_channels; i++)
			{
				if(this->_channels[i].name != 'A')
				{
					ch_idx[j] = i;
					out_idx[j] = this->_channels[i].standard_idx;
					j++;
				}
			}
			this->ExtractChannels(ch_idx, out_idx, this->_num_channels - 1, mx_rgb);
		}
		else
		{
			for(size_t i = 0; i < this->_num_channels; i++)
			{
				ch_idx[i] = i;
				out_idx[i] = this->_channels[i].standard_idx;
			}
			this->ExtractChannels(ch_idx, out_idx, this->_num_channels, mx_rgb);
		}
	}
	
	void DXGIPixel::ExtractRGBA(mxArray*& mx_rgba)
	{
		size_t ch_idx[MAX_CHANNELS];
		size_t out_idx[MAX_CHANNELS];
		for(size_t i = 0; i < this->_num_channels; i++)
		{
			ch_idx[i] = i;
			out_idx[i] = this->_channels[i].standard_idx;
		}
		this->ExtractChannels(ch_idx, out_idx, this->_num_channels, mx_rgba);
	}
	
	void DXGIPixel::ExtractRGBA(mxArray*& mx_rgb, mxArray*& mx_a)
	{
		size_t ch_idx[MAX_CHANNELS];
		size_t out_idx[MAX_CHANNELS];
		if(DirectX::HasAlpha(this->_format))
		{
			for(size_t i = 0, j = 0; i < this->_num_channels; i++)
			{
				if(this->_channels[i].name != 'A')
				{
					ch_idx[j] = i;
					out_idx[j] = this->_channels[i].standard_idx;
					j++;
				}
				else
				{
					this->ExtractChannels(i, 0, mx_a);
				}
			}
			this->ExtractChannels(ch_idx, out_idx, this->_num_channels - 1, mx_rgb);
		}
		else
		{
			for(size_t i = 0; i < this->_num_channels; i++)
			{
				ch_idx[i] = i;
				out_idx[i] = this->_channels[i].standard_idx;
			}
			this->ExtractChannels(ch_idx, out_idx, this->_num_channels, mx_rgb);
			mx_a = mxCreateDoubleMatrix(0, 0, mxREAL);
		}
	}

	void DXGIPixel::ExtractAll(mxArray*& mx_out)
	{
		size_t ch_idx[MAX_CHANNELS];
		for (size_t i = 0; i < this->_num_channels; i++)
		{
			ch_idx[i] = i;
		}
		this->ExtractChannels(ch_idx, ch_idx, this->_num_channels, mx_out);
	}

	
	
	DXGIPixel::StorageFunction DXGIPixel::GetStorageFunction(const DATATYPE in_type, mxClassID out_type)
	{
		switch(in_type)
		{
			case DATATYPE::TYPELESS: return GetStorageFunction<DATATYPE::TYPELESS>(out_type);
			case DATATYPE::SNORM: return GetStorageFunction<DATATYPE::SNORM>(out_type);
			case DATATYPE::UNORM: return GetStorageFunction<DATATYPE::UNORM>(out_type);
			case DATATYPE::SINT: return GetStorageFunction<DATATYPE::SINT>(out_type);
			case DATATYPE::UINT: return GetStorageFunction<DATATYPE::UINT>(out_type);
			case DATATYPE::FLOAT: return GetStorageFunction<DATATYPE::FLOAT>(out_type);
			case DATATYPE::SRGB: return GetStorageFunction<DATATYPE::SRGB>(out_type);
			case DATATYPE::SHAREDEXP: return GetStorageFunction<DATATYPE::SHAREDEXP>(out_type);
			case DATATYPE::XR_BIAS: return GetStorageFunction<DATATYPE::XR_BIAS>(out_type);
			default:
			{
				MEXError::PrintMexError(MEU_FL,
								    MEU_SEVERITY_INTERNAL,
								    "UnexpectedInputTypeError",
								    "Unexpected input datatype (%u).",
								    in_type);
			}
		}
		return nullptr;
	}

	template <DXGIPixel::DATATYPE IN_TYPE>
	DXGIPixel::StorageFunction DXGIPixel::GetStorageFunction(mxClassID out_type)
	{
		switch (out_type)
		{
			case mxINT8_CLASS:    return &ChannelElement<IN_TYPE, mxInt8>::StoreMX;
			case mxINT16_CLASS:   return &ChannelElement<IN_TYPE, mxInt16>::StoreMX;
			case mxINT32_CLASS:   return &ChannelElement<IN_TYPE, mxInt32>::StoreMX;
			case mxUINT8_CLASS:   return &ChannelElement<IN_TYPE, mxUint8>::StoreMX;
			case mxUINT16_CLASS:  return &ChannelElement<IN_TYPE, mxUint16>::StoreMX;
			case mxUINT32_CLASS:  return &ChannelElement<IN_TYPE, mxUint32>::StoreMX;
			case mxSINGLE_CLASS:  return &ChannelElement<IN_TYPE, mxSingle>::StoreMX;
			case mxDOUBLE_CLASS:  return &ChannelElement<IN_TYPE, mxDouble>::StoreMX;
			case mxLOGICAL_CLASS: return &ChannelElement<IN_TYPE, mxLogical>::StoreMX;
			default:
			{ 
				MEXError::PrintMexError(MEU_FL,
					MEU_SEVERITY_INTERNAL,
					"UnexpectedOutputTypeError",
					"Unexpected datatype requested for output (%u).",
					out_type);
			}
		}
		return nullptr;
	}

