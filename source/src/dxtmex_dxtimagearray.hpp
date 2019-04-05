#pragma once

#include "mex.h"
#include "DirectXTex.h"

#ifndef MAX
#  define MAX(a,b) a > b? a : b
#endif

#define MEXF_IN  int nrhs, const mxArray* prhs[]
#define MEXF_OUT int nlhs, mxArray* plhs[]
#define MEXF_SIG MEXF_OUT, MEXF_IN

namespace DXTMEX
{
	class DXTImage : public DirectX::ScratchImage
	{
	public:
		DXTImage() : _dds_flags(DirectX::DDS_FLAGS_NONE), _type(DXTImage::UNKNOWN) {};
		explicit DXTImage(DirectX::DDS_FLAGS flags) : _dds_flags(flags), _type(DXTImage::UNKNOWN) {};
		explicit DXTImage(const mxArray* mx_width, const mxArray* mx_height, const mxArray* mx_row_pitch, const mxArray* mx_slice_pitch, const mxArray* mx_pixels, const mxArray* mx_formatid, const mxArray* mx_flags);
		DXTImage(const mxArray* mx_metadata, const mxArray* mx_images);  // MATLAB 'DXTImage' object -> MEX object
		// Inherits:
		// size_t      m_nimages;
		// size_t      m_size;
		// TexMetadata m_metadata;
		// Image*      m_image;
		// uint8_t*    m_memory;
		
		DXTImage& operator=(DXTImage&& in) noexcept
		{
			this->_dds_flags = in._dds_flags;
			DirectX::ScratchImage::operator=(std::move(in));
			return *this;
		}
		
		enum IMAGE_TYPE
		{
			UNKNOWN = 0x0,
			DDS     = 0x1,
			HDR     = 0x2,
			TGA     = 0x3
		};
		
		void SetFlags(DirectX::DDS_FLAGS flags) {_dds_flags = flags;}
		DirectX::DDS_FLAGS GetFlags() {return _dds_flags;}
		
		void SetImageType(DXTImage::IMAGE_TYPE type) {_type = type;}
		DXTImage::IMAGE_TYPE GetImageType() {return _type;}
		
		mwIndex ComputeIndexMEX(size_t mip, size_t item, size_t slice);
		
		static mxArray* ExportMetadata(const DirectX::TexMetadata& metadata, DXTImage::IMAGE_TYPE type);
		mxArray* ExportMetadata();
		mxArray* ExportImages();
		
		static bool IsDXTImageImport(const mxArray* in);
		static bool IsDXTImageSliceImport(const mxArray* in);
		
		void ToImage   (mxArray*& mx_dxtimage_rgb);
		void ToImage   (mxArray*& mx_dxtimage_rgb, mxArray*& mx_dxtimage_a);
		void ToMatrix  (mxArray*& mx_dxtimage_rgb, bool combine_alpha);
		void ToMatrix  (mxArray*& mx_dxtimage_rgb, mxArray*& mx_dxtimage_a);
		
		void WriteHDR(std::wstring &filename, std::wstring &ext, bool remove_idx_if_singular = false);
		void WriteHDR(std::wstring &filename, size_t mip, size_t item, size_t slice);
		
		void WriteTGA(std::wstring &filename, std::wstring &ext, bool remove_idx_if_singular = false);
		void WriteTGA(std::wstring &filename, size_t mip, size_t item, size_t slice);
		
	private:
		DXTImage::IMAGE_TYPE _type;
		DirectX::DDS_FLAGS _dds_flags;
		
		static mxArray* ExportFormat(DXGI_FORMAT fmt);
		
		void PrepareImages(DXTImage &out);
		static void ImportMetadata(const mxArray* mx_metadata, DirectX::TexMetadata& metadata);
		static void ImportImages(const mxArray* mx_images, DirectX::Image* images, size_t array_size, size_t mip_levels, size_t depth, DirectX::TEX_DIMENSION dimension);
		void FormMatrix(mxArray*& mx_dxtimage_rgb, bool combine_alpha = false);
		void FormMatrix(mxArray*& mx_dxtimage_rgb, mxArray*& mx_dxtimage_a);
		static void FormMatrix(const DirectX::Image* raw_img, mxArray*& mx_dxtimageslice_rgb, bool combine_alpha);
		static void FormMatrix(const DirectX::Image* raw_img, mxArray*& mx_dxtimageslice_rgb, mxArray*& mx_dxtimageslice_a);
	};
	
