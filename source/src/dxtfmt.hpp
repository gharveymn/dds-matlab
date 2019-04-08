#pragma once

#include "mex.h"
#include "DirectXTex.h"

#define MAX_CHANNELS 4
#define MAX_CHANNEL_NAME_LEN 16

namespace DXTMEX
{
	
	class DXGIPixels
	{
	private:
		size_t paging_size;
		size_t bit_offset = 0;
	public:
		DXGI_FORMAT format;
		size_t num_channels;
		size_t pixel_size;
		size_t* channel_offsets;
		size_t* channel_sizes;
		char** channel_names;
		uint8_t* pixels;
		
		/* getChannel */
		/* nextPixel */
		
		DXGIPixels(DXGI_FORMAT fmt) {}
		
		
		void Set(DXGI_FORMAT fmt, size_t nch, size_t px_sz, size_t ch_sz[MAX_CHANNELS], char ch_names[MAX_CHANNELS][MAX_CHANNEL_NAME_LEN])
		{
			format = fmt;
			num_channels = nch;
			pixel_size = px_sz;
			memcpy(&channel_sizes, ch_sz, MAX_CHANNELS*sizeof(size_t));
			for(int i = 0; i < MAX_CHANNELS; i++)
			{
				memcpy(&channel_names, ch_names, MAX_CHANNEL_NAME_LEN*sizeof(char));
				if(i > 0)
				{
					channel_offsets[i] = channel_offsets[i - 1] + channel_sizes[i - 1];
				}
				else
				{
					channel_offsets[0] = 0;
				}
			}
		}
		
		mxArray* ExtractChannel(std::vector<size_t> ch_nums);
		
	};
}

