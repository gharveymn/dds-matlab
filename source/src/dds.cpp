#include "mex.h"
#include "mlerrorutils.h"
#include "directxtex.h"
#include "directxtex.inl"

template <class T>
static void dds_SetScalarField(mxArray* mx_struct, mwIndex idx, const char* field, mxClassID classid, T data)
{
	mxArray* scalar = mxCreateNumericMatrix(1, 1, classid, mxREAL);
	*(T*)mxGetData(scalar) = data;
	mxSetField(mx_struct, idx, field, scalar);
}


static wchar_t* dds_ExtractFilename(const mxArray* mx_filename)
{
	mwIndex i;
	mxChar* mx_str = mxGetChars(mx_filename);
	mwSize mx_strlen = mxGetNumberOfElements(mx_filename);
	auto filename = (wchar_t*)mxMalloc((mx_strlen + 1) * sizeof(wchar_t));
	for(i = 0; i < mx_strlen; i++)
	{
		filename[i] = mx_str[i];
	}
	filename[mx_strlen] = 0;
	return filename;
}

static bool dds_CompareString(const mxArray* mx_arr, const char cmpstr[])
{
	mwIndex i;
	mxChar* mx_str = mxGetChars(mx_arr);
	mwSize mx_strlen = mxGetNumberOfElements(mx_arr);
	if(strlen(cmpstr) != mx_strlen)
	{
		return false;
	}
	for(i = 0; i < mx_strlen; i++)
	{
		if(mx_str[i] != cmpstr[i])
		{
			return false;
		}
	}
	return true;
}

static DWORD dds_ParseFlags(const mxArray* flags_array)
{
	mwIndex i;
	mxArray* curr_flag;
	DWORD flags = 0;
	mwSize num_flags = mxGetNumberOfElements(flags_array);
	for(i = 0; i < num_flags; i++)
	{
		curr_flag = mxGetCell(flags_array, i);
		if(dds_CompareString(curr_flag, "None"))
		{
			flags |= DirectX::DDS_FLAGS_NONE;
		}
		else if(dds_CompareString(curr_flag, "LegacyDWord"))
		{
			flags |= DirectX::DDS_FLAGS_LEGACY_DWORD;
		}
		else if(dds_CompareString(curr_flag, "NoLegacyExpansion"))
		{
			flags |= DirectX::DDS_FLAGS_NO_LEGACY_EXPANSION;
		}
		else if(dds_CompareString(curr_flag, "NoR10B10G10A2FixUp"))
		{
			flags |= DirectX::DDS_FLAGS_NO_R10B10G10A2_FIXUP;
		}
		else if(dds_CompareString(curr_flag, "ForceRGB"))
		{
			flags |= DirectX::DDS_FLAGS_FORCE_RGB;
		}
		else if(dds_CompareString(curr_flag, "No16BPP"))
		{
			flags |= DirectX::DDS_FLAGS_NO_16BPP;
		}
		else if(dds_CompareString(curr_flag, "ExpandLuminance"))
		{
			flags |= DirectX::DDS_FLAGS_EXPAND_LUMINANCE;
		}
		else if(dds_CompareString(curr_flag, "BadDXTNTails"))
		{
			flags |= DirectX::DDS_FLAGS_BAD_DXTN_TAILS;
		}
		else if(dds_CompareString(curr_flag, "ForceDX10Ext"))
		{
			flags |= DirectX::DDS_FLAGS_FORCE_DX10_EXT;
		}
		else if(dds_CompareString(curr_flag, "ForceDX10ExtMisc2"))
		{
			flags |= DirectX::DDS_FLAGS_FORCE_DX10_EXT_MISC2;
		}
		else
		{
			meu_PrintMexError(MEU_FL, MEU_SEVERITY_USER | MEU_SEVERITY_INTERNAL, "UnexpectedFlagError",
			"An unexpected flag was encountered during input parsing. Please use the provided entry function.");
		}
	}
	return flags;
}