	class DXTImageArray
	{
	public:
		DXTImageArray() : _arr(nullptr), _sz_m(0), _sz_n(0), _size(0) {};
		~DXTImageArray() {delete[] _arr;}
		
		/* move */
		DXTImageArray(DXTImageArray&& in) noexcept
		{
			this->_arr  = in._arr;
			this->_sz_m = in._sz_m;
			this->_sz_n = in._sz_n;
			this->_size = in._size;
		}
		
		/* assign */
		DXTImageArray& operator=(const DXTImageArray& in) = default;
		
		/* move */
		DXTImageArray& operator=(DXTImageArray&& in) = default;
		
		/* initializers */
		void ReadDDS                     (MEXF_IN);
		void ReadHDR                     (MEXF_IN);
		void ReadTGA                     (MEXF_IN);
		void Import                      (MEXF_IN);
		
		/* utilities */
		void FlipRotate                  (MEXF_IN);
		void Resize                      (MEXF_IN);
		void Convert                     (MEXF_IN);
		void ConvertToSinglePlane        (MEXF_IN);
		void GenerateMipMaps             (MEXF_IN);
		// void GenerateMipMaps3D           (MEXF_IN);
		void ScaleMipMapsAlphaForCoverage(MEXF_IN);
		void PremultiplyAlpha            (MEXF_IN);
		void Compress                    (MEXF_IN);
		void Decompress                  (MEXF_IN);
		void ComputeNormalMap            (MEXF_IN);
		static void CopyRectangle        (DXTImageArray& dst, DXTImageArray& src, MEXF_IN);
		static void ComputeMSE           (DXTImageArray& dxtimagearray1, DXTImageArray& dxtimagearray2, MEXF_SIG);
		
		void WriteDDS                    (MEXF_IN);
		void WriteHDR                    (MEXF_IN);
		void WriteTGA                    (MEXF_IN);
		
		void ToImage                     (MEXF_SIG);
		void ToMatrix                    (MEXF_SIG);
		
		void ToExport                    (MEXF_OUT);
		
		static void ReadDDS(MEXF_SIG)
		{
			DXTImageArray dxtimage_array;
			dxtimage_array.ReadDDS(nrhs, prhs);
			dxtimage_array.ToExport(nlhs, plhs);
		}
		
		static void ReadHDR(MEXF_SIG)
		{
			DXTImageArray dxtimage_array;
			dxtimage_array.ReadHDR(nrhs, prhs);
			dxtimage_array.ToExport(nlhs, plhs);
		}
		
		static void ReadTGA          (MEXF_SIG)
		{
			DXTImageArray dxtimage_array;
			dxtimage_array.ReadTGA(nrhs, prhs);
			dxtimage_array.ToExport(nlhs, plhs);
		}
		
		static void ReadDDSMetadata      (MEXF_SIG);
		static void ReadHDRMetadata      (MEXF_SIG);
		static void ReadTGAMetadata      (MEXF_SIG);
		
		static void IsDDS(MEXF_SIG);
		static void IsHDR(MEXF_SIG);
		static void IsTGA(MEXF_SIG);
		
		static DXGI_FORMAT ParseFormat(const mxArray* mx_fmt);
		
		DXTImage& GetDXTImage(size_t idx)
		{
			return _arr[idx];
		}
		
		void SetDXTImage(size_t idx, DXTImage&& img)
		{
			_arr[idx] = std::move(img);
		}
		
		size_t GetSize() {return _size;}
		size_t GetM() {return _sz_m;}
		size_t GetN() {return _sz_n;}
		
