#include "mex.h"
#include "dxtmex_maps.hpp"
#include "dxtmex_flags.hpp"
#include "dxtmex_mexutils.hpp"

using namespace DXTMEX;

namespace DXTMEX
{
	
	template class DXTFlags<DirectX::DDS_FLAGS>;
	template class DXTFlags<DirectX::TEX_FILTER_FLAGS>;
	template class DXTFlags<DirectX::TEX_FR_FLAGS>;
	template class DXTFlags<DirectX::TEX_PMALPHA_FLAGS>;
	template class DXTFlags<DirectX::TEX_COMPRESS_FLAGS>;
	template class DXTFlags<DirectX::CNMAP_FLAGS>;
	template class DXTFlags<DirectX::CMSE_FLAGS>;
	
	BiMap<DXGI_FORMAT, std::string> g_format_map{{DXGI_FORMAT_UNKNOWN,                    "UNKNOWN"},
	                                             {DXGI_FORMAT_R32G32B32A32_TYPELESS,      "R32G32B32A32_TYPELESS"},
	                                             {DXGI_FORMAT_R32G32B32A32_FLOAT,         "R32G32B32A32_FLOAT"},
	                                             {DXGI_FORMAT_R32G32B32A32_UINT,          "R32G32B32A32_UINT"},
	                                             {DXGI_FORMAT_R32G32B32A32_SINT,          "R32G32B32A32_SINT"},
	                                             {DXGI_FORMAT_R32G32B32_TYPELESS,         "R32G32B32_TYPELESS"},
	                                             {DXGI_FORMAT_R32G32B32_FLOAT,            "R32G32B32_FLOAT"},
	                                             {DXGI_FORMAT_R32G32B32_UINT,             "R32G32B32_UINT"},
	                                             {DXGI_FORMAT_R32G32B32_SINT,             "R32G32B32_SINT"},
	                                             {DXGI_FORMAT_R16G16B16A16_TYPELESS,      "R16G16B16A16_TYPELESS"},
	                                             {DXGI_FORMAT_R16G16B16A16_FLOAT,         "R16G16B16A16_FLOAT"},
	                                             {DXGI_FORMAT_R16G16B16A16_UNORM,         "R16G16B16A16_UNORM"},
	                                             {DXGI_FORMAT_R16G16B16A16_UINT,          "R16G16B16A16_UINT"},
	                                             {DXGI_FORMAT_R16G16B16A16_SNORM,         "R16G16B16A16_SNORM"},
	                                             {DXGI_FORMAT_R16G16B16A16_SINT,          "R16G16B16A16_SINT"},
	                                             {DXGI_FORMAT_R32G32_TYPELESS,            "R32G32_TYPELESS"},
	                                             {DXGI_FORMAT_R32G32_FLOAT,               "R32G32_FLOAT"},
	                                             {DXGI_FORMAT_R32G32_UINT,                "R32G32_UINT"},
	                                             {DXGI_FORMAT_R32G32_SINT,                "R32G32_SINT"},
	                                             {DXGI_FORMAT_R32G8X24_TYPELESS,          "R32G8X24_TYPELESS"},
	                                             {DXGI_FORMAT_D32_FLOAT_S8X24_UINT,       "D32_FLOAT_S8X24_UINT"},
	                                             {DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS,   "R32_FLOAT_X8X24_TYPELESS"},
	                                             {DXGI_FORMAT_X32_TYPELESS_G8X24_UINT,    "X32_TYPELESS_G8X24_UINT"},
	                                             {DXGI_FORMAT_R10G10B10A2_TYPELESS,       "R10G10B10A2_TYPELESS"},
	                                             {DXGI_FORMAT_R10G10B10A2_UNORM,          "R10G10B10A2_UNORM"},
	                                             {DXGI_FORMAT_R10G10B10A2_UINT,           "R10G10B10A2_UINT"},
	                                             {DXGI_FORMAT_R11G11B10_FLOAT,            "R11G11B10_FLOAT"},
	                                             {DXGI_FORMAT_R8G8B8A8_TYPELESS,          "R8G8B8A8_TYPELESS"},
	                                             {DXGI_FORMAT_R8G8B8A8_UNORM,             "R8G8B8A8_UNORM"},
	                                             {DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,        "R8G8B8A8_UNORM_SRGB"},
	                                             {DXGI_FORMAT_R8G8B8A8_UINT,              "R8G8B8A8_UINT"},
	                                             {DXGI_FORMAT_R8G8B8A8_SNORM,             "R8G8B8A8_SNORM"},
	                                             {DXGI_FORMAT_R8G8B8A8_SINT,              "R8G8B8A8_SINT"},
	                                             {DXGI_FORMAT_R16G16_TYPELESS,            "R16G16_TYPELESS"},
	                                             {DXGI_FORMAT_R16G16_FLOAT,               "R16G16_FLOAT"},
	                                             {DXGI_FORMAT_R16G16_UNORM,               "R16G16_UNORM"},
	                                             {DXGI_FORMAT_R16G16_UINT,                "R16G16_UINT"},
	                                             {DXGI_FORMAT_R16G16_SNORM,               "R16G16_SNORM"},
	                                             {DXGI_FORMAT_R16G16_SINT,                "R16G16_SINT"},
	                                             {DXGI_FORMAT_R32_TYPELESS,               "R32_TYPELESS"},
	                                             {DXGI_FORMAT_D32_FLOAT,                  "D32_FLOAT"},
	                                             {DXGI_FORMAT_R32_FLOAT,                  "R32_FLOAT"},
	                                             {DXGI_FORMAT_R32_UINT,                   "R32_UINT"},
	                                             {DXGI_FORMAT_R32_SINT,                   "R32_SINT"},
	                                             {DXGI_FORMAT_R24G8_TYPELESS,             "R24G8_TYPELESS"},
	                                             {DXGI_FORMAT_D24_UNORM_S8_UINT,          "D24_UNORM_S8_UINT"},
	                                             {DXGI_FORMAT_R24_UNORM_X8_TYPELESS,      "R24_UNORM_X8_TYPELESS"},
	                                             {DXGI_FORMAT_X24_TYPELESS_G8_UINT,       "X24_TYPELESS_G8_UINT"},
	                                             {DXGI_FORMAT_R8G8_TYPELESS,              "R8G8_TYPELESS"},
	                                             {DXGI_FORMAT_R8G8_UNORM,                 "R8G8_UNORM"},
	                                             {DXGI_FORMAT_R8G8_UINT,                  "R8G8_UINT"},
	                                             {DXGI_FORMAT_R8G8_SNORM,                 "R8G8_SNORM"},
	                                             {DXGI_FORMAT_R8G8_SINT,                  "R8G8_SINT"},
	                                             {DXGI_FORMAT_R16_TYPELESS,               "R16_TYPELESS"},
	                                             {DXGI_FORMAT_R16_FLOAT,                  "R16_FLOAT"},
	                                             {DXGI_FORMAT_D16_UNORM,                  "D16_UNORM"},
	                                             {DXGI_FORMAT_R16_UNORM,                  "R16_UNORM"},
	                                             {DXGI_FORMAT_R16_UINT,                   "R16_UINT"},
	                                             {DXGI_FORMAT_R16_SNORM,                  "R16_SNORM"},
	                                             {DXGI_FORMAT_R16_SINT,                   "R16_SINT"},
	                                             {DXGI_FORMAT_R8_TYPELESS,                "R8_TYPELESS"},
	                                             {DXGI_FORMAT_R8_UNORM,                   "R8_UNORM"},
	                                             {DXGI_FORMAT_R8_UINT,                    "R8_UINT"},
	                                             {DXGI_FORMAT_R8_SNORM,                   "R8_SNORM"},
	                                             {DXGI_FORMAT_R8_SINT,                    "R8_SINT"},
	                                             {DXGI_FORMAT_A8_UNORM,                   "A8_UNORM"},
	                                             {DXGI_FORMAT_R1_UNORM,                   "R1_UNORM"},
	                                             {DXGI_FORMAT_R9G9B9E5_SHAREDEXP,         "R9G9B9E5_SHAREDEXP"},
	                                             {DXGI_FORMAT_R8G8_B8G8_UNORM,            "R8G8_B8G8_UNORM"},
	                                             {DXGI_FORMAT_G8R8_G8B8_UNORM,            "G8R8_G8B8_UNORM"},
	                                             {DXGI_FORMAT_BC1_TYPELESS,               "BC1_TYPELESS"},
	                                             {DXGI_FORMAT_BC1_UNORM,                  "BC1_UNORM"},
	                                             {DXGI_FORMAT_BC1_UNORM_SRGB,             "BC1_UNORM_SRGB"},
	                                             {DXGI_FORMAT_BC2_TYPELESS,               "BC2_TYPELESS"},
	                                             {DXGI_FORMAT_BC2_UNORM,                  "BC2_UNORM"},
	                                             {DXGI_FORMAT_BC2_UNORM_SRGB,             "BC2_UNORM_SRGB"},
	                                             {DXGI_FORMAT_BC3_TYPELESS,               "BC3_TYPELESS"},
	                                             {DXGI_FORMAT_BC3_UNORM,                  "BC3_UNORM"},
	                                             {DXGI_FORMAT_BC3_UNORM_SRGB,             "BC3_UNORM_SRGB"},
	                                             {DXGI_FORMAT_BC4_TYPELESS,               "BC4_TYPELESS"},
	                                             {DXGI_FORMAT_BC4_UNORM,                  "BC4_UNORM"},
	                                             {DXGI_FORMAT_BC4_SNORM,                  "BC4_SNORM"},
	                                             {DXGI_FORMAT_BC5_TYPELESS,               "BC5_TYPELESS"},
	                                             {DXGI_FORMAT_BC5_UNORM,                  "BC5_UNORM"},
	                                             {DXGI_FORMAT_BC5_SNORM,                  "BC5_SNORM"},
	                                             {DXGI_FORMAT_B5G6R5_UNORM,               "B5G6R5_UNORM"},
	                                             {DXGI_FORMAT_B5G5R5A1_UNORM,             "B5G5R5A1_UNORM"},
	                                             {DXGI_FORMAT_B8G8R8A8_UNORM,             "B8G8R8A8_UNORM"},
	                                             {DXGI_FORMAT_B8G8R8X8_UNORM,             "B8G8R8X8_UNORM"},
	                                             {DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM, "R10G10B10_XR_BIAS_A2_UNORM"},
	                                             {DXGI_FORMAT_B8G8R8A8_TYPELESS,          "B8G8R8A8_TYPELESS"},
	                                             {DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,        "B8G8R8A8_UNORM_SRGB"},
	                                             {DXGI_FORMAT_B8G8R8X8_TYPELESS,          "B8G8R8X8_TYPELESS"},
	                                             {DXGI_FORMAT_B8G8R8X8_UNORM_SRGB,        "B8G8R8X8_UNORM_SRGB"},
	                                             {DXGI_FORMAT_BC6H_TYPELESS,              "BC6H_TYPELESS"},
	                                             {DXGI_FORMAT_BC6H_UF16,                  "BC6H_UF16"},
	                                             {DXGI_FORMAT_BC6H_SF16,                  "BC6H_SF16"},
	                                             {DXGI_FORMAT_BC7_TYPELESS,               "BC7_TYPELESS"},
	                                             {DXGI_FORMAT_BC7_UNORM,                  "BC7_UNORM"},
	                                             {DXGI_FORMAT_BC7_UNORM_SRGB,             "BC7_UNORM_SRGB"},
	                                             {DXGI_FORMAT_AYUV,                       "AYUV"},
	                                             {DXGI_FORMAT_Y410,                       "Y410"},
	                                             {DXGI_FORMAT_Y416,                       "Y416"},
	                                             {DXGI_FORMAT_NV12,                       "NV12"},
	                                             {DXGI_FORMAT_P010,                       "P010"},
	                                             {DXGI_FORMAT_P016,                       "P016"},
	                                             {DXGI_FORMAT_420_OPAQUE,                 "420_OPAQUE"},
	                                             {DXGI_FORMAT_YUY2,                       "YUY2"},
	                                             {DXGI_FORMAT_Y210,                       "Y210"},
	                                             {DXGI_FORMAT_Y216,                       "Y216"},
	                                             {DXGI_FORMAT_NV11,                       "NV11"},
	                                             {DXGI_FORMAT_AI44,                       "AI44"},
	                                             {DXGI_FORMAT_IA44,                       "IA44"},
	                                             {DXGI_FORMAT_P8,                         "P8"},
	                                             {DXGI_FORMAT_A8P8,                       "A8P8"},
	                                             {DXGI_FORMAT_B4G4R4A4_UNORM,             "B4G4R4A4_UNORM"},
	                                             {DXGI_FORMAT_P208,                       "P208"},
	                                             {DXGI_FORMAT_V208,                       "V208"},
	                                             {DXGI_FORMAT_V408,                       "V408"},
	                                             {DXGI_FORMAT_FORCE_UINT,                 "FORCE_UINT"}};
	