#define CHECKFMT(FMT) case FMT : return mxCreateString(#FMT);
static mxArray* dds_GetFormatName(DXGI_FORMAT fmt)
{
	switch(fmt)
	{
		CHECKFMT(DXGI_FORMAT_UNKNOWN)
		CHECKFMT(DXGI_FORMAT_R32G32B32A32_TYPELESS)
		CHECKFMT(DXGI_FORMAT_R32G32B32A32_FLOAT)
		CHECKFMT(DXGI_FORMAT_R32G32B32A32_UINT)
		CHECKFMT(DXGI_FORMAT_R32G32B32A32_SINT)
		CHECKFMT(DXGI_FORMAT_R32G32B32_TYPELESS)
		CHECKFMT(DXGI_FORMAT_R32G32B32_FLOAT)
		CHECKFMT(DXGI_FORMAT_R32G32B32_UINT)
		CHECKFMT(DXGI_FORMAT_R32G32B32_SINT)
		CHECKFMT(DXGI_FORMAT_R16G16B16A16_TYPELESS)
		CHECKFMT(DXGI_FORMAT_R16G16B16A16_FLOAT)
		CHECKFMT(DXGI_FORMAT_R16G16B16A16_UNORM)
		CHECKFMT(DXGI_FORMAT_R16G16B16A16_UINT)
		CHECKFMT(DXGI_FORMAT_R16G16B16A16_SNORM)
		CHECKFMT(DXGI_FORMAT_R16G16B16A16_SINT)
		CHECKFMT(DXGI_FORMAT_R32G32_TYPELESS)
		CHECKFMT(DXGI_FORMAT_R32G32_FLOAT)
		CHECKFMT(DXGI_FORMAT_R32G32_UINT)
		CHECKFMT(DXGI_FORMAT_R32G32_SINT)
		CHECKFMT(DXGI_FORMAT_R32G8X24_TYPELESS)
		CHECKFMT(DXGI_FORMAT_D32_FLOAT_S8X24_UINT)
		CHECKFMT(DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS)
		CHECKFMT(DXGI_FORMAT_X32_TYPELESS_G8X24_UINT)
		CHECKFMT(DXGI_FORMAT_R10G10B10A2_TYPELESS)
		CHECKFMT(DXGI_FORMAT_R10G10B10A2_UNORM)
		CHECKFMT(DXGI_FORMAT_R10G10B10A2_UINT)
		CHECKFMT(DXGI_FORMAT_R11G11B10_FLOAT)
		CHECKFMT(DXGI_FORMAT_R8G8B8A8_TYPELESS)
		CHECKFMT(DXGI_FORMAT_R8G8B8A8_UNORM)
		CHECKFMT(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB)
		CHECKFMT(DXGI_FORMAT_R8G8B8A8_UINT)
		CHECKFMT(DXGI_FORMAT_R8G8B8A8_SNORM)
		CHECKFMT(DXGI_FORMAT_R8G8B8A8_SINT)
		CHECKFMT(DXGI_FORMAT_R16G16_TYPELESS)
		CHECKFMT(DXGI_FORMAT_R16G16_FLOAT)
		CHECKFMT(DXGI_FORMAT_R16G16_UNORM)
		CHECKFMT(DXGI_FORMAT_R16G16_UINT)
		CHECKFMT(DXGI_FORMAT_R16G16_SNORM)
		CHECKFMT(DXGI_FORMAT_R16G16_SINT)
		CHECKFMT(DXGI_FORMAT_R32_TYPELESS)
		CHECKFMT(DXGI_FORMAT_D32_FLOAT)
		CHECKFMT(DXGI_FORMAT_R32_FLOAT)
		CHECKFMT(DXGI_FORMAT_R32_UINT)
		CHECKFMT(DXGI_FORMAT_R32_SINT)
		CHECKFMT(DXGI_FORMAT_R24G8_TYPELESS)
		CHECKFMT(DXGI_FORMAT_D24_UNORM_S8_UINT)
		CHECKFMT(DXGI_FORMAT_R24_UNORM_X8_TYPELESS)
		CHECKFMT(DXGI_FORMAT_X24_TYPELESS_G8_UINT)
		CHECKFMT(DXGI_FORMAT_R8G8_TYPELESS)
		CHECKFMT(DXGI_FORMAT_R8G8_UNORM)
		CHECKFMT(DXGI_FORMAT_R8G8_UINT)
		CHECKFMT(DXGI_FORMAT_R8G8_SNORM)
		CHECKFMT(DXGI_FORMAT_R8G8_SINT)
		CHECKFMT(DXGI_FORMAT_R16_TYPELESS)
		CHECKFMT(DXGI_FORMAT_R16_FLOAT)
		CHECKFMT(DXGI_FORMAT_D16_UNORM)
		CHECKFMT(DXGI_FORMAT_R16_UNORM)
		CHECKFMT(DXGI_FORMAT_R16_UINT)
		CHECKFMT(DXGI_FORMAT_R16_SNORM)
		CHECKFMT(DXGI_FORMAT_R16_SINT)
		CHECKFMT(DXGI_FORMAT_R8_TYPELESS)
		CHECKFMT(DXGI_FORMAT_R8_UNORM)
		CHECKFMT(DXGI_FORMAT_R8_UINT)
		CHECKFMT(DXGI_FORMAT_R8_SNORM)
		CHECKFMT(DXGI_FORMAT_R8_SINT)
		CHECKFMT(DXGI_FORMAT_A8_UNORM)
		CHECKFMT(DXGI_FORMAT_R1_UNORM)
		CHECKFMT(DXGI_FORMAT_R9G9B9E5_SHAREDEXP)
		CHECKFMT(DXGI_FORMAT_R8G8_B8G8_UNORM)
		CHECKFMT(DXGI_FORMAT_G8R8_G8B8_UNORM)
		CHECKFMT(DXGI_FORMAT_BC1_TYPELESS)
		CHECKFMT(DXGI_FORMAT_BC1_UNORM)
		CHECKFMT(DXGI_FORMAT_BC1_UNORM_SRGB)
		CHECKFMT(DXGI_FORMAT_BC2_TYPELESS)
		CHECKFMT(DXGI_FORMAT_BC2_UNORM)
		CHECKFMT(DXGI_FORMAT_BC2_UNORM_SRGB)
		CHECKFMT(DXGI_FORMAT_BC3_TYPELESS)
		CHECKFMT(DXGI_FORMAT_BC3_UNORM)
		CHECKFMT(DXGI_FORMAT_BC3_UNORM_SRGB)
		CHECKFMT(DXGI_FORMAT_BC4_TYPELESS)
		CHECKFMT(DXGI_FORMAT_BC4_UNORM)
		CHECKFMT(DXGI_FORMAT_BC4_SNORM)
		CHECKFMT(DXGI_FORMAT_BC5_TYPELESS)
		CHECKFMT(DXGI_FORMAT_BC5_UNORM)
		CHECKFMT(DXGI_FORMAT_BC5_SNORM)
		CHECKFMT(DXGI_FORMAT_B5G6R5_UNORM)
		CHECKFMT(DXGI_FORMAT_B5G5R5A1_UNORM)
		CHECKFMT(DXGI_FORMAT_B8G8R8A8_UNORM)
		CHECKFMT(DXGI_FORMAT_B8G8R8X8_UNORM)
		CHECKFMT(DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM)
		CHECKFMT(DXGI_FORMAT_B8G8R8A8_TYPELESS)
		CHECKFMT(DXGI_FORMAT_B8G8R8A8_UNORM_SRGB)
		CHECKFMT(DXGI_FORMAT_B8G8R8X8_TYPELESS)
		CHECKFMT(DXGI_FORMAT_B8G8R8X8_UNORM_SRGB)
		CHECKFMT(DXGI_FORMAT_BC6H_TYPELESS)
		CHECKFMT(DXGI_FORMAT_BC6H_UF16)
		CHECKFMT(DXGI_FORMAT_BC6H_SF16)
		CHECKFMT(DXGI_FORMAT_BC7_TYPELESS)
		CHECKFMT(DXGI_FORMAT_BC7_UNORM)
		CHECKFMT(DXGI_FORMAT_BC7_UNORM_SRGB)
		CHECKFMT(DXGI_FORMAT_AYUV)
		CHECKFMT(DXGI_FORMAT_Y410)
		CHECKFMT(DXGI_FORMAT_Y416)
		CHECKFMT(DXGI_FORMAT_NV12)
		CHECKFMT(DXGI_FORMAT_P010)
		CHECKFMT(DXGI_FORMAT_P016)
		CHECKFMT(DXGI_FORMAT_420_OPAQUE)
		CHECKFMT(DXGI_FORMAT_YUY2)
		CHECKFMT(DXGI_FORMAT_Y210)
		CHECKFMT(DXGI_FORMAT_Y216)
		CHECKFMT(DXGI_FORMAT_NV11)
		CHECKFMT(DXGI_FORMAT_AI44)
		CHECKFMT(DXGI_FORMAT_IA44)
		CHECKFMT(DXGI_FORMAT_P8)
		CHECKFMT(DXGI_FORMAT_A8P8)
		CHECKFMT(DXGI_FORMAT_B4G4R4A4_UNORM)
		CHECKFMT(DXGI_FORMAT_P208)
		CHECKFMT(DXGI_FORMAT_V208)
		CHECKFMT(DXGI_FORMAT_V408)
		CHECKFMT(DXGI_FORMAT_FORCE_UINT)
		default: return mxCreateString("DXGI_FORMAT_UNKNOWN");
	}
}

