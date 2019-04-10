#pragma once

#include "mex.h"
#include "DirectXTex.h"

#define MAX_CHANNELS 4

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
		
		void ExtractChannel(const size_t* ch_nums, size_t num_ch, mxArray*& out, mxClassID out_class = mxUNKNOWN_CLASS);
		
		void ExtractRGB(mxArray*& rgb);
		void ExtractRGBA(mxArray*& rgba);
		void ExtractRGBA(mxArray*& rgb, mxArray* a);
		
		
	private:
		
		template <typename T>
		static inline T SignExtend(uint32_t ir, const uint32_t num_bits)
		{
			uint32_t mask = (uint32_t)1 << (num_bits - 1);
			return (T)((ir ^ mask) - mask);
		}
		
		typedef void (*StorageFunction)(void*, mwIndex, uint32_t, uint32_t);
		
		template <DXGIPixel::DATATYPE, typename>
		struct ChannelElement;
		
		template <typename T>
		struct DXGIPixel::ChannelElement<DXGIPixel::TYPELESS, T>
		{
			static inline void Store(void* data, mwIndex dst_idx, uint32_t ir, uint32_t num_bits = 0)
			{
				/* in the narrowing case this isn't portable */
				((T*)data)[dst_idx] = *((T*)&ir);
			}
		};
		
		/* SNORM conversions */
		template <>
		struct DXGIPixel::ChannelElement<DXGIPixel::SNORM, mxSingle>
		{
			static inline void Store(void* data, mwIndex dst_idx, uint32_t ir, uint32_t num_bits = 32)
			{
				uint32_t MSB = (uint32_t)1 << (num_bits - 1);
				if(ir == MSB)
				{
					((mxSingle*)data)[dst_idx] = -1.0f;
					return;
				}
				auto c = DXGIPixel::SignExtend<mxSingle>(ir, num_bits);
				((mxSingle*)data)[dst_idx] = c/(MSB - 1);
			}
		};
		
		/* UNORM conversions */
		template <>
		struct DXGIPixel::ChannelElement<DXGIPixel::UNORM, mxSingle>
		{
			static inline void Store(void* data, mwIndex dst_idx, uint32_t ir, uint32_t num_bits = 32)
			{
				((mxSingle*)data)[dst_idx] = (mxSingle)ir/(((uint32_t)1 << num_bits) - 1);
			}
		};
		
		/* SINT conversions */
		template <typename T>
		struct DXGIPixel::ChannelElement<DXGIPixel::SINT, T>
		{
			static inline void Store(void* data, mwIndex dst_idx, uint32_t ir, uint32_t num_bits)
			{
				((T*)data)[dst_idx] = DXGIPixel::SignExtend<T>(ir, num_bits);
			}
		};
		
		/* UINT conversions */
		template <typename T>
		struct DXGIPixel::ChannelElement<DXGIPixel::UINT, T>
		{
			static inline void Store(void* data, mwIndex dst_idx, uint32_t ir, uint32_t num_bits)
			{
				((T*)data)[dst_idx] = (T)ir;
			}
		};
		
		/* FLOAT conversions */
		template <>
		struct DXGIPixel::ChannelElement<DXGIPixel::FLOAT, mxSingle>
		{
			static inline void Store(void* data, mwIndex dst_idx, uint32_t ir, uint32_t num_bits = 32)
			{
				((float*)data)[dst_idx] = *((float*)&ir);
			}
		};
		
		/* SRGB conversions */
		template <>
		struct DXGIPixel::ChannelElement<DXGIPixel::SRGB, mxSingle>
		{
			static inline void Store(void* data, mwIndex dst_idx, uint32_t ir, uint32_t num_bits = 32)
			{
				float c = (float)ir/(((uint32_t)1 << num_bits) - 1);
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
		
		template <>
		struct DXGIPixel::ChannelElement<DXGIPixel::SRGB, mxUint8>
		{
			static inline void Store(void* data, mwIndex dst_idx, uint32_t ir, uint32_t num_bits = 8)
			{
				((mxUint8*)data)[dst_idx] = (mxUint8)ir;
			}
		};
		
		template <>
		struct DXGIPixel::ChannelElement<DXGIPixel::SHAREDEXP, mxUint16>
		{
			static inline void Store(void* data, mwIndex dst_idx, uint32_t ir, uint32_t num_bits = 16)
			{
				((mxUint16*)data)[dst_idx] = (mxUint16)ir;
			}
		};
		
		template <>
		struct DXGIPixel::ChannelElement<DXGIPixel::SHAREDEXP, mxUint8>
		{
			static inline void Store(void* data, mwIndex dst_idx, uint32_t ir, uint32_t num_bits = 8)
			{
				((mxUint8*)data)[dst_idx] = (mxUint8)ir;
			}
		};
		
		/* XR_BIAS conversions */
		template <>
		struct DXGIPixel::ChannelElement<DXGIPixel::XR_BIAS, mxSingle>
		{
			static inline void Store(void* data, mwIndex dst_idx, uint32_t ir, uint32_t num_bits = 32)
			{
				((mxSingle*)data)[dst_idx] = (float)((ir & (uint32_t)0x3ff) - 0x180) / 510.f;
			}
		};
		
		void SetChannels(DXGI_FORMAT);
		
		template <typename T>
		inline void StoreUniformChannels(const size_t* ch_nums, size_t num_ch_nums, void* data, StorageFunction storage_function)
		{
			auto pixels = (T*)this->_image->pixels;
			for(size_t i = 0; i < this->_num_pixels; i++)
			{
				auto v_pix = pixels + i*this->_num_channels;
				for(size_t j = 0; j < num_ch_nums; j++)
				{
					mwIndex dst_idx = i / this->_image->width + (i % this->_image->width) * this->_image->height + j * this->_num_pixels;
					storage_function(data, dst_idx, *(pixels + i*this->_num_channels + ch_nums[j]), (uint32_t)(sizeof(T) * 8));
				}
			}
		}
	};
}