	BiMap<DirectX::TEX_ALPHA_MODE, std::string> g_alphamode_map{{DirectX::TEX_ALPHA_MODE_UNKNOWN,       "Unknown"},
	                                                            {DirectX::TEX_ALPHA_MODE_STRAIGHT,      "Straight"},
	                                                            {DirectX::TEX_ALPHA_MODE_PREMULTIPLIED, "Premultiplied"},
	                                                            {DirectX::TEX_ALPHA_MODE_OPAQUE,        "Opaque"},
	                                                            {DirectX::TEX_ALPHA_MODE_CUSTOM,        "Custom"}};
	
	BiMap<DXTImage::IMAGE_TYPE, std::string> g_imagetype_map{{DXTImage::UNKNOWN, "Unknown"},
	                                                         {DXTImage::DDS,     "DDS"},
	                                                         {DXTImage::HDR,     "HDR"},
	                                                         {DXTImage::TGA,     "TGA"}};
	
	static const DirectX::DDS_FLAGS g_ddsflags_list[] = {DirectX::DDS_FLAGS_NONE,
	                                                     DirectX::DDS_FLAGS_LEGACY_DWORD,
	                                                     DirectX::DDS_FLAGS_NO_LEGACY_EXPANSION,
	                                                     DirectX::DDS_FLAGS_NO_R10B10G10A2_FIXUP,
	                                                     DirectX::DDS_FLAGS_FORCE_RGB,
	                                                     DirectX::DDS_FLAGS_NO_16BPP,
	                                                     DirectX::DDS_FLAGS_EXPAND_LUMINANCE,
	                                                     DirectX::DDS_FLAGS_BAD_DXTN_TAILS,
	                                                     DirectX::DDS_FLAGS_FORCE_DX10_EXT,
	                                                     DirectX::DDS_FLAGS_FORCE_DX10_EXT_MISC2};
	static const int g_num_ddsflags = ARRAYSIZE(g_ddsflags_list);
	static const char* g_ddsflags_names[g_num_ddsflags] = {"None",
	                                                       "LegacyDWord",
	                                                       "NoLegacyExpansion",
	                                                       "NoR10B10G10A2FixUp",
	                                                       "ForceRGB",
	                                                       "No16BPP",
	                                                       "ExpandLuminance",
	                                                       "BadDXTNTails",
	                                                       "ForceDX10Ext",
	                                                       "ForceDX10ExtMisc2"};
	
