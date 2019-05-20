#include "mex.h"
#include "dxtmex_mexerror.hpp"
#include "DirectXTex.h"
#include "DirectXTex.inl"
#include "dxtmex_dxtimagearray.hpp"
#include "dxtmex_flags.hpp"

using namespace DXTMEX;

const char* MEXError::g_library_name = "dxtmex";

/* The gateway function. */
void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	DXTImageArray dxtimage_array;
	
	if(nrhs < 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughInputsError", "Not enough arguments. Please supply a directive.");
	}
	
	if(!mxIsChar(prhs[0]))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InvalidDirectiveError", "The directive supplied must be of class 'char'.");
	}
	
	const DXTImageArray::OPERATION op = DXTImageArray::GetOperation(prhs[0]);
	
	const int num_in              = nrhs-1;
	const mxArray** in      = prhs+1;
	
	switch(op)
	{
		case DXTImageArray::OPERATION::READ_DDS:
		{
			DXTImageArray::ReadDDS(nlhs, plhs, num_in, in);
			return; // EARLY RETURN
		}
		case DXTImageArray::OPERATION::READ_HDR:
		{
			DXTImageArray::ReadHDR(nlhs, plhs, num_in, in);
			return; // EARLY RETURN
		}
		case DXTImageArray::OPERATION::READ_TGA:
		{
			DXTImageArray::ReadTGA(nlhs, plhs, num_in, in);
			return; // EARLY RETURN
		}
		case DXTImageArray::OPERATION::READ_DDS_META:
		{
			DXTImageArray::ReadDDSMetadata(nlhs, plhs, num_in, in);
			return; // EARLY RETURN
		}
		case DXTImageArray::OPERATION::READ_HDR_META:
		{
			DXTImageArray::ReadHDRMetadata(nlhs, plhs, num_in, in);
			return; // EARLY RETURN
		}
		case DXTImageArray::OPERATION::READ_TGA_META:
		{
			DXTImageArray::ReadTGAMetadata(nlhs, plhs, num_in, in);
			return; // EARLY RETURN
		}
		case DXTImageArray::OPERATION::IS_DDS:
		{
			DXTImageArray::IsDDS(nlhs, plhs, num_in, in);
			return; // EARLY RETURN
		}
		case DXTImageArray::OPERATION::IS_HDR:
		{
			DXTImageArray::IsHDR(nlhs, plhs, num_in, in);
			return; // EARLY RETURN
		}
		case DXTImageArray::OPERATION::IS_TGA:
		{
			DXTImageArray::IsTGA(nlhs, plhs, num_in, in);
			return; // EARLY RETURN
		}
		case DXTImageArray::OPERATION::WRITE_MATRIX_DDS:
		{
			DXTImageArray::WriteMatrixDDS(num_in, in);
			return; // EARLY RETURN
		}
		case DXTImageArray::OPERATION::WRITE_MATRIX_HDR:
		{
			DXTImageArray::WriteMatrixHDR(num_in, in);
			return; // EARLY RETURN
		}
		case DXTImageArray::OPERATION::WRITE_MATRIX_TGA:
		{
			DXTImageArray::WriteMatrixTGA(num_in, in);
			return; // EARLY RETURN
		}
		case DXTImageArray::OPERATION::WRITE_DDS:
		case DXTImageArray::OPERATION::WRITE_HDR:
		case DXTImageArray::OPERATION::WRITE_TGA:
		case DXTImageArray::OPERATION::FLIP_ROTATE:
		case DXTImageArray::OPERATION::RESIZE:
		case DXTImageArray::OPERATION::CONVERT:
		case DXTImageArray::OPERATION::CONVERT_TO_SINGLE_PLANE:
		case DXTImageArray::OPERATION::GENERATE_MIPMAPS:
		case DXTImageArray::OPERATION::GENERATE_MIPMAPS_3D:
		case DXTImageArray::OPERATION::SCALE_MIPMAPS_ALPHA_FOR_COVERAGE:
		case DXTImageArray::OPERATION::PREMULTIPLY_ALPHA:
		case DXTImageArray::OPERATION::COMPRESS:
		case DXTImageArray::OPERATION::DECOMPRESS:
		case DXTImageArray::OPERATION::COMPUTE_NORMAL_MAP:
		case DXTImageArray::OPERATION::COPY_RECTANGLE:
		case DXTImageArray::OPERATION::COMPUTE_MSE:
		case DXTImageArray::OPERATION::TO_IMAGE:
		case DXTImageArray::OPERATION::TO_MATRIX:
		{
			dxtimage_array.Import(num_in, in);
			break;
		}
		case DXTImageArray::OPERATION::NO_OP:
		default:
		{
			return;
		}
	}
	
	if(dxtimage_array.GetSize() < 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_INTERNAL, "NoImageSuppliedError", "No images were supplied. Cannot continue.");
	}
	
	const int num_options   = num_in-1;
	const mxArray** options = in+1;
	
	switch(op)
	{
		case DXTImageArray::OPERATION::WRITE_DDS:
		{
			dxtimage_array.WriteDDS(num_options, options);
			return; // EARLY RETURN
		}
		case DXTImageArray::OPERATION::WRITE_HDR:
		{
			dxtimage_array.WriteHDR(num_options, options);
			return; // EARLY RETURN
		}
		case DXTImageArray::OPERATION::WRITE_TGA:
		{
			dxtimage_array.WriteTGA(num_options, options);
			return; // EARLY RETURN
		}
		case DXTImageArray::OPERATION::TO_IMAGE:
		{
			dxtimage_array.ToImage(nlhs, plhs, num_options, options);
			return; // EARLY RETURN
		}
		case DXTImageArray::OPERATION::TO_MATRIX:
		{
			dxtimage_array.ToMatrix(nlhs, plhs, num_options, options);
			return; // EARLY RETURN
		}
		case DXTImageArray::OPERATION::FLIP_ROTATE:
		{
			dxtimage_array.FlipRotate(num_options, options);
			break;
		}
		case DXTImageArray::OPERATION::RESIZE:
		{
			dxtimage_array.Resize(num_options, options);
			break;
		}
		case DXTImageArray::OPERATION::CONVERT:
		{
			dxtimage_array.Convert(num_options, options);
			break;
		}
		case DXTImageArray::OPERATION::CONVERT_TO_SINGLE_PLANE:
		{
			dxtimage_array.ConvertToSinglePlane(num_options, options);
			break;
		}
		case DXTImageArray::OPERATION::GENERATE_MIPMAPS:
		{
			dxtimage_array.GenerateMipMaps(num_options, options);
			break;
		}
		case DXTImageArray::OPERATION::SCALE_MIPMAPS_ALPHA_FOR_COVERAGE:
		{
			dxtimage_array.ScaleMipMapsAlphaForCoverage(num_options, options);
			break;
		}
		case DXTImageArray::OPERATION::PREMULTIPLY_ALPHA:
		{
			dxtimage_array.PremultiplyAlpha(num_options, options);
			break;
		}
		case DXTImageArray::OPERATION::COMPRESS:
		{
			dxtimage_array.Compress(num_options, options);
			break;
		}
		case DXTImageArray::OPERATION::DECOMPRESS:
		{
			dxtimage_array.Decompress(num_options, options);
			break;
		}
		case DXTImageArray::OPERATION::COMPUTE_NORMAL_MAP:
		{
			dxtimage_array.ComputeNormalMap(num_options, options);
			break;
		}
		case DXTImageArray::OPERATION::COPY_RECTANGLE:
		{
			DXTImageArray dxtimage_src;
			if(num_options < 1)
			{
				MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughArgumentsError", "Not enough arguments. Please supply a destination image.");
			}
			dxtimage_src.Import(num_options, options);
			DXTImageArray::CopyRectangle(dxtimage_array, dxtimage_src, num_options-1, options+1);
			break;
		}
		case DXTImageArray::OPERATION::COMPUTE_MSE:
		{
			DXTImageArray dxtimage_cmp;
			if(num_options < 1)
			{
				MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughArgumentsError", "Not enough arguments. Please supply a comparison image.");
			}
			dxtimage_cmp.Import(num_options, options);
			DXTImageArray::ComputeMSE(dxtimage_array, dxtimage_cmp, nlhs, plhs, num_options, options);
			return; // EARLY RETURN
		}
		default:
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InvalidDirectiveError", "The directive supplied does not correspond to an operation.");
			break;
		}
	}
	
	dxtimage_array.ToExport(nlhs, plhs);
	
}