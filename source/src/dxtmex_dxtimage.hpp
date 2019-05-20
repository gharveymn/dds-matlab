#pragma once

#include "mex.h"
#include "DirectXTex.h"
#include <string>

namespace DXTMEX
{
	namespace MEXToDXT
	{
		enum class COLORSPACE
		{
			LINEAR,
			SRGB,
			DEFAULT = SRGB
		};

		template <typename MX_TYPE, typename DXT_TYPE, int NCHANNELS, COLORSPACE CS, typename Enable = void>
		struct Converter
		{
			static void ToIntermediate(uint8_t* in_data, DirectX::Image* out_img);
		};

		using f_toir = void (*)(uint8_t*, DirectX::Image*);

		f_toir GetToIRFunction(mxClassID class_id, int num_channels, COLORSPACE input_colorspace);

		template <typename MX_TYPE, typename DXT_TYPE>
		f_toir GetToIRFunction(int num_channels, COLORSPACE input_colorspace)
		{
			switch(num_channels)
			{
				case 1:  return GetToIRFunction<MX_TYPE, DXT_TYPE, 1>(input_colorspace);
				case 2:  return GetToIRFunction<MX_TYPE, DXT_TYPE, 2>(input_colorspace);
				case 3:  return GetToIRFunction<MX_TYPE, DXT_TYPE, 3>(input_colorspace);
				case 4:
				default: return GetToIRFunction<MX_TYPE, DXT_TYPE, 4>(input_colorspace);
			}
		}

		template <typename MX_TYPE, typename DXT_TYPE, int NCHANNELS>
		f_toir GetToIRFunction(COLORSPACE input_colorspace)
		{
			switch(input_colorspace)
			{
				case COLORSPACE::LINEAR: return Converter<MX_TYPE, DXT_TYPE, NCHANNELS, COLORSPACE::LINEAR>::ToIntermediate;
				case COLORSPACE::SRGB:
				default:                 return Converter<MX_TYPE, DXT_TYPE, NCHANNELS, COLORSPACE::SRGB>::ToIntermediate;
			}
		}

		static DirectX::TexMetadata DeriveMetadata(const mxArray* data_in,
			COLORSPACE input_colorspace,
			DirectX::TEX_ALPHA_MODE alpha_mode,
			bool is_cubemap);

		void DoConversion(DirectX::Image* img_out, const mxArray* data_in, size_t depth, COLORSPACE input_colorspace);

		void ConvertToIntermediate(DirectX::ScratchImage& scimg_out,
			const mxArray* data_in,
			COLORSPACE input_colorspace,
			DirectX::TEX_ALPHA_MODE alpha_mode,
			bool is_cubemap,
			DirectX::CP_FLAGS cp_flags);

		void ConvertToOutput(DXGI_FORMAT fmt_out,
			DirectX::TEX_FILTER_FLAGS filter_flags,
			float threshold,
			DirectX::ScratchImage& scimg_out,
			const mxArray* data_in,
			COLORSPACE input_colorspace,
			DirectX::TEX_ALPHA_MODE alpha_mode,
			bool is_cubemap,
			DirectX::CP_FLAGS cp_flags);


	};


	class DXTImage : public DirectX::ScratchImage
	{
	public:

		enum class IMAGE_TYPE
		{
			UNKNOWN,
			DDS,
			HDR,
			TGA
		};

		DXTImage() : _type(IMAGE_TYPE::UNKNOWN), _dds_flags(DirectX::DDS_FLAGS_NONE) {};
		DXTImage(const mxArray* data_in,
			MEXToDXT::COLORSPACE input_colorspace,
			DirectX::TEX_ALPHA_MODE alpha_mode,
			bool is_cubemap,
			DWORD flags);
		explicit DXTImage(const mxArray* mx_data) : DXTImage(mx_data, MEXToDXT::COLORSPACE::SRGB, DirectX::TEX_ALPHA_MODE::TEX_ALPHA_MODE_UNKNOWN, false, 0) {};
		explicit DXTImage(const DirectX::DDS_FLAGS flags) : _type(IMAGE_TYPE::UNKNOWN), _dds_flags(flags) {};
		explicit DXTImage(const IMAGE_TYPE type) : _type(IMAGE_TYPE::UNKNOWN), _dds_flags(DirectX::DDS_FLAGS::DDS_FLAGS_NONE) {};
		DXTImage(const IMAGE_TYPE type, const DirectX::DDS_FLAGS flags) : _type(type), _dds_flags(flags) {};
		DXTImage(const mxArray* mx_metadata, const mxArray* mx_images);  // MATLAB 'DXTImage' object -> MEX object
		DXTImage(const mxArray* mx_width, const mxArray* mx_height, const mxArray* mx_row_pitch, const mxArray* mx_slice_pitch, const mxArray* mx_pixels, const mxArray* mx_formatid, const mxArray* mx_flags);
		// Inherits:
		// size_t      m_nimages;
		// size_t      m_size;
		// TexMetadata m_metadata;
		// Image*      m_image;
		// uint8_t*    m_memory;

		DXTImage& operator=(DXTImage&& in) noexcept
		{
			this->_type = in._type;
			this->_dds_flags = in._dds_flags;
			this->ScratchImage::operator=(std::move(in));
			return *this;
		}

		void SetFlags(DirectX::DDS_FLAGS flags) { _dds_flags = flags; }
		DirectX::DDS_FLAGS GetFlags() { return _dds_flags; }

		void SetImageType(DXTImage::IMAGE_TYPE type) { _type = type; }
		DXTImage::IMAGE_TYPE GetImageType() { return _type; }

		mwIndex ComputeIndexMEX(size_t mip, size_t item, size_t slice);

		static mxArray* ExportMetadata(const DirectX::TexMetadata& metadata, DXTImage::IMAGE_TYPE type);
		mxArray* ExportMetadata();
		mxArray* ExportImages();

		static bool IsDXTImageImport(const mxArray* in);
		static bool IsDXTImageSliceImport(const mxArray* in);

		void ToImage(mxArray*& mx_dxtimage_rgb, bool combine_alpha);
		void ToImage(mxArray*& mx_dxtimage_rgb, mxArray*& mx_dxtimage_a);
		void ToImageMatrix(mxArray*& mx_dxtimage_rgb, bool combine_alpha);
		void ToImageMatrix(mxArray*& mx_dxtimage_rgb, mxArray*& mx_dxtimage_a);

		void ToMatrix(mxArray*& mx_dxtimage_out);

		void WriteHDR(const std::wstring& filename, std::wstring& ext, bool remove_idx_if_singular = false);
		void WriteHDR(const std::wstring& filename, size_t mip, size_t item, size_t slice);

		void WriteTGA(const std::wstring& filename, std::wstring& ext, bool remove_idx_if_singular = false);
		void WriteTGA(const std::wstring& filename, size_t mip, size_t item, size_t slice);

	private:
		IMAGE_TYPE         _type;
		DirectX::DDS_FLAGS _dds_flags;

		static mxArray* ExportFormat(DXGI_FORMAT fmt);

		void PrepareImages(DXTImage& out);
		static void ImportMetadata(const mxArray* mx_metadata, DirectX::TexMetadata& metadata);
		static void ImportImages(const mxArray* mx_images, DirectX::Image* images, size_t array_size, size_t mip_levels, size_t depth, DirectX::TEX_DIMENSION dimension);
	};


}