	DXTFlags<DirectX::DDS_FLAGS> g_ddsflags(g_num_ddsflags,
	                                        g_ddsflags_list,
	                                        g_ddsflags_names);
	
	static const DirectX::TEX_FILTER_FLAGS g_filterflags_list[] = {DirectX::TEX_FILTER_DEFAULT,
	                                                               DirectX::TEX_FILTER_POINT,
	                                                               DirectX::TEX_FILTER_LINEAR,
	                                                               DirectX::TEX_FILTER_CUBIC,
	                                                               DirectX::TEX_FILTER_FANT,
	                                                               DirectX::TEX_FILTER_BOX,
	                                                               DirectX::TEX_FILTER_TRIANGLE,
	                                                               DirectX::TEX_FILTER_DITHER,
	                                                               DirectX::TEX_FILTER_DITHER_DIFFUSION,
	                                                               DirectX::TEX_FILTER_WRAP_U,
	                                                               DirectX::TEX_FILTER_WRAP_V,
	                                                               DirectX::TEX_FILTER_WRAP_W,
	                                                               DirectX::TEX_FILTER_WRAP,
	                                                               DirectX::TEX_FILTER_MIRROR_U,
	                                                               DirectX::TEX_FILTER_MIRROR_V,
	                                                               DirectX::TEX_FILTER_MIRROR_W,
	                                                               DirectX::TEX_FILTER_MIRROR,
	                                                               DirectX::TEX_FILTER_SRGB_IN,
	                                                               DirectX::TEX_FILTER_SRGB_OUT,
	                                                               DirectX::TEX_FILTER_SRGB,
	                                                               DirectX::TEX_FILTER_FORCE_NON_WIC,
	                                                               DirectX::TEX_FILTER_FORCE_WIC,
	                                                               DirectX::TEX_FILTER_SEPARATE_ALPHA,
	                                                               DirectX::TEX_FILTER_RGB_COPY_RED,
	                                                               DirectX::TEX_FILTER_RGB_COPY_GREEN,
	                                                               DirectX::TEX_FILTER_RGB_COPY_BLUE,
	                                                               DirectX::TEX_FILTER_FLOAT_X2BIAS};
	static const int g_num_filterflags = ARRAYSIZE(g_filterflags_list);
	static const char* g_filterflags_names[g_num_filterflags] = {"Default",
	                                                             "Point",
	                                                             "Linear",
	                                                             "Cubic",
	                                                             "Fant",
	                                                             "Box",
	                                                             "Triangle",
	                                                             "Dither",
	                                                             "DitherDiffusion",
	                                                             "WrapU",
	                                                             "WrapV",
	                                                             "WrapW",
	                                                             "Wrap",
	                                                             "MirrorU",
	                                                             "MirrorV",
	                                                             "MirrorW",
	                                                             "Mirror",
	                                                             "SRGBIn",
	                                                             "SRGBOut",
	                                                             "SRGB",
	                                                             "ForceNonWIC",
	                                                             "ForceWIC",
	                                                             "SeparateAlpha",
	                                                             "RGBCopyRed",
	                                                             "RGBCopyGreen",
	                                                             "RGBCopyBlue",
	                                                             "FloatX2Bias"};
	
