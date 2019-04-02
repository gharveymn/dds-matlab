#pragma once

#include "mex.h"
#include "directxtex.h"

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
		DDS(const mxArray* mx_metadata, const mxArray* mx_images);  // MATLAB 'Dds' object -> MEX object
		// Inherits:
		// size_t      m_nimages;
		// size_t      m_size;
		// TexMetadata m_metadata;
		// Image*      m_image;
		// uint8_t*    m_memory;
		
		void SetFlags(DWORD flags) {_flags = flags;}
		DWORD GetFlags() {return _flags;}
		
		mwIndex ComputeIndexMEX(size_t mip, size_t item, size_t slice);
		
		mxArray* ExportMetadata();
		mxArray* ExportImages();
		static mxArray* ExportMetadata(const DirectX::TexMetadata& metadata);
		static mxArray* ExportMetadata(const DirectX::TexMetadata& metadata, DWORD flags);
		
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
		
		/* assign */
		DDSArray& operator=(const DDSArray& in) = default;
		
		/* static constructors */
		static DDSArray ReadFile    (int nrhs, const mxArray* prhs[]);
		static DDSArray Import      (const mxArray* in_struct);
		
		void FlipRotate                  (MEXF_IN);
		void Resize                      (MEXF_IN);
		void Convert                     (MEXF_IN);
		void ConvertToSinglePlane        (MEXF_IN);
		void GenerateMipMaps             (MEXF_IN);
		void GenerateMipMaps3D           (MEXF_IN);
		void ScaleMipMapsAlphaForCoverage(MEXF_IN);
		void PremultiplyAlpha            (MEXF_IN);
		void Compress                    (MEXF_IN);
		void Decompress                  (MEXF_IN);
		void ComputeNormalMap            (MEXF_IN);
		void ComputeMSE                  (MEXF_IN);
		
		void ToImage                     (MEXF_SIG);
		void ToMatrix                    (MEXF_SIG);
		void ToExport                    (MEXF_OUT);
		
		static void ReadFile             (MEXF_SIG)
		{
			DDSArray::ReadFile(nrhs, prhs).ToExport(nlhs, plhs);
		}
		static void ReadMetadata         (MEXF_SIG);
		
		static void ParseFlags(const mxArray* flags_array, BiMap& map, DWORD& flags);
		static DXGI_FORMAT ParseFormat(const mxArray* mx_fmt);
		
		DDS& GetDDS(size_t idx) {return _arr[idx];}
	
	private:
		DDS*          _arr;
		size_t        _sz_m;
		size_t        _sz_n;
		size_t        _size;
		
		
		/* import helpers */
		static wchar_t* ParseFilename(const mxArray* mx_filename);
		static DDS*     AllocateDDSArray(size_t num, DDS* in);
		static DDS*     AllocateDDSArray(size_t num, DWORD flags);
	};
}