static mxArray* dds_ExtractMetadata(DirectX::TexMetadata metadata)
{
	mxArray* tmp;
	const char* fieldnames[] = {"Width", "Height", "Depth", "ArraySize", "MipLevels", "MiscFlags", "MiscFlags2", "Format", "Dimension", "FormatID"};
	mxArray* mx_metadata = mxCreateStructMatrix(1, 1, 10, fieldnames);
	dds_SetScalarField(mx_metadata, 0, "Width", mxUINT64_CLASS, metadata.width);
	dds_SetScalarField(mx_metadata, 0, "Height", mxUINT64_CLASS, metadata.height);
	dds_SetScalarField(mx_metadata, 0, "Depth", mxUINT64_CLASS, metadata.depth);
	dds_SetScalarField(mx_metadata, 0, "ArraySize", mxUINT64_CLASS, metadata.arraySize);
	dds_SetScalarField(mx_metadata, 0, "MipLevels", mxUINT64_CLASS, metadata.mipLevels);
	dds_SetScalarField(mx_metadata, 0, "MiscFlags", mxUINT32_CLASS, metadata.miscFlags);
	dds_SetScalarField(mx_metadata, 0, "MiscFlags2", mxUINT32_CLASS, metadata.miscFlags2);
	mxSetField(mx_metadata, 0, "Format", dds_GetFormatName(metadata.format));
	dds_SetScalarField(mx_metadata, 0, "Dimension", mxUINT8_CLASS, metadata.dimension - 1);
	dds_SetScalarField(mx_metadata, 0, "FormatID", mxUINT8_CLASS, metadata.format);
	return mx_metadata;
}