	DXTFlags<DirectX::TEX_FILTER_FLAGS> g_filterflags(g_num_filterflags,
	                                                  g_filterflags_list,
	                                                  g_filterflags_names);
	
	
	static const DirectX::TEX_FR_FLAGS g_frflags_list[]{DirectX::TEX_FR_ROTATE0,
	                                                    DirectX::TEX_FR_ROTATE90,
	                                                    DirectX::TEX_FR_ROTATE180,
	                                                    DirectX::TEX_FR_ROTATE270,
	                                                    DirectX::TEX_FR_FLIP_HORIZONTAL,
	                                                    DirectX::TEX_FR_FLIP_VERTICAL};
	static const int g_num_frflags = ARRAYSIZE(g_frflags_list);
	static const char* g_frflags_names[g_num_frflags]{"ROTATE0",
	                                                  "ROTATE90",
	                                                  "ROTATE180",
	                                                  "ROTATE270",
	                                                  "FLIP_HORIZONTAL",
	                                                  "FLIP_VERTICAL"};
	
	DXTFlags<DirectX::TEX_FR_FLAGS> g_frflags(g_num_frflags,
	                                          g_frflags_list,
	                                          g_frflags_names);
	
	static const DirectX::TEX_PMALPHA_FLAGS g_pmflags_list[]{DirectX::TEX_PMALPHA_DEFAULT,
	                                                         DirectX::TEX_PMALPHA_IGNORE_SRGB,
	                                                         DirectX::TEX_PMALPHA_REVERSE,
	                                                         DirectX::TEX_PMALPHA_SRGB_IN,
	                                                         DirectX::TEX_PMALPHA_SRGB_OUT,
	                                                         DirectX::TEX_PMALPHA_SRGB};
	static const int g_num_pmflags = ARRAYSIZE(g_pmflags_list);
	static const char* g_pmflags_names[g_num_pmflags]{"DEFAULT",
	                                                  "IGNORE_SRGB",
	                                                  "REVERSE",
	                                                  "SRGB_IN",
	                                                  "SRGB_OUT",
	                                                  "SRGB"};
	
