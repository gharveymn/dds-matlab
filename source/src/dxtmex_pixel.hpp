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
		enum DATATYPE
		{
			TYPELESS,
			SNORM,     /* -> float -1.0f to 1.0f */
			UNORM,     /* -> float  0.0f to 1.0f*/
			SINT,      /* -> signed int */
			UINT,      /* -> unsigned int */
			FLOAT,     /* -> float */
			SRGB,      /* -> float  0.0f to 1.0f */
			SHAREDEXP, /* -> float */
			XR_BIAS    /* -> float */
		};
		
		struct PixelChannel
		{
			size_t              width;             /* in bits */
			size_t              offset;           /* offset into interior of pixel */
			size_t              standard_idx;     /* if this is channel 0 in BGRA format this this is 2, corresponding to standard RGBA format. */
			DXGIPixel::DATATYPE datatype;        /* determines MATLAB output class */
			char                name;             /* ex. R, G, B, A, L, B, etc. */
		};
		
		const size_t            _pixel_width;
		const size_t            _num_pixels;
		size_t                  _num_channels;
		DXGIPixel::PixelChannel _channels[MAX_CHANNELS];
		const DirectX::Image*   _image;
		const DXGI_FORMAT       _format;
		bool                    _has_uniform_datatype;
		bool                    _has_uniform_width;
		
		/* getChannel */
		/* nextPixel */
		
		explicit DXGIPixel(DXGI_FORMAT fmt, const DirectX::Image* image) :
		_format(fmt),
		_num_pixels(image->width * image->height),
		_pixel_width(DirectX::BitsPerPixel(fmt)),
		_num_channels(0),
		_channels{0},
		_image(image),
		_has_uniform_datatype(false),
		_has_uniform_width(false)
		{
			this->SetChannels(fmt);
		}
		
		uint32_t DXGIPixel::ExtractPixelChannel(size_t idx, int ch_num);
		
		void ExtractChannel(const size_t* ch_nums, size_t num_ch, mxArray*& out, mxClassID out_class = mxUNKNOWN_CLASS);
		
		template <int, DXGIPixel::DATATYPE, typename OUT_TYPE>
		struct ChannelElement
		{
			static inline void Store(void* data, mwIndex dst_idx, uint32_t ir) {};
			static inline OUT_TYPE SignExtend(uint32_t ir)
			{
				struct {OUT_TYPE bits:N;} s;
				return s.bits = ir;
			}
		};
		
//		void StoreTYPELESSAsSingle(void* data, mwIndex dst_idx, uint32_t ir);
//		void StoreTYPELESSAsInt8(void* data, mwIndex dst_idx, uint32_t ir);
//		void StoreTYPELESSAsInt16(void* data, mwIndex dst_idx, uint32_t ir);
//		void StoreTYPELESSAsInt32(void* data, mwIndex dst_idx, uint32_t ir);
//		void StoreTYPELESSAsUInt8(void* data, mwIndex dst_idx, uint32_t ir);
//		void StoreTYPELESSAsUInt16(void* data, mwIndex dst_idx, uint32_t ir);
//		void StoreTYPELESSAsUInt32(void* data, mwIndex dst_idx, uint32_t ir);
//		void StoreSNORMAsSingle(void* data, mwIndex dst_idx, uint32_t ir);
//		void StoreUNORMAsSingle(void* data, mwIndex dst_idx, uint32_t ir);
//		void StoreSINTAsLogical(void* data, mwIndex dst_idx, uint32_t ir);
//		void StoreSINTAsInt8(void* data, mwIndex dst_idx, uint32_t ir);
//		void StoreSINTAsInt16(void* data, mwIndex dst_idx, uint32_t ir);
//		void StoreSINTAsInt32(void* data, mwIndex dst_idx, uint32_t ir);
//		void StoreUINTAsLogical(void* data, mwIndex dst_idx, uint32_t ir);
//		void StoreUINTAsUInt8(void* data, mwIndex dst_idx, uint32_t ir);
//		void StoreUINTAsUInt16(void* data, mwIndex dst_idx, uint32_t ir);
//		void StoreUINTAsUInt32(void* data, mwIndex dst_idx, uint32_t ir);
//		void StoreFLOATAsSingle(void* data, mwIndex dst_idx, uint32_t ir);
//		void StoreSRGBAsSingle(void* data, mwIndex dst_idx, uint32_t ir);
//		void StoreSRGBAsUInt8(void* data, mwIndex dst_idx, uint32_t ir);
//		void StoreSHAREDEXPAsSingle(void* data, mwIndex dst_idx, uint32_t ir);
//		void StoreXR_BIASAsSingle(void* data, mwIndex dst_idx, uint32_t ir);
		
		void ExtractRGB(mxArray*& rgb);
		void ExtractRGBA(mxArray*& rgba);
		void ExtractRGBA(mxArray*& rgb, mxArray* a);
		
		
	private:
		void SetChannels(DXGI_FORMAT);
		
		template <typename T>
		static inline void StoreUniformChannels(const size_t* ch_nums, size_t num_ch_nums, size_t num_ch, T pixels, void* data)
		{
			for(size_t i = 0; i < this->_num_pixels; i++)
			{
				auto v_pix = pixels + i*num_ch;
				for(size_t j = 0; j < num_ch_nums; j++)
				{
					mwIndex dst_idx = i / this->_image->width + (i % this->_image->width) * this->_image->height + j * this->_num_pixels;
					(this->*storage_function)(data, dst_idx, *(pixels + i*num_ch + ch_nums[j]));
				}
			}
		}
		
		size_t _bit_offset = 0;
	};
}

