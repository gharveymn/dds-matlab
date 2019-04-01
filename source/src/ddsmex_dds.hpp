#pragma once

#include "mex.h"
#include "directxtex.h"

#define MEXFUNCTION_SIGNATURE int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]

namespace DDSMex
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
	
	class DDSImage : public DirectX::ScratchImage
	{
	public:
		DDSImage() : m_flags(DirectX::CP_FLAGS_NONE) {};
		DDSImage(const mxArray* mx_metadata, const mxArray* mx_images);  // MATLAB 'Dds' object -> MEX object
		// Inherits:
		// size_t      m_nimages;
		// size_t      m_size;
		// TexMetadata m_metadata;
		// Image*      m_image;
		// uint8_t*    m_memory;
		
		mxArray* ExportMetadata();
		mxArray* ExportImages();
		
	private:
		DWORD m_flags;
		mxArray* ExportFormat(DXGI_FORMAT fmt);
		void ImportMetadata(const mxArray* mx_metadata, DirectX::TexMetadata &metadata);
		void ImportImages(const mxArray* mx_images, DirectX::Image* images, size_t array_size, size_t mip_levels, size_t depth, DirectX::TEX_DIMENSION dimension);
		
	};
	
	class DDS
	{
	public:
		DDS() : dds_images(nullptr), m_sz(0), n_sz(0) {};
		~DDS() {if(dds_images) mxFree(dds_images);}
		
		/* static constructors */
		static DDS ReadFile    (int nrhs, const mxArray* prhs[]);
		static DDS ReadMetadata(int nrhs, const mxArray* prhs[]);
		static DDS Import      (int nrhs, const mxArray* prhs[]);
		
		void FlipRotate                  (MEXFUNCTION_SIGNATURE);
		void Resize                      (MEXFUNCTION_SIGNATURE);
		void Convert                     (MEXFUNCTION_SIGNATURE);
		void ConvertToSinglePlane        (MEXFUNCTION_SIGNATURE);
		void GenerateMipMaps             (MEXFUNCTION_SIGNATURE);
		void GenerateMipMaps3D           (MEXFUNCTION_SIGNATURE);
		void ScaleMipMapsAlphaForCoverage(MEXFUNCTION_SIGNATURE);
		void PremultiplyAlpha            (MEXFUNCTION_SIGNATURE);
		void Compress                    (MEXFUNCTION_SIGNATURE);
		void Decompress                  (MEXFUNCTION_SIGNATURE);
		void ComputeNormalMap            (MEXFUNCTION_SIGNATURE);
		void ComputeMSE                  (MEXFUNCTION_SIGNATURE);
		void AsImage                     (MEXFUNCTION_SIGNATURE);
		void AsMatrix                    (MEXFUNCTION_SIGNATURE);
	
	private:
		DDSImage*     dds_images;
		mwSize        m_sz;
		mwSize        n_sz;
		
		mwSize        num_images() {return m_sz * n_sz;};
		
		void ToExport(int nlhs, mxArray* plhs[]);
		
		/* import helpers */
		wchar_t* ImportFilename(const mxArray* mx_filename);
		void ImportFlags(const mxArray* flags_array, BiMap& map, DWORD& flags);
		
		/* export helpers */
		mxArray* ExportFormat(DXGI_FORMAT fmt);
	};
}
