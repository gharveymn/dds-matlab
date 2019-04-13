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
	
	const DXTImageArray::operation op = DXTImageArray::GetOperation(prhs[0]);
	
	const int num_in              = nrhs-1;
	const mxArray** in      = prhs+1;
	
	switch(op)
	{
		case DXTImageArray::READ_DDS:
		{
			DXTImageArray::ReadDDS(nlhs, plhs, num_in, in);
			return; // EARLY RETURN
		}
		case DXTImageArray::READ_HDR:
		{
			DXTImageArray::ReadHDR(nlhs, plhs, num_in, in);
			return; // EARLY RETURN
		}
		case DXTImageArray::READ_TGA:
		{
			DXTImageArray::ReadTGA(nlhs, plhs, num_in, in);
			return; // EARLY RETURN
		}
		case DXTImageArray::READ_DDS_META:
		{
			DXTImageArray::ReadDDSMetadata(nlhs, plhs, num_in, in);
			return; // EARLY RETURN
		}
		case DXTImageArray::READ_HDR_META:
		{
			DXTImageArray::ReadHDRMetadata(nlhs, plhs, num_in, in);
			return; // EARLY RETURN
		}
		case DXTImageArray::READ_TGA_META:
		{
			DXTImageArray::ReadTGAMetadata(nlhs, plhs, num_in, in);
			return; // EARLY RETURN
		}
		case DXTImageArray::IS_DDS:
		{
			DXTImageArray::IsDDS(nlhs, plhs, num_in, in);
			return; // EARLY RETURN
		}
		case DXTImageArray::IS_HDR:
		{
			DXTImageArray::IsHDR(nlhs, plhs, num_in, in);
			return; // EARLY RETURN
		}
		case DXTImageArray::IS_TGA:
		{
			DXTImageArray::IsTGA(nlhs, plhs, num_in, in);
			return; // EARLY RETURN
		}
		case DXTImageArray::WRITE_DDS:
		case DXTImageArray::WRITE_HDR:
		case DXTImageArray::WRITE_TGA:
		case DXTImageArray::FLIP_ROTATE:
		case DXTImageArray::RESIZE:
		case DXTImageArray::CONVERT:
		case DXTImageArray::CONVERT_TO_SINGLE_PLANE:
		case DXTImageArray::GENERATE_MIPMAPS:
		case DXTImageArray::GENERATE_MIPMAPS_3D:
		case DXTImageArray::SCALE_MIPMAPS_ALPHA_FOR_COVERAGE:
		case DXTImageArray::PREMULTIPLY_ALPHA:
		case DXTImageArray::COMPRESS:
		case DXTImageArray::DECOMPRESS:
		case DXTImageArray::COMPUTE_NORMAL_MAP:
		case DXTImageArray::COPY_RECTANGLE:
		case DXTImageArray::COMPUTE_MSE:
		case DXTImageArray::TO_IMAGE:
		case DXTImageArray::TO_MATRIX:
		{
			dxtimage_array.Import(num_in, in);
			break;
		}
		case DXTImageArray::NO_OP:
		default:
		{
			return;
		}
	}
	
	if(dxtimage_array.GetSize() < 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_INTERNAL, "NoImageSuppliedError", "No images were supplied. Cannot continue.");
	}
	
	const int num_options         = num_in-1;
	const mxArray** options = in+1;
	
	switch(op)
	{
		case DXTImageArray::WRITE_DDS:
		{
			dxtimage_array.WriteDDS(num_options, options);
			return; // EARLY RETURN
		}
		case DXTImageArray::WRITE_HDR:
		{
			dxtimage_array.WriteHDR(num_options, options);
			return; // EARLY RETURN
		}
		case DXTImageArray::WRITE_TGA:
		{
			dxtimage_array.WriteTGA(num_options, options);
			return; // EARLY RETURN
		}
		case DXTImageArray::TO_IMAGE:
		{
			dxtimage_array.ToImage(nlhs, plhs, num_options, options);
			return; // EARLY RETURN
		}
		case DXTImageArray::TO_MATRIX:
		{
			dxtimage_array.ToMatrix(nlhs, plhs, num_options, options);
			return; // EARLY RETURN
		}
		case DXTImageArray::FLIP_ROTATE:
		{
			dxtimage_array.FlipRotate(num_options, options);
			break;
		}
		case DXTImageArray::RESIZE:
		{
			dxtimage_array.Resize(num_options, options);
			break;
		}
		case DXTImageArray::CONVERT:
		{
			dxtimage_array.Convert(num_options, options);
			break;
		}
		case DXTImageArray::CONVERT_TO_SINGLE_PLANE:
		{
			dxtimage_array.ConvertToSinglePlane(num_options, options);
			break;
		}
		case DXTImageArray::GENERATE_MIPMAPS:
		{
			dxtimage_array.GenerateMipMaps(num_options, options);
			break;
		}
		case DXTImageArray::SCALE_MIPMAPS_ALPHA_FOR_COVERAGE:
		{
			dxtimage_array.ScaleMipMapsAlphaForCoverage(num_options, options);
			break;
		}
		case DXTImageArray::PREMULTIPLY_ALPHA:
		{
			dxtimage_array.PremultiplyAlpha(num_options, options);
			break;
		}
		case DXTImageArray::COMPRESS:
		{
			dxtimage_array.Compress(num_options, options);
			break;
		}
		case DXTImageArray::DECOMPRESS:
		{
			dxtimage_array.Decompress(num_options, options);
			break;
		}
		case DXTImageArray::COMPUTE_NORMAL_MAP:
		{
			dxtimage_array.ComputeNormalMap(num_options, options);
			break;
		}
		case DXTImageArray::COPY_RECTANGLE:
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
		case DXTImageArray::COMPUTE_MSE:
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