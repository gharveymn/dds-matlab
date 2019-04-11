#pragma once

#include "mex.h"
#include "DirectXTex.h"

#define SHAREDEXP_BIAS 0xF
#define SHAREDEXP_R_MASK 0x000001FF
#define SHAREDEXP_G_MASK 0x0003FE00
#define SHAREDEXP_B_MASK 0x07FC0000
#define SHAREDEXP_E_MASK 0xF8000000

#define FLOAT16_BIAS   0xF
#define FLOAT16_S_MASK 0x8000u
#define FLOAT16_E_MASK 0x7C00u
#define FLOAT16_M_MASK 0x03FFu

#define FLOAT11_BIAS   0xF
#define FLOAT11_E_MASK 0x7C0u
#define FLOAT11_M_MASK 0x03Fu

#define FLOAT10_BIAS   0xF
#define FLOAT10_E_MASK 0x3E0u
#define FLOAT10_M_MASK 0x01Fu

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
		
		/* getChannel */
		/* nextPixel */
		
		explicit DXGIPixel(DXGI_FORMAT fmt) :
		_format(fmt),
		_pixel_width(DirectX::BitsPerPixel(fmt)),
		_num_channels(0),
		_channels{0},
		_image(nullptr),
		_has_uniform_datatype(false),
		_has_uniform_width(false)
		{
			this->SetChannels(fmt);
		}
		
		DXGIPixel(DXGI_FORMAT fmt, const DirectX::Image* image) :
		_format(fmt),
		_pixel_width(DirectX::BitsPerPixel(fmt)),
		_num_channels(0),
		_channels{0},
		_image(image),
		_has_uniform_datatype(false),
		_has_uniform_width(false)
		{
			this->SetChannels(fmt);
		}
		
		void SetImage(const DirectX::Image* image)
		{
			this->_image = image;
		}
		
		constexpr inline size_t _num_pixels()
		{
			return this->_image->width * this->_image->height;
		}
		
		void ExtractChannels(const size_t* ch_idx, const size_t* out_idx, size_t num_idx, mxArray*& out, mxClassID out_class = mxUNKNOWN_CLASS);
		inline void ExtractChannels(size_t ch_idx, size_t out_idx, size_t num_idx, mxArray*& out, mxClassID out_class = mxUNKNOWN_CLASS)
		{
			const size_t ch_idx_arr[1]  = {ch_idx};
			const size_t out_idx_arr[1] = {out_idx};
			this->ExtractChannels(ch_idx_arr, out_idx_arr, 1, out, out_class);
		}
		
		void ExtractRGB(mxArray*& mx_rgb);
		void ExtractRGBA(mxArray*& mx_rgba);
		void ExtractRGBA(mxArray*& mx_rgb, mxArray*& mx_a);
		
		
	private:
		
		struct PixelChannel
		{
			size_t              width;             /* in bits */
			size_t              offset;           /* offset into interior of pixel */
			size_t              standard_idx;     /* if this is channel 0 in BGRA format this this is 2, corresponding to standard RGBA format. */
			DXGIPixel::DATATYPE datatype;        /* determines MATLAB output class */
			char                name;             /* ex. R, G, B, A, L, B, etc. */
		};
		
		const size_t            _pixel_width;
		size_t                  _num_channels;
		DXGIPixel::PixelChannel _channels[MAX_CHANNELS];
		const DirectX::Image*   _image;
		const DXGI_FORMAT       _format;
		bool                    _has_uniform_datatype;
		bool                    _has_uniform_width;
		
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
		struct ChannelElement<DXGIPixel::TYPELESS, T>
		{
			static inline void Store(void* data, mwIndex dst_idx, uint32_t ir, uint32_t num_bits = 0)
			{
				/* in the narrowing case this isn't portable */
				((T*)data)[dst_idx] = *((T*)&ir);
			}
		};
		
		template <>
		struct ChannelElement<DXGIPixel::TYPELESS, mxDouble>
		{
			static inline void Store(void* data, mwIndex dst_idx, uint32_t ir, uint32_t num_bits = 0)
			{
				/* make sure we don't go over 32-bit width */
				((mxDouble*)data)[dst_idx] = (mxDouble)*((float*)&ir);
			}
		};
		
		/* SNORM conversions */
		template <>
		struct ChannelElement<DXGIPixel::SNORM, mxSingle>
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
		struct ChannelElement<DXGIPixel::UNORM, mxSingle>
		{
			static inline void Store(void* data, mwIndex dst_idx, uint32_t ir, uint32_t num_bits = 32)
			{
				((mxSingle*)data)[dst_idx] = (mxSingle)ir/(((uint32_t)1 << num_bits) - 1);
			}
		};
		
		/* SINT conversions */
		template <typename T>
		struct ChannelElement<DXGIPixel::SINT, T>
		{
			static inline void Store(void* data, mwIndex dst_idx, uint32_t ir, uint32_t num_bits)
			{
				((T*)data)[dst_idx] = (T)DXGIPixel::SignExtend<T>(ir, num_bits);
			}
		};
		
		/* UINT conversions */
		template <typename T>
		struct ChannelElement<DXGIPixel::UINT, T>
		{
			static inline void Store(void* data, mwIndex dst_idx, uint32_t ir, uint32_t num_bits)
			{
				((T*)data)[dst_idx] = (T)ir;
			}
		};
		
		/* FLOAT conversions */
		template <>
		struct ChannelElement<DXGIPixel::FLOAT, mxSingle>
		{
			static inline void Store(void* data, mwIndex dst_idx, uint32_t ir, uint32_t num_bits = 32)
			{
				((mxSingle*)data)[dst_idx] = (num_bits == 32)? (mxSingle)*((float*)&ir) : GetFloat(ir, num_bits);
			}
			
			static float GetFloat(uint32_t ir, uint32_t num_bits)
			{
				switch(num_bits)
				{
					/* all cases below have 1 added to mantissa if exp is not zero and support denorms */
					case 16:
					{
						
						float c;
						int32_t exp_bits = (ir & FLOAT16_E_MASK) >> 10u;
						if(exp_bits == 0)
						{
							/* this format has denorm support */
							c = (float)(ir & FLOAT16_M_MASK) * std::pow(2.0f, 1 - FLOAT16_BIAS);
						}
						else
						{
							c = ((float)(ir & FLOAT16_M_MASK) + 1.0f) * std::pow(2.0f, exp_bits - FLOAT16_BIAS);
						}
						return (ir & FLOAT16_S_MASK)? -c : c;
					}
					/* no sign bit for cases below */
					case 11:
					{
						
						int32_t exp_bits = (ir & FLOAT11_E_MASK) >> 10u;
						if(exp_bits == 0)
						{
							/* this format has denorm support */
							return (float)(ir & FLOAT11_M_MASK) * std::pow(2.0f, 1 - FLOAT11_BIAS);
						}
						else
						{
							return ((float)(ir & FLOAT11_M_MASK) + 1.0f) * std::pow(2.0f, exp_bits - FLOAT11_BIAS);
						}
					}
					case 10:
					{
						
						int32_t exp_bits = (ir & FLOAT10_E_MASK) >> 10u;
						if(exp_bits == 0)
						{
							/* this format has denorm support */
							return (float)(ir & FLOAT10_M_MASK) * std::pow(2.0f, 1 - FLOAT10_BIAS);
						}
						else
						{
							return ((float)(ir & FLOAT10_M_MASK) + 1.0f) * std::pow(2.0f, exp_bits - FLOAT10_BIAS);
						}
					}
					default:
					{
						MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_INTERNAL, "UnexpectedFormatError", "Unexpected floating point format had a channel with %lu bits.", num_bits);
					}
				}
				return 0.0f;
			}
		};
		
		template <>
		struct ChannelElement<DXGIPixel::FLOAT, mxDouble>
		{
			static inline void Store(void* data, mwIndex dst_idx, uint32_t ir, uint32_t num_bits = 32)
			{
				((mxDouble*)data)[dst_idx] = (num_bits == 32)? (mxDouble)*((float*)&ir) : GetDouble(ir, num_bits);
			}
			
			static double GetDouble(uint32_t ir, uint32_t num_bits)
			{
				switch(num_bits)
				{
					/* all cases below have 1 added to mantissa if exp is not zero and support denorms */
					case 16:
					{
						double c;
						int32_t exp_bits = (ir & FLOAT16_E_MASK) >> 10u;
						if(exp_bits == 0)
						{
							/* this format has denorm support */
							c = (double)(ir & FLOAT16_M_MASK) * std::pow(2.0, 1 - FLOAT16_BIAS);
						}
						else
						{
							c = ((double)(ir & FLOAT16_M_MASK) + 1.0) * std::pow(2.0, exp_bits - FLOAT16_BIAS);
						}
						return (ir & FLOAT16_S_MASK)? -c : c;
					}
						/* no sign bit for cases below */
					case 11:
					{
						int32_t exp_bits = (ir & FLOAT11_E_MASK) >> 10u;
						if(exp_bits == 0)
						{
							/* this format has denorm support */
							return (double)(ir & FLOAT11_M_MASK) * std::pow(2.0, 1 - FLOAT11_BIAS);
						}
						else
						{
							return ((double)(ir & FLOAT11_M_MASK) + 1.0) * std::pow(2.0, exp_bits - FLOAT11_BIAS);
						}
						break;
					}
					case 10:
					{
						int32_t exp_bits = (ir & FLOAT10_E_MASK) >> 10u;
						if(exp_bits == 0)
						{
							/* this format has denorm support */
							return (double)(ir & FLOAT10_M_MASK) * std::pow(2.0, 1 - FLOAT10_BIAS);
						}
						else
						{
							return ((double)(ir & FLOAT10_M_MASK) + 1.0) * std::pow(2.0, exp_bits - FLOAT10_BIAS);
						}
						break;
					}
					default:
					{
						MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_INTERNAL, "UnexpectedFormatError", "Unexpected floating point format had a channel with %lu bits.", num_bits);
					}
				}
				return 0.0;
			}
		};
		
		template <typename T>
		struct ChannelElement<DXGIPixel::FLOAT, T>
		{
			static inline void Store(void* data, mwIndex dst_idx, uint32_t ir, uint32_t num_bits = 32)
			{
				((T*)data)[dst_idx] = (num_bits == 32)? (T)ir : (T)ChannelElement<DXGIPixel::FLOAT, mxSingle>::GetFloat(ir, num_bits);
			}
		};
		
		/* SRGB conversions */
		template <>
		struct ChannelElement<DXGIPixel::SRGB, mxSingle>
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
					((mxSingle*)data)[dst_idx] = std::pow((c + D3D11_SRGB_TO_FLOAT_OFFSET)/D3D11_SRGB_TO_FLOAT_DENOMINATOR_2, D3D11_SRGB_TO_FLOAT_EXPONENT);
				}
			}
		};
		
		template <>
		struct ChannelElement<DXGIPixel::SRGB, mxUint8>
		{
			static inline void Store(void* data, mwIndex dst_idx, uint32_t ir, uint32_t num_bits = 8)
			{
				((mxUint8*)data)[dst_idx] = (mxUint8)ir;
			}
		};
		
		template <>
		struct ChannelElement<DXGIPixel::SHAREDEXP, mxUint16>
		{
			static inline void Store(void* data, mwIndex dst_idx, uint32_t ir, uint32_t num_bits = 16)
			{
				((mxUint16*)data)[dst_idx] = (mxUint16)ir;
			}
		};
		
		template <>
		struct ChannelElement<DXGIPixel::SHAREDEXP, mxUint8>
		{
			static inline void Store(void* data, mwIndex dst_idx, uint32_t ir, uint32_t num_bits = 8)
			{
				((mxUint8*)data)[dst_idx] = (mxUint8)ir;
			}
		};
		
		/* XR_BIAS conversions */
		template <>
		struct ChannelElement<DXGIPixel::XR_BIAS, mxSingle>
		{
			static inline void Store(void* data, mwIndex dst_idx, uint32_t ir, uint32_t num_bits = 32)
			{
				((mxSingle*)data)[dst_idx] = (float)((ir & (uint32_t)0x3ff) - 0x180) / 510.f;
			}
		};
		
		void SetChannels(DXGI_FORMAT);
		
		template <typename T>
		inline void StoreUniformChannels(const size_t* ch_idx, const size_t* out_idx,  size_t num_idx, void* data, StorageFunction storage_function)
		{
			auto pixels = (T*)this->_image->pixels;
			for(size_t i = 0; i < this->_num_pixels(); i++)
			{
				auto v_pix = pixels + i*this->_num_channels;
				for(size_t j = 0; j < num_idx; j++)
				{
					mwIndex dst_idx = i / this->_image->width + (i % this->_image->width) * this->_image->height + out_idx[j] * this->_num_pixels();
					storage_function(data, dst_idx, *(v_pix + ch_idx[j]), (uint32_t)(sizeof(T) * 8));
				}
			}
		}
	};
}