	DXTFlags<DirectX::TEX_PMALPHA_FLAGS> g_pmflags(g_num_pmflags,
	                                               g_pmflags_list,
	                                               g_pmflags_names);
	
	static const DirectX::TEX_COMPRESS_FLAGS g_compressflags_list[]{DirectX::TEX_COMPRESS_DEFAULT,
	                                                                DirectX::TEX_COMPRESS_RGB_DITHER,
	                                                                DirectX::TEX_COMPRESS_A_DITHER,
	                                                                DirectX::TEX_COMPRESS_DITHER,
	                                                                DirectX::TEX_COMPRESS_UNIFORM,
	                                                                DirectX::TEX_COMPRESS_BC7_USE_3SUBSETS,
	                                                                DirectX::TEX_COMPRESS_BC7_QUICK,
	                                                                DirectX::TEX_COMPRESS_SRGB_IN,
	                                                                DirectX::TEX_COMPRESS_SRGB_OUT,
	                                                                DirectX::TEX_COMPRESS_SRGB,
	                                                                DirectX::TEX_COMPRESS_PARALLEL};
	static const int g_num_compressflags = ARRAYSIZE(g_compressflags_list);
	static const char* g_compressflags_names[g_num_compressflags]{"DEFAULT",
	                                                              "RGB_DITHER",
	                                                              "A_DITHER",
	                                                              "DITHER",
	                                                              "UNIFORM",
	                                                              "BC7_USE_3SUBSETS",
	                                                              "BC7_QUICK",
	                                                              "SRGB_IN",
	                                                              "SRGB_OUT",
	                                                              "SRGB",
	                                                              "PARALLEL"};
	
