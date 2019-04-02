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
	void (DDSArray::* op_func)(int, const mxArray* []) = nullptr;
	
	if(nrhs < 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughInputsError", "Not enough arguments. Please supply a directive.");
	}
	
	if(!mxIsChar(prhs[0]))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InvalidDirectiveError", "The directive supplied must be of class 'char'.");
	}
	
	if(MEXUtils::CompareMEXString(prhs[0], "READFILE"))
	{
		DDSArray::ReadFile(nlhs, plhs, nrhs, prhs);
		return;
	}
	else if(MEXUtils::CompareMEXString(prhs[0], "READMETA"))
	{
		DDSArray::ReadMetadata(nlhs, plhs, nrhs, prhs);
		return;
	}
	
	DDSArray dds_array = DDSArray::Import(prhs[0]);
	
	int num_options         = nrhs-1;
	const mxArray** options = prhs+1;
	
	if(MEXUtils::CompareMEXString(prhs[0], "TOIMAGE"))
	{
		dds_array.ToImage(nlhs, plhs, num_options, options);
		return;
	}
	else if(MEXUtils::CompareMEXString(prhs[0], "TOMATRIX"))
	{
		dds_array.ToMatrix(nlhs, plhs, num_options, options);
		return;
	}
	
	if(MEXUtils::CompareMEXString(prhs[0], "CONVERT"))
	{
		dds_array.Convert(num_options, options);
	}
	else if(MEXUtils::CompareMEXString(prhs[0], "FLIPROTATE"))
	{
		dds_array.FlipRotate(num_options, options);
	}
	else if(MEXUtils::CompareMEXString(prhs[0], "DECOMPRESS"))
	{
		dds_array.Decompress(num_options, options);
	}
	else
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InvalidDirectiveError", "The directive supplied does not correspond to an operation.");
	}
	
	dds_array.ToExport(nlhs, plhs);
	
}