static void dds_ConstructImage(const mxArray* mx_image, DirectX::Image& image)
{
	image.width      = *(uint64_T*)mxGetData(mxGetField(mx_image, 0, "Width"));
	image.height     = *(uint64_T*)mxGetData(mxGetField(mx_image, 0, "Height"));
	image.format     =  (DXGI_FORMAT)*(uint8_T*)mxGetData(mxGetField(mx_image, 0, "FormatID"));
	image.rowPitch   = *(uint64_T*)mxGetData(mxGetField(mx_image, 0, "RowPitch"));
	image.slicePitch = *(uint64_T*)mxGetData(mxGetField(mx_image, 0, "SlicePitch"));
	image.pixels     =  (uint8_t*)mxGetData(mxGetField(mx_image, 0, "pixels"));
}

static mxArray* dds_ExtractImages(const DirectX::Image* images, size_t array_size, size_t mip_levels)
{
	mwIndex i;
	mxArray* tmp;
	const char* fieldnames[] = {"Width", "Height", "Format", "RowPitch", "SlicePitch", "Pixels", "FormatID"};
	mxArray* mx_images = mxCreateStructMatrix(array_size, mip_levels, 7, fieldnames);
	mwSize num_images = array_size * mip_levels;
	for(i = 0; i < num_images; i++)
	{
		dds_SetScalarField(mx_images, i, "Width", mxUINT64_CLASS, images[i].width);
		dds_SetScalarField(mx_images, i, "Height", mxUINT64_CLASS, images[i].height);
		mxSetField(mx_images, i, "Format", dds_GetFormatName(images[i].format));
		dds_SetScalarField(mx_images, i, "RowPitch", mxUINT64_CLASS, images[i].rowPitch);
		dds_SetScalarField(mx_images, i, "SlicePitch", mxUINT64_CLASS, images[i].slicePitch);
		tmp = mxCreateNumericMatrix(images[i].rowPitch, images[i].slicePitch / images[i].rowPitch, mxUINT8_CLASS, mxREAL);
		memcpy(mxGetData(tmp), images[i].pixels, images[i].slicePitch * sizeof(uint8_t));
		mxSetField(mx_images, i, "Pixels", tmp);
		dds_SetScalarField(mx_images, 0, "FormatID", mxUINT8_CLASS, images[i].format);
	}
	return mx_images;
}

