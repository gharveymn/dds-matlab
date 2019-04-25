#pragma once

#include "mex.h"
#include "DirectXTex.h"
#include <atomic>

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

	enum class ir32 : uint32_t {};

	template <typename T>
	constexpr T to_integer(ir32 ir)
	{
		return T(ir);
	}

	template <typename T>
	constexpr ir32& operator<<=(ir32& ir, typename std::enable_if<std::is_integral<T>::value>::type s) noexcept
	{
		return ir = ir32(static_cast<uint32_T>(ir) << s);
	}

	template <typename T>
	constexpr ir32 operator<<(ir32 & ir, typename std::enable_if<std::is_integral<T>::value>::type s) noexcept
	{
		return ir32(static_cast<uint32_t>(ir) << s);
	}

	template <typename T>
	constexpr ir32& operator>>=(ir32 & ir, typename std::enable_if<std::is_integral<T>::value>::type s) noexcept
	{
		return ir = ir32(static_cast<uint32_t>(ir) >> s);
	}

	template <typename T>
	constexpr ir32 operator>>(ir32 & ir, typename std::enable_if<std::is_integral<T>::value>::type s) noexcept
	{
		return ir32(static_cast<uint32_T>(ir) >> s);
	}

#define IR32_LOP_EQ(OP) \
constexpr ir32& operator##OP##=(ir32& ir, ir32 r) noexcept \
{                      \
	return ir = ir32(static_cast<uint32_t>(ir) OP static_cast<uint32_t>(r)); \
}

