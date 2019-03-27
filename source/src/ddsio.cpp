#include "mex.h"
#include "mlerrorutils.h"
#include "directxtex.h"
#include "directxtex.inl"
#include "ddsio.h"

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
	
	if(!mxIsCell(flags_array))
	{
		meu_PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InputError", "Flags must be contained by a cell array.");
	}
	mwSize num_flags = mxGetNumberOfElements(flags_array);
	for(i = 0; i < num_flags; i++)
	{
		curr_flag = mxGetCell(flags_array, i);
		if(!mxIsChar(curr_flag))
		{
			meu_PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InputError", "Flags must be of type 'char'.");
		}
		char* flagname = mxArrayToString(curr_flag);
		auto found = ctrlflag_map.find(flagname);
		if(found != ctrlflag_map.s_end())
		{
			flags |= found->second;
		}
		else
		{
			mxFree(flagname);
			meu_PrintMexError(MEU_FL, MEU_SEVERITY_USER, "UnexpectedFlagError",
			                  "An unexpected flag was encountered during input parsing.");
		}
		mxFree(flagname);
	}
	return flags;
}



static mxArray* dds_ExtractMetadata(DirectX::TexMetadata metadata)
{
	const char* fieldnames[] = {"Width", "Height", "Depth", "ArraySize", "MipLevels", "MiscFlags", "MiscFlags2", "Format", "Dimension", "FormatID"};
	mxArray* mx_metadata = mxCreateStructMatrix(1, 1, 10, fieldnames);
	dds_SetScalarField(mx_metadata, 0, "Width", mxUINT64_CLASS, metadata.width);
	dds_SetScalarField(mx_metadata, 0, "Height", mxUINT64_CLASS, metadata.height);
	dds_SetScalarField(mx_metadata, 0, "Depth", mxUINT64_CLASS, metadata.depth);
	dds_SetScalarField(mx_metadata, 0, "ArraySize", mxUINT64_CLASS, metadata.arraySize);
	dds_SetScalarField(mx_metadata, 0, "MipLevels", mxUINT64_CLASS, metadata.mipLevels);
	dds_SetScalarField(mx_metadata, 0, "MiscFlags", mxUINT32_CLASS, metadata.miscFlags);
	dds_SetScalarField(mx_metadata, 0, "MiscFlags2", mxUINT32_CLASS, metadata.miscFlags2);
	mxSetField(mx_metadata, 0, "Format", mxCreateString(format_map[metadata.format].c_str()));
	dds_SetScalarField(mx_metadata, 0, "Dimension", mxUINT8_CLASS, metadata.dimension - 1);
	dds_SetScalarField(mx_metadata, 0, "FormatID", mxUINT8_CLASS, metadata.format);
	return mx_metadata;
}

static void dds_ConstructImage(const mxArray* mx_image, DirectX::Image& image)
{
	image.width      = *(uint64_T*)mxGetData(mxGetCell(mx_image, 0));
	image.height     = *(uint64_T*)mxGetData(mxGetCell(mx_image, 1));
	image.format     =  (DXGI_FORMAT)*(uint8_T*)mxGetData(mxGetCell(mx_image, 2));
	image.rowPitch   = *(uint64_T*)mxGetData(mxGetCell(mx_image, 3));
	image.slicePitch = *(uint64_T*)mxGetData(mxGetCell(mx_image, 4));
	image.pixels     =  (uint8_t*)mxGetData(mxGetCell(mx_image, 5));
}

/*
static mxArray* dds_RowToColumn(uint8_t* src, size_t width, size_t height, size_t pixel_pitch)
{
	size_t src_idx, dst_idx;
	size_t row_pitch = width;
	size_t col_pitch = height * pixel_pitch;
	size_t num_pixels = width * height;
	mxArray* ret = mxCreateNumericMatrix(col_pitch, row_pitch, mxUINT8_CLASS, mxREAL);
	auto dst = (uint8_t*)mxGetData(ret);
	for(src_idx = 0; src_idx < num_pixels; src_idx++)
	{
		dst_idx = (src_idx % width) * height + src_idx / width;
		memcpy(dst + dst_idx*pixel_pitch, src + src_idx*pixel_pitch, pixel_pitch);
	}
	return ret;
}

static mxArray* dds_RowToColumn2(uint8_t* src, size_t row_pitch, size_t slice_pitch, size_t bpp)
{
	size_t src_idx, dst_idx;
	size_t new_row_pitch = row_pitch / pixel_pitch;
	size_t new_col_pitch = (slice_pitch / row_pitch) * pixel_pitch;
	mxArray* ret = mxCreateNumericMatrix(new_col_pitch, new_row_pitch, mxUINT8_CLASS, mxREAL);
	auto dst = (uint8_t*)mxGetData(ret);
	for(src_idx = 0; src_idx < slice_pitch; src_idx += pixel_pitch)
	{
		dst_idx = (src_idx % row_pitch) * new_col_pitch + src_idx / row_pitch;
		memcpy(dst + dst_idx, src + src_idx, pixel_pitch);
	}
	return ret;
}
*/