void dds_Read(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	DirectX::ScratchImage loaded_dds;
	DirectX::TexMetadata  metadata = {0};
	DWORD flags = 0;
	HRESULT hr;
	
	if(nrhs < 1)
	{
		meu_PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughInputsError", "Not enough arguments. Please supply a file name.");
	}
	else if(nrhs > 2)
	{
		meu_PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyInputsError", "Too many arguments.");
	}
	wchar_t* filename = dds_ExtractFilename(prhs[0]);
	
	if(nrhs == 2)
	{
		flags = dds_ParseFlags(prhs[1]);
	}
	
	hr = DirectX::LoadFromDDSFile(filename,flags,&metadata,loaded_dds);
	if(FAILED(hr))
	{
		meu_PrintMexError(MEU_FL, MEU_SEVERITY_USER | MEU_SEVERITY_SYSTEM, "FileReadError", "There was an error while reading the file.");
	}
	
	plhs[0] = dds_ExtractMetadata(metadata);
	if(nlhs > 1)
	{
		plhs[1] = dds_ExtractImages(loaded_dds.GetImages(), metadata.arraySize, metadata.mipLevels);
	}
	mxFree(filename);
	
}


void dds_ReadMetadata(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	DirectX::TexMetadata metadata = {0};
	DWORD flags = 0;
	HRESULT hr;
	
	if(nrhs < 1)
	{
		meu_PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughInputsError", "Not enough arguments. Please supply a file name.");
	}
	else if(nrhs > 2)
	{
		meu_PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyInputsError", "Too many arguments.");
	}
	wchar_t* filename = dds_ExtractFilename(prhs[0]);
	
	if(nrhs == 2)
	{
		flags = dds_ParseFlags(prhs[1]);
	}
	
	hr = DirectX::GetMetadataFromDDSFile(filename,flags,metadata);
	if(FAILED(hr))
	{
		meu_PrintMexError(MEU_FL, MEU_SEVERITY_USER | MEU_SEVERITY_SYSTEM, "FileReadError", "There was an error while reading the file.");
	}
	
	plhs[0] = dds_ExtractMetadata(metadata);
	mxFree(filename);
	
}

static void dds_Convert(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{

}

/* The gateway function. */
void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	
	void (*op_func)(int,mxArray*[],int,const mxArray*[]) = nullptr;
	
	meu_SetLibraryName("dds");
	
	if(nrhs < 1)
	{
		meu_PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughInputsError", "Not enough arguments. Please supply a directive.");
	}
	
	if(dds_CompareString(prhs[0], "READ"))
	{
		op_func = dds_Read;
	}
	else if(dds_CompareString(prhs[0],"READ_METADATA"))
	{
		op_func = dds_ReadMetadata;
	}
	else
	{
		meu_PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InvalidDirectiveError", "The directive supplied does not correspond to an operation.");
	}
	
	op_func(nlhs, plhs, nrhs-1, prhs+1);
	
}