	DXTFlags<DirectX::TEX_COMPRESS_FLAGS> g_compressflags(g_num_compressflags,
	                                                      g_compressflags_list,
	                                                      g_compressflags_names);
	
	static const DirectX::CNMAP_FLAGS g_cnflags_list[]{DirectX::CNMAP_DEFAULT,
	                                                   DirectX::CNMAP_CHANNEL_RED,
	                                                   DirectX::CNMAP_CHANNEL_GREEN,
	                                                   DirectX::CNMAP_CHANNEL_BLUE,
	                                                   DirectX::CNMAP_CHANNEL_ALPHA,
	                                                   DirectX::CNMAP_CHANNEL_LUMINANCE,
	                                                   DirectX::CNMAP_MIRROR_U,
	                                                   DirectX::CNMAP_MIRROR_V,
	                                                   DirectX::CNMAP_MIRROR,
	                                                   DirectX::CNMAP_INVERT_SIGN,
	                                                   DirectX::CNMAP_COMPUTE_OCCLUSION};
	static const int g_num_cnflags = ARRAYSIZE(g_cnflags_list);
	static const char* g_cnflags_names[g_num_cnflags]{"DEFAULT",
	                                                  "CHANNEL_RED",
	                                                  "CHANNEL_GREEN",
	                                                  "CHANNEL_BLUE",
	                                                  "CHANNEL_ALPHA",
	                                                  "CHANNEL_LUMINANCE",
	                                                  "MIRROR_U",
	                                                  "MIRROR_V",
	                                                  "MIRROR",
	                                                  "INVERT_SIGN",
	                                                  "COMPUTE_OCCLUSION"};
	