		enum operation
		{
			NO_OP                            = 0x00,
			READ_DDS                         = 0x01,
			READ_HDR                         = 0x02,
			READ_TGA                         = 0x03,
			WRITE_DDS                        = 0x04,
			WRITE_HDR                        = 0x05,
			WRITE_TGA                        = 0x06,
			READ_DDS_META                    = 0x07,
			READ_HDR_META                    = 0x08,
			READ_TGA_META                    = 0x09,
			IS_DDS                           = 0x10,
			IS_HDR                           = 0x11,
			IS_TGA                           = 0x12,
			FLIP_ROTATE                      = 0x13,
			RESIZE                           = 0x14,
			CONVERT                          = 0x15,
			CONVERT_TO_SINGLE_PLANE          = 0x16,
			GENERATE_MIPMAPS                 = 0x17,
			GENERATE_MIPMAPS_3D              = 0x18,
			SCALE_MIPMAPS_ALPHA_FOR_COVERAGE = 0x19,
			PREMULTIPLY_ALPHA                = 0x20,
			COMPRESS                         = 0x21,
			DECOMPRESS                       = 0x22,
			COMPUTE_NORMAL_MAP               = 0x23,
			COPY_RECTANGLE                   = 0x24,
			COMPUTE_MSE                      = 0x25,
			TO_IMAGE                         = 0x26,
			TO_MATRIX                        = 0x27
		};
		
		static DXTImageArray::operation GetOperation(const mxArray* directive);
	
	private:
		DXTImage*     _arr;
		size_t        _sz_m;
		size_t        _sz_n;
		size_t        _size;
		
		void Initialize(size_t m, size_t n)
		{
			this->_sz_m = m;
			this->_sz_n = n;
			this->_size = m*n;
			this->_arr  = AllocateDXTImageArray(this->_size);
		}
		
		void Initialize(size_t m, size_t n, DXTImage::IMAGE_TYPE type)
		{
			this->_sz_m = m;
			this->_sz_n = n;
			this->_size = m*n;
			this->_arr  = AllocateDXTImageArray(this->_size, type);
		}
		
		void Initialize(size_t m, size_t n, DXTImage::IMAGE_TYPE type, DirectX::DDS_FLAGS flags)
		{
			this->_sz_m = m;
			this->_sz_n = n;
			this->_size = m*n;
			this->_arr  = AllocateDXTImageArray(this->_size, type, flags);
		}
		
		size_t GetNumberOfSlices()
		{
			size_t i, total = 0;
			for(i = 0; i < this->_size; i++)
			{
				total += this->_arr[i].GetImageCount();
			}
		}
		
		void ReplaceArray(DXTImage* new_arr)
		{
			delete[](this->_arr);
			this->_arr = new_arr;
		}
		
		/* import helpers */
		static void      ImportFilename(const mxArray* mx_filename, std::wstring &filename);
		DXTImage*        CopyDXTImageArray();
		static DXTImage* AllocateDXTImageArray(size_t num);
		static DXTImage* AllocateDXTImageArray(size_t num, DXTImage::IMAGE_TYPE type);
		static DXTImage* AllocateDXTImageArray(size_t num, DXTImage::IMAGE_TYPE type, DirectX::DDS_FLAGS flags);
		static void      ComputeMSE(DXTImage& dxtimage1, DXTImage& dxtimage2, DirectX::CMSE_FLAGS cmse_flags, mxArray*& mx_dxtimage_mse);
		static void      ComputeMSE(DXTImage& dxtimage1, DXTImage& dxtimage2, DirectX::CMSE_FLAGS cmse_flags, mxArray*& mx_dxtimage_mse, mxArray*& mx_dxtimage_mseV);
		static void      ComputeMSE(const DirectX::Image* img1, const DirectX::Image* img2, DirectX::CMSE_FLAGS cmse_flags, mxArray*& mx_dxtimageslice_mse);
		static void      ComputeMSE(const DirectX::Image* img1, const DirectX::Image* img2, DirectX::CMSE_FLAGS cmse_flags, mxArray*& mx_dxtimageslice_mse, mxArray*& mx_dxtimageslice_mseV);
	};
}
