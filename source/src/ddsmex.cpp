#include "mex.h"
#include "ddsmex_mexerror.hpp"
#include "directxtex.h"
#include "directxtex.inl"
#include "ddsmex_maps.hpp"
#include "ddsmex_utils.hpp"
#include "ddsmex_mexutils.hpp"
#include "ddsmex_dds.hpp"

using namespace DDSMEX;

const char* MEXError::g_library_name = "ddsmex";

/* The gateway function. */
void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	
	DDSArray dds_array;
	
	if(nrhs < 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughInputsError", "Not enough arguments. Please supply a directive.");
	}
	
	if(!mxIsChar(prhs[0]))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InvalidDirectiveError", "The directive supplied must be of class 'char'.");
	}
	
	DDSArray::operation op = DDSArray::GetOperation(prhs[0]);
	
	int num_in              = nrhs-1;
	const mxArray** in      = prhs+1;
	
	switch(op)
	{
		case DDSArray::READ_FILE:
		{
			DDSArray::ReadFile(nlhs, plhs, num_in, in);
			return;
		}
		case DDSArray::READ_META:
		{
			DDSArray::ReadMetadata(nlhs, plhs, num_in, in);
			return;
		}
		case DDSArray::FLIP_ROTATE:
		case DDSArray::RESIZE:
		case DDSArray::CONVERT:
		case DDSArray::CONVERT_TO_SINGLE_PLANE:
		case DDSArray::GENERATE_MIPMAPS:
		case DDSArray::GENERATE_MIPMAPS_3D:
		case DDSArray::SCALE_MIPMAPS_ALPHA_FOR_COVERAGE:
		case DDSArray::PREMULTIPLY_ALPHA:
		case DDSArray::COMPRESS:
		case DDSArray::DECOMPRESS:
		case DDSArray::COMPUTE_NORMAL_MAP:
		case DDSArray::COPY_RECTANGLE:
		case DDSArray::COMPUTE_MSE:
		case DDSArray::TO_IMAGE:
		case DDSArray::TO_MATRIX:
		{
			dds_array.Import(num_in, in);
			break;
		}
		case DDSArray::NO_OP:
		default:
		{
			return;
		}
	}
	
	int num_options              = num_in-1;
	const mxArray** options      = in+1;
	
	switch(op)
	{
		case DDSArray::TO_IMAGE:
		{
			dds_array.ToImage(nlhs, plhs, num_options, options);
			return; // EARLY RETURN
		}
		case DDSArray::TO_MATRIX:
		{
			dds_array.ToMatrix(nlhs, plhs, num_options, options);
			return; // EARLY RETURN
		}
		case DDSArray::FLIP_ROTATE:
		{
			dds_array.FlipRotate(num_options, options);
			break;
		}
		case DDSArray::RESIZE:
		{
			dds_array.Resize(num_options, options);
			break;
		}
		case DDSArray::CONVERT:
		{
			dds_array.Convert(num_options, options);
			break;
		}
		case DDSArray::CONVERT_TO_SINGLE_PLANE:
		{
			dds_array.ConvertToSinglePlane(num_options, options);
			break;
		}
		case DDSArray::GENERATE_MIPMAPS:
		{
			dds_array.GenerateMipMaps(num_options, options);
			break;
		}
		case DDSArray::SCALE_MIPMAPS_ALPHA_FOR_COVERAGE:
		{
			dds_array.ScaleMipMapsAlphaForCoverage(num_options, options);
			break;
		}
		case DDSArray::PREMULTIPLY_ALPHA:
		{
			dds_array.PremultiplyAlpha(num_options, options);
			break;
		}
		case DDSArray::COMPRESS:
		{
			dds_array.Compress(num_options, options);
			break;
		}
		case DDSArray::DECOMPRESS:
		{
			dds_array.Decompress(num_options, options);
			break;
		}
		case DDSArray::COMPUTE_NORMAL_MAP:
		{
			dds_array.ComputeNormalMap(num_options, options);
			break;
		}
		case DDSArray::COPY_RECTANGLE:
		{
			DDSArray dds_dst;
			if(num_options < 1)
			{
				MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughArgumentsError", "Not enough arguments. Please supply a destination image.");
			}
			dds_dst.Import(num_options, options);
			DDSArray::CopyRectangle(dds_array, dds_dst, num_options-1, options+1);
			dds_array = dds_dst;
			break;
		}
		case DDSArray::COMPUTE_MSE:
		{
			DDSArray dds_cmp;
			if(num_options < 1)
			{
				MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughArgumentsError", "Not enough arguments. Please supply a comparison image.");
			}
			dds_cmp.Import(num_options, options);
			DDSArray::ComputeMSE(dds_array, dds_cmp, nlhs, plhs, num_options, options);
			return; // EARLY RETURN
		}
		default:
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InvalidDirectiveError", "The directive supplied does not correspond to an operation.");
			break;
		}
	}
	
	dds_array.ToExport(nlhs, plhs);
	
}