	DXTFlags<DirectX::CNMAP_FLAGS> g_cnflags(g_num_cnflags,
	                                         g_cnflags_list,
	                                         g_cnflags_names);
	
	
	static const DirectX::CMSE_FLAGS g_cmseflags_list[]{DirectX::CMSE_DEFAULT,
	                                                    DirectX::CMSE_IMAGE1_SRGB,
	                                                    DirectX::CMSE_IMAGE2_SRGB,
	                                                    DirectX::CMSE_IMAGE1_X2_BIAS,
	                                                    DirectX::CMSE_IMAGE2_X2_BIAS,
	                                                    DirectX::CMSE_IGNORE_RED,
	                                                    DirectX::CMSE_IGNORE_GREEN,
	                                                    DirectX::CMSE_IGNORE_BLUE,
	                                                    DirectX::CMSE_IGNORE_ALPHA};
	static const int g_num_cmseflags = ARRAYSIZE(g_cmseflags_list);
	static const char* g_cmseflags_names[g_num_cmseflags]{"DEFAULT",
	                                                      "IMAGE1_SRGB",
	                                                      "IMAGE2_SRGB",
	                                                      "IMAGE1_X2_BIAS",
	                                                      "IMAGE2_X2_BIAS",
	                                                      "IGNORE_RED",
	                                                      "IGNORE_GREEN",
	                                                      "IGNORE_BLUE",
	                                                      "IGNORE_ALPHA"};
	
	DXTFlags<DirectX::CMSE_FLAGS> g_cmseflags(g_num_cmseflags,
	                                          g_cmseflags_list,
	                                          g_cmseflags_names);
	
	
	template<typename T>
	void DXTFlags<T>::ImportFlags(int num_options, const mxArray* mx_options[], T& flags)
	{
		int i;
		const mxArray* curr_key;
		const mxArray* curr_val;
		
		if((num_options % 2) != 0)
		{
			MEXError::PrintMexError(MEU_FL,
			                        MEU_SEVERITY_USER,
			                        "KeyValueError",
			                        "Invalid number of arguments. A key is likely missing a value.");
		}
		
		for(i = 0; i < num_options; i += 2)
		{
			curr_key = mx_options[i];
			curr_val = mx_options[i + 1];
			if(!mxIsChar(curr_key))
			{
				MEXError::PrintMexError(MEU_FL,
				                        MEU_SEVERITY_USER,
				                        "InvalidKeyError",
				                        "All keys must be class 'char'.");
			}
			if(!mxIsLogicalScalar(curr_val))
			{
				MEXError::PrintMexError(MEU_FL,
				                        MEU_SEVERITY_USER,
				                        "InvalidValueError",
				                        "All flag values must be scalar class 'logical'.");
			}
			MEXUtils::ToUpper((mxArray*)curr_key);
			char* flagname = mxArrayToString(curr_key);
			T found_flag = this->FindFlag(flagname);
			if(mxIsLogicalScalarTrue(curr_val))
			{
				flags = (T)(flags | found_flag);
			}
			else
			{
				flags = (T)(flags & ~found_flag);
			}
			mxFree(flagname);
		}
	}
	
	
	template<typename T>
	mxArray* DXTFlags<T>::ExportFlags(T flags)
	{
		mxArray* flag_struct = mxCreateStructMatrix(1,
		                                            1,
		                                            this->GetNumberOfFlags(),
		                                            this->GetNames());
		for(auto &p : this->GetFlagToNameMap())
		{
			mxSetField(flag_struct,
			           0,
			           p.second.c_str(),
			           mxCreateLogicalScalar(
			           (flags & p.first) | (flags == p.first)));
		}
		mxAddField(flag_struct, "Value");
		MEXUtils::SetScalarField(flag_struct,
		                         0,
		                         "Value",
		                         mxUINT32_CLASS,
		                         flags);
		return flag_struct;
	}
	
}
