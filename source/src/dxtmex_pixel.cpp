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
			case DXGI_FORMAT_D32_FLOAT_S8X24_UINT: // UNSUPPORTED
			{
				this->_num_channels = 2;
				this->_channels[0] = {32, 0, 0, DXGIPixel::FLOAT, 'D'};
				this->_channels[1] = {8, 32, 1, DXGIPixel::UINT, 'S'};
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
			case DXGI_FORMAT_D24_UNORM_S8_UINT: // UNSUPPORTED
			{
				this->_num_channels = 2;
				this->_channels[0] = {24, 0, 0, DXGIPixel::UNORM, 'D'};
				this->_channels[1] = {8, 24, 1, DXGIPixel::UINT, 'S'};
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
			case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB: // UNSUPPORTED
			{
				this->_num_channels = 4;
				this->_channels[0] = {8,  0, 2, DXGIPixel::SRGB, 'B'};
				this->_channels[1] = {8,  8, 1, DXGIPixel::SRGB, 'G'};
				this->_channels[2] = {8, 16, 0, DXGIPixel::SRGB, 'R'};
				this->_channels[3] = {8, 24, 3, DXGIPixel::SRGB, 'A'};
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
	}
	
	
	uint32_t DXGIPixel::ExtractPixelChannel(size_t idx, int ch_num)
	{
		DXGIPixel::PixelChannel &ref_channel = this->_channels[ch_num];
		size_t bit_offset = idx*this->_pixel_width + ref_channel.offset;
		size_t byte_offset = bit_offset/sizeof(uint8_t);
		uint32_t mod_bit_offset = bit_offset%sizeof(uint8_t);
		
		if(mod_bit_offset + ref_channel.width > sizeof(uint32_t))
		{
			MEXError::PrintMexError(MEU_FL,
			                        MEU_SEVERITY_INTERNAL,
			                        "UnexpectedFormatError",
			                        "A channel could not be extracted because of an internal error with bit indexing.");
		}
		
		uint32_t mask =
		(((uint32_t)1 << ref_channel.width) - (uint32_t)1) << mod_bit_offset;
		return (*(uint32_t*)(this->_image->pixels + byte_offset) & mask)
		>> mod_bit_offset;
	}
	
	
	void DXGIPixel::ExtractChannel(const size_t* ch_nums, size_t num_ch, mxArray*& out, mxClassID out_class)
	{
		DXGIPixel::DATATYPE input_datatype;
		
		typedef void (*StorageFunction)(void*, mwIndex, uint32_t);
		void (*storage_functions[4])(void*, mwIndex, uint32_t);
		storage_functions[0] = &DXGIPixel::ChannelElement<5, DXGIPixel::SNORM, float>::Store;
		StorageFunction storage_function = &DXGIPixel::ChannelElement<5, DXGIPixel::SNORM, float>::Store;
		
		/* Determine datatype of MATLAB output.
		 * Try to do this losslessly unless otherwise specified. */
		
		const mwSize dims[] = {this->_image->height, this->_image->width, num_ch};
		mwSize ndim = ARRAYSIZE(dims);
		
		/* check channels requested are inside bounds */
		for(size_t i = 0; i < num_ch; i++)
		{
			if(ch_nums[i] >= this->_num_channels)
			{
				MEXError::PrintMexError(MEU_FL,
				                        MEU_SEVERITY_USER,
				                        "InvalidChannelError",
				                        "Channel index %llu exceeds the bounds of available channels. Restrict "
					                   "your selection to between 0 and %llu.",
				                        ch_nums[i], this->_num_channels);
			}
		}
		
		if(this->_has_uniform_datatype)
		{
			input_datatype = this->_channels[0].datatype;
		}
		else
		{
			input_datatype = this->_channels[ch_nums[0]].datatype;
			for(size_t i = 0; i < num_ch; i++)
			{
				if(this->_channels[ch_nums[0]].datatype != input_datatype)
				{
					MEXError::PrintMexError(MEU_FL,
					                        MEU_SEVERITY_USER,
					                        "IncompatibleDatatypesError",
					                        "The datatypes of the format must be uniform to output to a single matrix.");
					break;
				}
			}
		}
		
		/* determine output datatype */
		switch(input_datatype)
		{
			case DXGIPixel::SNORM:
			{
				out = mxCreateNumericArray(ndim, dims, mxSINGLE_CLASS, mxREAL);
				storage_function = &DXGIPixel::StoreSNORMAsSingle;
				break;
			}
			case DXGIPixel::UNORM:
			{
				out = mxCreateNumericArray(ndim, dims, mxSINGLE_CLASS, mxREAL);
				storage_function = &DXGIPixel::StoreUNORMAsSingle;
				break;
			}
			case DXGIPixel::FLOAT:
			{
				out = mxCreateNumericArray(ndim, dims, mxSINGLE_CLASS, mxREAL);
				storage_function = &DXGIPixel::StoreFLOATAsSingle;
				break;
			}
			case DXGIPixel::SRGB:
			{
				out = mxCreateNumericArray(ndim, dims, mxSINGLE_CLASS, mxREAL);
				storage_function = &DXGIPixel::StoreSRGBAsSingle;
				break;
			}
			case DXGIPixel::SHAREDEXP:
			{
				out = mxCreateNumericArray(ndim, dims, mxSINGLE_CLASS, mxREAL);
				storage_function = &DXGIPixel::StoreSHAREDEXPAsSingle;
				break;
			}
			case DXGIPixel::XR_BIAS:
			{
				out = mxCreateNumericArray(ndim, dims, mxSINGLE_CLASS, mxREAL);
				storage_function = &DXGIPixel::StoreXR_BIASAsSingle;
				break;
			}
			case DXGIPixel::SINT:
			{
				/* get max width of selected channels */
				size_t max_width = 0;
				for(size_t i = 0; i < num_ch; i++)
				{
					max_width = (this->_channels[ch_nums[i]].width > max_width)? this->_channels[ch_nums[i]].width : max_width;
				}
				
				/* determine MATLAB class width */
				if(max_width == 1)
				{
					out = mxCreateLogicalArray(ndim, dims);
					storage_function = &DXGIPixel::StoreSINTAsLogical;
				}
				else if(max_width <= 8)
				{
					out = mxCreateNumericArray(ndim, dims, mxINT8_CLASS, mxREAL);
					storage_function = &DXGIPixel::StoreSINTAsInt8;
				}
				else if(max_width <= 16)
				{
					out = mxCreateNumericArray(ndim, dims, mxINT16_CLASS, mxREAL);
					storage_function = &DXGIPixel::StoreSINTAsInt16;
				}
				else
				{
					out = mxCreateNumericArray(ndim, dims, mxINT32_CLASS, mxREAL);
					storage_function = &DXGIPixel::StoreSINTAsInt32;
				}
				break;
			}
			case DXGIPixel::UINT:
			{
				/* get max width of selected channels */
				size_t max_width = 0;
				for(size_t i = 0; i < num_ch; i++)
				{
					max_width = (this->_channels[ch_nums[i]].width > max_width)? this->_channels[ch_nums[i]].width : max_width;
				}
				
				/* determine MATLAB class width */
				if(max_width == 1)
				{
					out = mxCreateLogicalArray(ndim, dims);
					storage_function = &DXGIPixel::StoreUINTAsLogical;
				}
				else if(max_width <= 8)
				{
					out = mxCreateNumericArray(ndim, dims, mxUINT8_CLASS, mxREAL);
					storage_function = &DXGIPixel::StoreUINTAsUInt8;
				}
				else if(max_width <= 16)
				{
					out = mxCreateNumericArray(ndim, dims, mxUINT16_CLASS, mxREAL);
					storage_function = &DXGIPixel::StoreUINTAsUInt16;
				}
				else
				{
					out = mxCreateNumericArray(ndim, dims, mxUINT32_CLASS, mxREAL);
					storage_function = &DXGIPixel::StoreUINTAsUInt32;
				}
				break;
			}
			case DXGIPixel::TYPELESS:
			{
				/* get max width of selected channels */
				size_t max_width = 0;
				for(size_t i = 0; i < num_ch; i++)
				{
					max_width = (this->_channels[ch_nums[i]].width > max_width)? this->_channels[ch_nums[i]].width : max_width;
				}
				
				/* determine MATLAB class width */
				if(max_width == 1)
				{
					out = mxCreateLogicalArray(ndim, dims);
					storage_function = &DXGIPixel::StoreTYPELESSAsLogical;
				}
				else if(max_width <= 8)
				{
					out = mxCreateNumericArray(ndim, dims, mxUINT8_CLASS, mxREAL);
					storage_function = &DXGIPixel::StoreTYPELESSAsUInt8;
				}
				else if(max_width <= 16)
				{
					out = mxCreateNumericArray(ndim, dims, mxUINT16_CLASS, mxREAL);
					storage_function = &DXGIPixel::StoreTYPELESSAsUInt16;
				}
				else
				{
					out = mxCreateNumericArray(ndim, dims, mxUINT32_CLASS, mxREAL);
					storage_function = &DXGIPixel::StoreTYPELESSAsUInt32;
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
			case 128:
			{
				/* always uniform width with 4 channels */
				DXGIPixel::StoreUniformChannels(ch_nums, num_ch, this->_num_channels, (uint32_t*)this->_image->pixels, data);
				break;
			}
			case 96:
			{
				/* always uniform width with 3 channels*/
				DXGIPixel::StoreUniformChannels(ch_nums, num_ch, this->_num_channels, (uint32_t*)this->_image->pixels, data);
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
							DXGIPixel::StoreUniformChannels(ch_nums, num_ch, this->_num_channels, (uint32_t*)this->_image->pixels, data);
							break;
						}
						case 4:
						{
							DXGIPixel::StoreUniformChannels(ch_nums, num_ch, this->_num_channels, (uint16_t*)this->_image->pixels, data);
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
					
					for(size_t i = 0; i < this->_num_pixels; i++)
					{
						for(size_t j = 0; j < num_ch; j++)
						{
							mwIndex dst_idx = i / this->_image->width + (i % this->_image->width) * this->_image->height + j * this->_num_pixels;
							uint32_t channel_data = (*(pixels + i) & masks[ch_nums[j]]) >> this->_channels[ch_nums[j]].offset;
							storage_function(data, dst_idx, channel_data);
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
							DXGIPixel::StoreUniformChannels(ch_nums, num_ch, this->_num_channels, (uint32_t*)this->_image->pixels, data);
							break;
						}
						case 2:
						{
							DXGIPixel::StoreUniformChannels(ch_nums, num_ch, this->_num_channels, (uint16_t*)this->_image->pixels, data);
							break;
						}
						case 4:
						{
							DXGIPixel::StoreUniformChannels(ch_nums, num_ch, this->_num_channels, (uint8_t*)this->_image->pixels, data);
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
					
					for(size_t i = 0; i < this->_num_pixels; i++)
					{
						for(size_t j = 0; j < num_ch; j++)
						{
							mwIndex dst_idx = i / this->_image->width + (i % this->_image->width) * this->_image->height + j * this->_num_pixels;
							uint32_t channel_data = (*(pixels + i) & masks[ch_nums[j]]) >> this->_channels[ch_nums[j]].offset;
							storage_function(data, dst_idx, channel_data);
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
							DXGIPixel::StoreUniformChannels(ch_nums, num_ch, this->_num_channels, (uint16_t*)this->_image->pixels, data);
							break;
						}
						case 2:
						{
							DXGIPixel::StoreUniformChannels(ch_nums, num_ch, this->_num_channels, (uint8_t*)this->_image->pixels, data);
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
						masks[i] =(((uint16_t)1 << this->_channels[i].width) - (uint16_t)1) << this->_channels[i].offset;
					}
					
					for(size_t i = 0; i < this->_num_pixels; i++)
					{
						for(size_t j = 0; j < num_ch; j++)
						{
							mwIndex dst_idx = i / this->_image->width + (i % this->_image->width) * this->_image->height + j * this->_num_pixels;
							uint32_t channel_data = (*(pixels + i) & masks[ch_nums[j]]) >> this->_channels[ch_nums[j]].offset;
							storage_function(data, dst_idx, channel_data);
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
							DXGIPixel::StoreUniformChannels(ch_nums, num_ch, this->_num_channels, (uint8_t*)this->_image->pixels, data);
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
				/* use 32-bit uints because DWORD size is fastest on x86-64 and i686 */
				auto pixels32 = (uint32_t*)this->_image->pixels;
				size_t i;
				for(i = 0; i < this->_num_pixels / 32; i++)
				{
					uint32_t page = pixels32[i];
					for(size_t j = 0; j < 32; j++)
					{
						/* there should only be one channel, but it may be repeatedly stored */
						uint32_t channel_data = (page & ((uint32_t)1 << j)) != 0;
						for(size_t k = 0; k < num_ch; k++)
						{
							mwIndex dst_idx = i / this->_image->width + (i % this->_image->width) * this->_image->height + k * this->_num_pixels;
							storage_function(data, dst_idx, channel_data);
						}
					}
				}
			}
		}
	}
	
	
	/* TYPELESS conversions */
	template <int N, typename OUT_TYPE>
	struct DXGIPixel::ChannelElement<N, DXGIPixel::TYPELESS, OUT_TYPE>
	{
		static inline void Store(void* data, mwIndex dst_idx, uint32_t ir)
		{
			/* in the narrowing case this isn't portable */
			((OUT_TYPE*)data)[dst_idx] = *((OUT_TYPE*)&ir);
		}
	};
	
	/* SNORM conversions */
	template <int N>
	struct DXGIPixel::ChannelElement<N, DXGIPixel::SNORM, mxSingle>
	{
		static inline void Store(void* data, mwIndex dst_idx, uint32_t ir)
		{
			uint32_t MSB = (uint32_t)1 << (N - 1);
			if(ir == MSB)
			{
				return -1.0f;
			}
			mxSingle c = SignExtend(ir);
			((mxSingle*)data)[dst_idx] = c/(MSB - 1);
		}
	};
	
	/* UNORM conversions */
	template <int N>
	struct DXGIPixel::ChannelElement<N, DXGIPixel::UNORM, mxSingle>
	{
		static inline void Store(void* data, mwIndex dst_idx, uint32_t ir)
		{
			((mxSingle*)data)[dst_idx] = (mxSingle)ir/(((uint32_t)1 << N) - 1);
		}
	};
	
	/* SINT conversions */
	template <int N, typename OUT_TYPE>
	struct DXGIPixel::ChannelElement<N, DXGIPixel::SINT, OUT_TYPE>
	{
		static inline void Store(void* data, mwIndex dst_idx, uint32_t ir)
		{
			((OUT_TYPE*)data)[dst_idx] = SignExtend(ir);
		}
	};
	
	/* UINT conversions */
	template <int N, typename OUT_TYPE>
	struct DXGIPixel::ChannelElement<N, DXGIPixel::UINT, OUT_TYPE>
	{
		static inline void Store(void* data, mwIndex dst_idx, uint32_t ir)
		{
			((OUT_TYPE*)data)[dst_idx] = (OUT_TYPE)ir;
		}
	};
	
	/* FLOAT conversions */
	template <int N>
	struct DXGIPixel::ChannelElement<N, DXGIPixel::FLOAT, mxSingle>
	{
		static inline void Store(void* data, mwIndex dst_idx, uint32_t ir)
		{
			((float*)data)[dst_idx] = *((float*)&ir);
		}
	};
	
	/* SRGB conversions */
	template <int N>
	struct DXGIPixel::ChannelElement<N, DXGIPixel::SRGB, mxSingle>
	{
		static inline void Store(void* data, mwIndex dst_idx, uint32_t ir)
		{
			float c = (float)ir/(((uint32_t)1 << N) - 1);
			if(c <= D3D11_SRGB_TO_FLOAT_THRESHOLD)
			{
				((mxSingle*)data)[dst_idx] = c/D3D11_SRGB_TO_FLOAT_DENOMINATOR_1;
			}
			else
			{
				((mxSingle*)data)[dst_idx] = ((c + D3D11_SRGB_TO_FLOAT_OFFSET)/D3D11_SRGB_TO_FLOAT_DENOMINATOR_2)*D3D11_SRGB_TO_FLOAT_EXPONENT;
			}
		}
	};
	
	template <int N>
	struct DXGIPixel::ChannelElement<N, DXGIPixel::SRGB, mxUint8>
	{
		static inline void Store(void* data, mwIndex dst_idx, uint32_t ir)
		{
			((mxUint8*)data)[dst_idx] = (mxUint8)ir;
		}
	};
	
	template <int N>
	struct DXGIPixel::ChannelElement<N, DXGIPixel::XR_BIAS, mxSingle>
	{
		static inline void Store(void* data, mwIndex dst_idx, uint32_t ir)
		{
			((mxSingle*)data)[dst_idx] = (float)((ir & 0x3ff) - 0x180) / 510.f;
		}
	};
	
}