#define IR32_LOP(OP) \
constexpr ir32 operator##OP##(ir32 ir, ir32 r) noexcept \
{                      \
	return ir32(static_cast<uint32_t>(ir) OP static_cast<uint32_t>(r)); \
}

	IR32_LOP_EQ(| )
		IR32_LOP(| )
		IR32_LOP_EQ(^)
		IR32_LOP(^)
		IR32_LOP_EQ(&)
		IR32_LOP(&)

		constexpr ir32 operator~(ir32 ir) noexcept
	{
		return ir32(~static_cast<uint32_t>(ir));
	}
	
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
		_pixel_bit_width(DirectX::BitsPerPixel(fmt)),
		_pixel_byte_width((_pixel_bit_width + 7u) / 8u),
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
		_pixel_bit_width(DirectX::BitsPerPixel(fmt)),
		_pixel_byte_width((_pixel_bit_width + 7u) / 8u),
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

		void InsertChannels(const size_t* dims, size_t num_dims, void* data);
		
		void ExtractChannels(const size_t* ch_idx, const size_t* out_idx, size_t num_idx, mxArray*& out, mxClassID out_class = mxUNKNOWN_CLASS);
		inline void ExtractChannels(size_t ch_idx, size_t out_idx,  mxArray*& out, mxClassID out_class = mxUNKNOWN_CLASS)
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
		
		const size_t            _pixel_bit_width;
		const size_t            _pixel_byte_width;
		size_t                  _num_channels;
		DXGIPixel::PixelChannel _channels[MAX_CHANNELS];
		const DirectX::Image*   _image;
		const DXGI_FORMAT       _format;
		bool                    _has_uniform_datatype;
		bool                    _has_uniform_width;
		
		/* handles signed to signed and signed to unsigned */
		template <typename T>
		constexpr static inline T SaturateCast(int32_t ir)
		{
			return (ir < (int64_t)(std::numeric_limits<T>::min)())? (std::numeric_limits<T>::min)() : (((int64_t)(std::numeric_limits<T>::max)() < ir) ?  (std::numeric_limits<T>::max)() : (T)ir);
		}
		
		/* handles unsigned to unsigned and unsigned to signed */
		template <typename T>
		constexpr static inline T SaturateCast(uint32_t ir)
		{
			return ((uint64_t)(std::numeric_limits<T>::max)() < ir)? (std::numeric_limits<T>::max)() : (T)ir;
		}
		
		static inline int32_t SignExtend(uint32_t ir, const uint32_t num_bits)
		{
			uint32_t mask = (uint32_t)1 << (num_bits - 1);
			uint32_t extended = (ir ^ mask) - mask;
			return *(int32_t*)&extended;
		}
		
		typedef void (*StorageFunction)(void*, mwIndex, uint32_t, uint32_t);
		static StorageFunction GetStorageFunction(DXGIPixel::DATATYPE in_type, mxClassID out_type);
		
		template <DXGIPixel::DATATYPE, typename, class enable = void>
		struct ChannelElement;
		
		void SetChannels(DXGI_FORMAT);
		
		template <typename T>
		inline void StoreUniformChannels(const size_t* ch_idx, const size_t* out_idx,  size_t num_idx, void* data, StorageFunction storage_function)
		{
			mwIndex dst_idx = 0;
			for(size_t i = 0; i < this->_num_pixels(); i++)
			{
				auto v_pix = this->_image->pixels + i*this->_pixel_byte_width;
				for(size_t j = 0; j < num_idx; j++)
				{
					storage_function(data, dst_idx + out_idx[j] * this->_num_pixels(), *(v_pix + ch_idx[j]), (uint32_t)(sizeof(T) * 8u));
				}
				dst_idx += this->_image->height;
				if(dst_idx >= this->_num_pixels())
				{
					dst_idx = i / this->_image->width;
				}
			}
		}

		template <typename T>
		class ChannelExtractor
		{
		private:
			const DXGIPixel& _parent;
			T _masks[4];

		public:
			ChannelExtractor(const DXGIPixel& parent) : _parent(parent)
			{
				for (size_t i = 0; i < parent._num_channels; i++)
				{
					this->_masks[i] = ((T(1) << parent._channels[i].width) - T(1)) << parent._channels[i].offset;
				}
			}


		};

	};
	
	template <>
	constexpr mxLogical DXGIPixel::SaturateCast<mxLogical>(int32_t ir)
	{
		return ir != 0;
	}
	
	template <>
	constexpr mxLogical DXGIPixel::SaturateCast<mxLogical>(uint32_t ir)
	{
		return ir != 0;
	}
	
	
	/* TYPELESS conversions (these are just memcpys) */
	template <typename T>
	struct DXGIPixel::ChannelElement<DXGIPixel::TYPELESS, T>
	{
		static constexpr size_t cpy_sz = sizeof(T) < sizeof(uint32_t)? sizeof(T) : sizeof(uint32_t);
		static inline void Extract(void* data, mwIndex dst_idx, uint32_t ir, uint32_t)
		{
			memcpy((T*)data + dst_idx, &ir, cpy_sz);
		}
	};
	
	/* SNORM conversions */
	
	/* to integral type */
	template <typename T>
	struct DXGIPixel::ChannelElement<DXGIPixel::SNORM, T, typename std::enable_if<std::is_integral<T>::value>::type>
	{
		static inline void Extract(void* data, mwIndex dst_idx, uint32_t ir, uint32_t num_bits = 32)
		{
			((T*)data)[dst_idx] = SaturateCast<T>(SignExtend(ir, num_bits));
		}
	};
	
	/* to floating point */
	/* NATURAL CONVERSION */
	template <typename T>
	struct DXGIPixel::ChannelElement<DXGIPixel::SNORM, T, typename std::enable_if<std::is_floating_point<T>::value>::type>
	{
		static inline void Extract(void* data, mwIndex dst_idx, uint32_t ir, uint32_t num_bits = 32)
		{
			uint32_t MSB = (uint32_t)1 << (num_bits - 1);
			if(ir == MSB)
			{
				((T*)data)[dst_idx] = -1.0f;
				return;
			}
			auto c = (float)SignExtend(ir, num_bits);
			((T*)data)[dst_idx] = c/(MSB - 1);
		}
	};
	
	/* UNORM conversions */
	
	/* to integral type */
	template <typename T>
	struct DXGIPixel::ChannelElement<DXGIPixel::UNORM, T, typename std::enable_if<std::is_integral<T>::value>::type>
	{
		static inline void Extract(void* data, mwIndex dst_idx, uint32_t ir, uint32_t)
		{
			/* don't renormalize so we can get the original data in MATLAB */
			((T*)data)[dst_idx] = SaturateCast<T>(ir);
		}
	};
	
	/* to floating point */
	/* NATURAL CONVERSION */
	template <typename T>
	struct DXGIPixel::ChannelElement<DXGIPixel::UNORM, T, typename std::enable_if<std::is_floating_point<T>::value>::type>
	{
		static inline void Extract(void* data, mwIndex dst_idx, uint32_t ir, uint32_t num_bits = 32)
		{
			/* converted to FLOAT */
			((T*)data)[dst_idx] = (T)ir/((1u << num_bits) - 1);
		}
	};
	
	/* SINT conversions */
	/* NATURAL CONVERSION */
	template <typename T>
	struct DXGIPixel::ChannelElement<DXGIPixel::SINT, T>
	{
		static inline void Extract(void* data, mwIndex dst_idx, uint32_t ir, uint32_t num_bits)
		{
			((T*)data)[dst_idx] = SaturateCast<T>(SignExtend(ir, num_bits));
		}
	};
	
	/* UINT conversions */
	/* NATURAL CONVERSION */
	template <typename T>
	struct DXGIPixel::ChannelElement<DXGIPixel::UINT, T>
	{
		static inline void Extract(void* data, mwIndex dst_idx, uint32_t ir, uint32_t)
		{
			((T*)data)[dst_idx] = SaturateCast<T>(ir);
		}
	};
	
	/* FLOAT conversions */
	/* NATURAL CONVERSION */
	template <>
	struct DXGIPixel::ChannelElement<DXGIPixel::FLOAT, mxSingle>
	{
		static inline void Extract(void* data, mwIndex dst_idx, uint32_t ir, uint32_t num_bits = 32)
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
	struct DXGIPixel::ChannelElement<DXGIPixel::FLOAT, mxDouble>
	{
		static inline void Extract(void* data, mwIndex dst_idx, uint32_t ir, uint32_t num_bits = 32)
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
	struct DXGIPixel::ChannelElement<DXGIPixel::FLOAT, T>
	{
		static inline void Extract(void* data, mwIndex dst_idx, uint32_t ir, uint32_t num_bits = 32)
		{
			((T*)data)[dst_idx] = (num_bits == 32)? (T)ir : (T)DXGIPixel::ChannelElement<DXGIPixel::FLOAT, mxSingle>::GetFloat(ir, num_bits);
		}
	};
	
	/* SRGB conversions */
	
	/* Don't use SRGB to FLOAT conversion because the MATLAB float representation
	 * is actually just a linear map to SRGB. Treat this as UNORM. */
	template <typename T>
	struct DXGIPixel::ChannelElement<DXGIPixel::SRGB, T>
	{
		static inline void Extract(void* data, mwIndex dst_idx, uint32_t ir, uint32_t num_bits = 8)
		{
			DXGIPixel::ChannelElement<DXGIPixel::UNORM, T>::Extract(data, dst_idx, ir, num_bits);
		}
	};
	
	/* SHAREDEXP conversions */
	/* RAW DATA CONVERSION */
	template <typename T>
	struct DXGIPixel::ChannelElement<DXGIPixel::SHAREDEXP, T>
	{
		static inline void Extract(void* data, mwIndex dst_idx, uint32_t ir, uint32_t num_bits = 16)
		{
			DXGIPixel::ChannelElement<DXGIPixel::UINT, T>::Extract(data, dst_idx, ir, num_bits);
		}
	};
	
	/* XR_BIAS conversions */
	/* NATURAL CONVERSION */
	template <typename T>
	struct DXGIPixel::ChannelElement<DXGIPixel::XR_BIAS, T, typename std::enable_if<std::is_floating_point<T>::value>::type>
	{
		static inline void Extract(void* data, mwIndex dst_idx, uint32_t ir, uint32_t)
		{
			((T*)data)[dst_idx] = (T)((ir & 0x3FFu) - 0x180) / 510.0f;
		}
	};
	
	/* RAW DATA CONVERSION */
	template <typename T>
	struct DXGIPixel::ChannelElement<DXGIPixel::XR_BIAS, T, typename std::enable_if<std::is_integral<T>::value>::type>
	{
		static inline void Extract(void* data, mwIndex dst_idx, uint32_t ir, uint32_t num_bits = 32)
		{
			DXGIPixel::ChannelElement<DXGIPixel::UINT, T>::Extract(data, dst_idx, ir, num_bits);
		}
	};	
	
}

