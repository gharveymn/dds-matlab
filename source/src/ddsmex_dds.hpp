#pragma once

#include "mex.h"
#include "directxtex.h"
#include <map>

#define MEXF_IN  int nrhs, const mxArray* prhs[]
#define MEXF_OUT int nlhs, mxArray* plhs[]
#define MEXF_SIG MEXF_OUT, MEXF_IN

namespace DDSMEX
{
	// Read                            | DONE
	// ReadMetadata                    | DONE
	// FlipRotate                      | DONE
	// Resize                          |
	// Convert                         | DONE
	// ConvertToSinglePlane            |
	// GenerateMipMaps                 |
	// GenerateMipMaps3D               |
	// ScaleMipMapsAlphaForCoverage    |
	// PremultiplyAlpha                |
	// Compress                        |
	// Decompress                      | DONE
	// ComputeNormalMap                |
	// ComputeMSE                      |
	
	// ToImageMatrix                   | PARTIAL
	// ToMatrix                        | PARTIAL
	
	class DDS : public DirectX::ScratchImage
	{
	public:
		DDS() : _flags(DirectX::CP_FLAGS_NONE) {};
		explicit DDS(DWORD flags) : _flags(flags) {};
		explicit DDS(const mxArray* mx_width, const mxArray* mx_height, const mxArray* mx_row_pitch, const mxArray* mx_slice_pitch, const mxArray* mx_pixels, const mxArray* mx_formatid, const mxArray* mx_flags);
		DDS(const mxArray* mx_metadata, const mxArray* mx_images);  // MATLAB 'Dds' object -> MEX object
		// Inherits:
		// size_t      m_nimages;
		// size_t      m_size;
		// TexMetadata m_metadata;
		// Image*      m_image;
		// uint8_t*    m_memory;
		
		DDS& operator=(DDS&& in) noexcept
		{
			this->_flags = in._flags;
			DirectX::ScratchImage::operator=(std::move(in));
			return *this;
		}
		
		void SetFlags(DWORD flags) {_flags = flags;}
		DWORD GetFlags() {return _flags;}
		
		mwIndex ComputeIndexMEX(size_t mip, size_t item, size_t slice);
		
		mxArray* ExportMetadata();
		mxArray* ExportImages();
		static mxArray* ExportMetadata(const DirectX::TexMetadata& metadata);
		static mxArray* ExportMetadata(const DirectX::TexMetadata& metadata, DWORD flags);
		
		static bool IsDdsImport(const mxArray* in);
		static bool IsDdsSliceImport(const mxArray* in);
		
		void ToImage   (mxArray*& mx_dds_rgb);
		void ToImage   (mxArray*& mx_dds_rgb, mxArray*& mx_dds_a);
		void ToMatrix  (mxArray*& mat_out, bool combine_alpha);
		void ToMatrix  (mxArray*& mx_dds_rgb, mxArray*& mx_dds_a);
		
	private:
		DWORD _flags;
		
		static mxArray* ExportFormat(DXGI_FORMAT fmt);
		
		void PrepareImages(DDS &out);
		void ImportMetadata(const mxArray* mx_metadata, DirectX::TexMetadata& metadata);
		void ImportImages(const mxArray* mx_images, DirectX::Image* images, size_t array_size, size_t mip_levels, size_t depth, DirectX::TEX_DIMENSION dimension);
		void FormMatrix(mxArray*& mx_dds_rgb, bool combine_alpha = false);
		void FormMatrix(mxArray*& mx_dds_rgb, mxArray*& mx_dds_a);
		void FormMatrix(const DirectX::Image* raw_img, mxArray*& mx_ddsslice_rgb, bool combine_alpha);
		void FormMatrix(const DirectX::Image* raw_img, mxArray*& mx_ddsslice_rgb, mxArray*& mx_ddsslice_a);
	};
	
	class DDSArray
	{
	public:
		DDSArray() : _arr(nullptr), _sz_m(0), _sz_n(0), _size(0) {};
		DDSArray(size_t m, size_t n, DWORD flags = DirectX::CP_FLAGS_NONE);
		~DDSArray() {delete[] _arr;}
		
		/* move */
		DDSArray(DDSArray&& in) noexcept
		{
			this->_arr  = in._arr;
			this->_sz_m = in._sz_m;
			this->_sz_n = in._sz_n;
			this->_size = in._size;
		}
		
		/* assign */
		DDSArray& operator=(const DDSArray& in) = default;
		
		/* move */
		DDSArray& operator=(DDSArray&& in) = default;
		
		/* initializers */
		void ReadFile                    (MEXF_IN);
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
		static void CopyRectangle        (DDSArray& src, DDSArray& dst, MEXF_IN);
		void ComputeNormalMap            (MEXF_IN);
		static void ComputeMSE           (DDSArray& ddsarray1, DDSArray& ddsarray2, MEXF_SIG);
		
		void SaveFile                    (MEXF_IN);
		
		void ToImage                     (MEXF_SIG);
		void ToMatrix                    (MEXF_SIG);
		
		void ToExport                    (MEXF_OUT);
		
		static void ReadFile             (MEXF_SIG)
		{
			DDSArray dds_array;
			dds_array.ReadFile(nrhs, prhs);
			dds_array.ToExport(nlhs, plhs);
		}
		
		static void ReadMetadata         (MEXF_SIG);
		
		static void ParseFlags(const mxArray* mx_flags, BiMap &map, DWORD &flags);
		static DXGI_FORMAT ParseFormat(const mxArray* mx_fmt);
		
		DDS& GetDDS(size_t idx) {return _arr[idx];}
		size_t GetSize() {return _size;}
		size_t GetM() {return _sz_m;}
		size_t GetN() {return _sz_n;}
		
		enum operation
		{
			NO_OP,
			READ_FILE,
			READ_META,
			FLIP_ROTATE,
			RESIZE,
			CONVERT,
			CONVERT_TO_SINGLE_PLANE,
			GENERATE_MIPMAPS,
			GENERATE_MIPMAPS_3D,
			SCALE_MIPMAPS_ALPHA_FOR_COVERAGE,
			PREMULTIPLY_ALPHA,
			COMPRESS,
			DECOMPRESS,
			COMPUTE_NORMAL_MAP,
			COPY_RECTANGLE,
			COMPUTE_MSE,
			SAVE_FILE,
			TO_IMAGE,
			TO_MATRIX
		};
		
		static DDSArray::operation GetOperation(const mxArray* directive);
	
	private:
		DDS*          _arr;
		size_t        _sz_m;
		size_t        _sz_n;
		size_t        _size;
		
		/* import helpers */
		static wchar_t* ParseFilename(const mxArray* mx_filename);
		static void     ParseFilename(const mxArray* mx_filename, std::wstring& filename);
		static DDS*     AllocateDDSArray(size_t num);
		static DDS*     AllocateDDSArray(size_t num, DDS* in);
		static DDS*     AllocateDDSArray(size_t num, DWORD flags);
		static void     ComputeMSE(DDS& dds1, DDS& dds2, DWORD cmse_flags, mxArray*& mx_dds_mse);
		static void     ComputeMSE(DDS& dds1, DDS& dds2, DWORD cmse_flags, mxArray*& mx_dds_mse, mxArray*& mx_dds_mseV);
		static void     ComputeMSE(const DirectX::Image* img1, const DirectX::Image* img2, DWORD cmse_flags, mxArray*& mx_ddsslice_mse);
		static void     ComputeMSE(const DirectX::Image* img1, const DirectX::Image* img2, DWORD cmse_flags, mxArray*& mx_ddsslice_mse, mxArray*& mx_ddsslice_mseV);
	};
}