static mxArray* dds_ExtractImages(const DirectX::Image* images, size_t array_size, size_t mip_levels)
{
	mxArray* tmp;
	mwIndex i;
	const char* fieldnames[] = {"Width", "Height", "Format", "RowPitch", "SlicePitch", "Pixels", "FormatID"};
	mxArray* mx_images = mxCreateStructMatrix(array_size, mip_levels, 7, fieldnames);
	mwSize num_images = array_size * mip_levels;
	for(i = 0; i < num_images; i++)
	{
		dds_SetScalarField(mx_images, i, "Width", mxUINT64_CLASS, images[i].width);
		dds_SetScalarField(mx_images, i, "Height", mxUINT64_CLASS, images[i].height);
		mxSetField(mx_images, i, "Format", mxCreateString(format_map[images[i].format].c_str()));
		dds_SetScalarField(mx_images, i, "RowPitch", mxUINT64_CLASS, images[i].rowPitch);
		dds_SetScalarField(mx_images, i, "SlicePitch", mxUINT64_CLASS, images[i].slicePitch);
		// mxSetField(mx_images, i, "Pixels", dds_RowToColumn(images[i].pixels, images[i].width, images[i].height, images[i].rowPitch / images[i].width));
		// mxSetField(mx_images, i, "Pixels", dds_RowToColumn2(images[i].pixels, images[i].rowPitch, images[i].slicePitch, images[i].rowPitch / images[i].width));
		tmp = mxCreateNumericMatrix(images[i].slicePitch, 1, mxUINT8_CLASS, mxREAL);
		memcpy(mxGetData(tmp), images[i].pixels, images[i].slicePitch * sizeof(uint8_T));
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

static DWORD dds_ParseFilter(const mxArray* filter_flag_array)
{
	mwIndex i;
	mxArray* curr_flag;
	DWORD filter_flags = 0;
	
	if(!mxIsCell(filter_flag_array))
	{
		meu_PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InputError", "Flags must be contained by a cell array.");
	}
	mwSize num_flags = mxGetNumberOfElements(filter_flag_array);
	for(i = 0; i < num_flags; i++)
	{
		curr_flag = mxGetCell(filter_flag_array, i);
		if(!mxIsChar(curr_flag))
		{
			meu_PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InputError", "Flags must be of class 'char'.");
		}
		char* flagname = mxArrayToString(curr_flag);
		auto found = ctrlflag_map.find(flagname);
		if(found != ctrlflag_map.s_end())
		{
			filter_flags |= found->second;
		}
		else
		{
			mxFree(flagname);
			meu_PrintMexError(MEU_FL, MEU_SEVERITY_USER, "UnexpectedFlagError",
			                  "An unexpected flag was encountered during input parsing.");
		}
		mxFree(flagname);
	}
	return filter_flags;
}

static DXGI_FORMAT dds_ParseFormat(const mxArray* mx_fmt)
{
	DXGI_FORMAT fmt;
	if(!mxIsChar(mx_fmt))
	{
		meu_PrintMexError(MEU_FL, MEU_SEVERITY_USER|MEU_SEVERITY_INTERNAL, "FormatParsingError", "Format input must be of class 'char'.");
	}
	char* fmtname = mxArrayToString(mx_fmt);
	auto found = format_map.find(fmtname);
	if(found != format_map.s_end())
	{
		fmt = (DXGI_FORMAT)found->second;
	}
	else
	{
		mxFree(fmtname);
		meu_PrintMexError(MEU_FL, MEU_SEVERITY_USER, "UnexpectedFlagError",
		                  "An unexpected flag was encountered during input parsing.");
	}
	mxFree(fmtname);
	return fmt;
}

static void dds_Convert(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	HRESULT hr;
	DirectX::Image src_img = {};
	DirectX::ScratchImage conv;
	DWORD filter = DirectX::TEX_FILTER_DEFAULT;
	float threshold = DirectX::TEX_THRESHOLD_DEFAULT;
	if(nrhs < 2)
	{
		meu_PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughInputsError", "Not enough arguments. Please supply a file name.");
	}
	else if(nrhs > 4)
	{
		meu_PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyInputsError", "Too many arguments.");
	}
	
	dds_ConstructImage(prhs[0], src_img);
	DXGI_FORMAT fmt = dds_ParseFormat(prhs[1]);
	
	if(nrhs > 2)
	{
		filter = dds_ParseFilter(prhs[2]);
	}
	
	if(nrhs > 3)
	{
		threshold = (float)mxGetScalar(prhs[3]);
	}
	
	hr = DirectX::Convert(src_img, fmt, filter, threshold, conv);
	if(FAILED(hr))
	{
		meu_PrintMexError(MEU_FL, MEU_SEVERITY_USER|MEU_SEVERITY_SYSTEM, "ConversionError", "There was an error converting the image.");
	}
	DirectX::TexMetadata conv_meta = conv.GetMetadata();
	plhs[0] = dds_ExtractImages(conv.GetImages(), conv_meta.arraySize, conv_meta.mipLevels);
	
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
	
	if(!mxIsChar(prhs[0]))
	{
		meu_PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InvalidDirectiveError", "The directive supplied must be of class 'char'.");
	}
	
	if(dds_CompareString(prhs[0], "READ"))
	{
		op_func = dds_Read;
	}
	else if(dds_CompareString(prhs[0],"READ_METADATA"))
	{
		op_func = dds_ReadMetadata;
	}
	else if(dds_CompareString(prhs[0],"CONVERT"))
	{
		op_func = dds_Convert;
	}
	else
	{
		meu_PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InvalidDirectiveError", "The directive supplied does not correspond to an operation.");
	}
	
	op_func(nlhs, plhs, nrhs-1, prhs+1);
	
}

