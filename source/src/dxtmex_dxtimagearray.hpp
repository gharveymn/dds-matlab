#pragma once

#include <string>

#include "mex.h"
#include "DirectXTex.h"
#include "dxtmex_dxtimage.hpp"
#include <memory>

#define MEXF_IN  int nrhs, const mxArray* prhs[]
#define MEXF_OUT int nlhs, mxArray* plhs[]
#define MEXF_SIG MEXF_OUT, MEXF_IN

namespace DXTMEX
{
	
	class DXTImageArray
	{
	public:
		DXTImageArray() : _arr(nullptr), _sz_m(0), _sz_n(0), _size(0) {};

		/* move */
		DXTImageArray(DXTImageArray&& in) noexcept
		{
			this->_arr  = std::move(in._arr);
			this->_sz_m = in._sz_m;
			this->_sz_n = in._sz_n;
			this->_size = in._size;
		}
		
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

		static void WriteMatrixDDS       (MEXF_IN);
		static void WriteMatrixHDR       (MEXF_IN);
		static void WriteMatrixTGA       (MEXF_IN);
		
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
		
		enum class OPERATION
		{
			NO_OP                           ,
			READ_DDS                        ,
			READ_HDR                        ,
			READ_TGA                        ,
			WRITE_DDS                       ,
			WRITE_HDR                       ,
			WRITE_TGA                       ,
			WRITE_MATRIX_DDS                ,
			WRITE_MATRIX_HDR                ,
			WRITE_MATRIX_TGA                ,
			READ_DDS_META                   ,
			READ_HDR_META                   ,
			READ_TGA_META                   ,
			IS_DDS                          ,
			IS_HDR                          ,
			IS_TGA                          ,
			FLIP_ROTATE                     ,
			RESIZE                          ,
			CONVERT                         ,
			CONVERT_TO_SINGLE_PLANE         ,
			GENERATE_MIPMAPS                ,
			GENERATE_MIPMAPS_3D             ,
			SCALE_MIPMAPS_ALPHA_FOR_COVERAGE,
			PREMULTIPLY_ALPHA               ,
			COMPRESS                        ,
			DECOMPRESS                      ,
			COMPUTE_NORMAL_MAP              ,
			COPY_RECTANGLE                  ,
			COMPUTE_MSE                     ,
			TO_IMAGE                        ,
			TO_MATRIX                       ,
		};
		
		static DXTImageArray::OPERATION GetOperation(const mxArray* directive);
	
	private:
		std::unique_ptr<DXTImage[]> _arr;
		size_t                      _sz_m;
		size_t                      _sz_n;
		size_t                      _size;
		
		void Initialize(size_t m, size_t n)
		{
			this->_sz_m = m;
			this->_sz_n = n;
			this->_size = m*n;
			this->_arr = std::make_unique<DXTImage[]>(this->_size);
		}
		
		void Initialize(size_t m, size_t n, DXTImage::IMAGE_TYPE type)
		{
			this->_sz_m = m;
			this->_sz_n = n;
			this->_size = m*n;
			this->_arr = std::make_unique<DXTImage[]>(this->_size);
			for(size_t i = 0; i < this->_size; i++)
			{
				this->_arr[i].SetImageType(type);
			}
		}
		
		void Initialize(size_t m, size_t n, DXTImage::IMAGE_TYPE type, DirectX::DDS_FLAGS flags)
		{
			this->_sz_m = m;
			this->_sz_n = n;
			this->_size = m*n;
			this->_arr = std::make_unique<DXTImage[]>(this->_size);
			for(size_t i = 0; i < this->_size; i++)
			{
				this->_arr[i].SetImageType(type);
				this->_arr[i].SetFlags(flags);
			}
		}
		
		size_t GetNumberOfSlices()
		{
			size_t total = 0;
			for(size_t i = 0; i < this->_size; i++)
			{
				total += this->_arr[i].GetImageCount();
			}
			return total;
		}
		
		/* import helpers */
		static void      ImportFilename(const mxArray* mx_filename, std::wstring &filename);
		std::unique_ptr<DXTImage[]> CopyDXTImageArray();
		static void      ComputeMSE(DXTImage& dxtimage1, DXTImage& dxtimage2, DirectX::CMSE_FLAGS cmse_flags, mxArray*& mx_dxtimage_mse);
		static void      ComputeMSE(DXTImage& dxtimage1, DXTImage& dxtimage2, DirectX::CMSE_FLAGS cmse_flags, mxArray*& mx_dxtimage_mse, mxArray*& mx_dxtimage_mseV);
		static void      ComputeMSE(const DirectX::Image* img1, const DirectX::Image* img2, DirectX::CMSE_FLAGS cmse_flags, mxArray*& mx_dxtimageslice_mse);
		static void      ComputeMSE(const DirectX::Image* img1, const DirectX::Image* img2, DirectX::CMSE_FLAGS cmse_flags, mxArray*& mx_dxtimageslice_mse, mxArray*& mx_dxtimageslice_mseV);
	};
}
