#pragma once

#include "mex.h"
#include "DirectXTex.h"

#define MAX_CHANNELS 4
#define MAX_CHANNEL_NAME_LEN 16

namespace DXTMEX
{
	
	class DXGIPixel
	{
	public:
		enum DATA_TYPE
		{
			TYPELESS,
			SNORM,
			UNORM,
			SINT,
			UINT,
			FLOAT,
			SRGB,
			SHAREDEXP,
			XR_BIAS
		};
		
		struct PixelChannel
		{
			size_t    size;             /* in bits */
			size_t    offset;           /* offset into interior of pixel */
			size_t    standard_idx;     /* if this is channel 0 in BGRA format this this is 2, corresponding to standard RGBA format. */
			uint32_t  mask;             /* derived from size */
			char      name;             /* ex. R, G, B, A, L, B, etc. */
		};
		
		DXGI_FORMAT _format;
		
		size_t _pixel_size;
		size_t _num_channels;
		PixelChannel _channels[MAX_CHANNELS];
		uint8_t* _pixels;
		
		/* getChannel */
		/* nextPixel */
		
		explicit DXGIPixel(DXGI_FORMAT fmt, uint8_t* pixels) : _format(fmt), _pixel_size(DirectX::BitsPerPixel(fmt)), _num_channels(0), _channels{0}
		{
			this->_pixels = pixels;
			this->SetChannels(fmt);
		}
		
		template <typename T>
		T ExtractPixelChannel(size_t idx);
		
		void ExtractChannel(size_t ch_num, mxArray*& out);
		void ExtractChannel(size_t* ch_nums, size_t num_ch, mxArray*& out);
		void ExtractRGB(mxArray*& rgb);
		void ExtractRGBA(mxArray*& rgba);
		void ExtractRGBA(mxArray*& rgb, mxArray* a);
		
		
	private:
		void SetChannels(DXGI_FORMAT);
		
		size_t bit_offset = 0;
	};
}

