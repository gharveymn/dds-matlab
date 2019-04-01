#include "mex.h"
#include "ddsmex_mexerror.hpp"
#include "directxtex.h"
#include "directxtex.inl"
#include "ddsmex.hpp"
#include "ddsmex_utils.hpp"
#include "ddsmex_mexutils.hpp"

using namespace DDSMex;

const char* MEXError::g_library_name = "ddsmex";


void Read(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	wchar_t* filename;
	size_t i;
	DirectX::ScratchImage out;
	DirectX::TexMetadata metadata = {};
	DWORD flags = DirectX::DDS_FLAGS_NONE;
	mxArray* ddsret;
	
	if(nrhs < 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughInputsError", "Not enough arguments. Please supply a file name.");
	}
	else if(nrhs > 2)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyInputsError", "Too many arguments.");
	}
	const mxArray* mx_filenames = prhs[0];
	
	if(nrhs == 2)
	{
		ImportFlags(prhs[1], ctrlflag_map, flags);
	}
	
	if(mxIsCell(mx_filenames))
	{
		ddsret = mxCreateStructMatrix(mxGetM(mx_filenames), mxGetN(mx_filenames), 2, ddsret_fieldnames);
		size_t num_fns = mxGetNumberOfElements(mx_filenames);
		for(i = 0; i < num_fns; i++)
		{
			filename = ImportFilename(mxGetCell(mx_filenames, i));
			hres = DirectX::LoadFromDDSFile(filename, flags, &metadata, out);
			if(FAILED(hres))
			{
				MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER | MEU_SEVERITY_HRESULT, "FileReadError", "There was an error while reading the file.");
			}
			mxSetField(ddsret, i, "Metadata", ExportMetadata(metadata));
			mxSetField(ddsret, i, "Images", ExportImages(out.GetImages(), metadata.arraySize, metadata.mipLevels, metadata.depth, metadata.dimension));
			mxFree(filename);
		}
	}
	else
	{
		ddsret = mxCreateStructMatrix(1, 1, 2, ddsret_fieldnames);
		filename = ImportFilename(mx_filenames);
		hres = DirectX::LoadFromDDSFile(filename, flags, &metadata, out);
		if(FAILED(hres))
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER | MEU_SEVERITY_HRESULT, "FileReadError", "There was an error while reading the file.");
		}
		mxSetField(ddsret, 0, "Metadata", ExportMetadata(metadata));
		mxSetField(ddsret, 0, "Images", ExportImages(out.GetImages(), metadata.arraySize, metadata.mipLevels, metadata.depth, metadata.dimension));
		mxFree(filename);
	}
	plhs[0] = ddsret;
}


void ReadMetadata(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	DirectX::TexMetadata metadata = {};
	DWORD flags = DirectX::DDS_FLAGS_NONE;
	
	if(nrhs < 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughInputsError", "Not enough arguments. Please supply a file name.");
	}
	else if(nrhs > 2)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyInputsError", "Too many arguments.");
	}
	wchar_t* filename = ImportFilename(prhs[0]);
	
	if(nrhs == 2)
	{
		ImportFlags(prhs[1], ctrlflag_map, flags);
	}
	
	hres = DirectX::GetMetadataFromDDSFile(filename, flags, metadata);
	if(FAILED(hres))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER | MEU_SEVERITY_HRESULT, "FileReadError", "There was an error while reading the file.");
	}
	
	plhs[0] = ExportMetadata(metadata);
	mxFree(filename);
	
}


static DXGI_FORMAT ParseFormat(const mxArray* mx_fmt)
{
	DXGI_FORMAT fmt = DXGI_FORMAT_UNKNOWN;
	if(!mxIsChar(mx_fmt))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER | MEU_SEVERITY_INTERNAL, "FormatParsingError", "Format input must be of class 'char'.");
	}
	MEXUtils::ToUpper((mxArray*)mx_fmt);
	char* fmtname = mxArrayToString(mx_fmt);
	auto found = format_map.find(fmtname);
	if(found != format_map.s_end())
	{
		fmt = (DXGI_FORMAT)found->second;
	}
	else
	{
		mxFree(fmtname);
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "UnexpectedFlagError", "An unexpected flag was encountered during input parsing.");
	}
	mxFree(fmtname);
	return fmt;
}


static void ConvertImage(int nlhs, mxArray** plhs, int nrhs, const mxArray** prhs)
{
	size_t i, num_imgs;
	DirectX::Image src_img = {};
	DirectX::ScratchImage out;
	DWORD filter = DirectX::TEX_FILTER_DEFAULT;
	float threshold = DirectX::TEX_THRESHOLD_DEFAULT;
	if(nrhs < 2)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughInputsError", "Not enough arguments. Please supply a DdsSlice and a format string");
	}
	else if(nrhs > 4)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyInputsError", "Too many arguments.");
	}
	
	const mxArray* mx_img = prhs[0];
	
	DXGI_FORMAT fmt = ParseFormat(prhs[1]);
	
	if(nrhs > 2)
	{
		ImportFlags(prhs[2], filterflag_map, filter);
	}
	
	if(nrhs > 3)
	{
		threshold = (float)mxGetScalar(prhs[3]);
	}
	
	mxArray* ddsret = mxCreateStructMatrix(mxGetM(mx_img), mxGetN(mx_img), 2, ddsret_fieldnames);
	num_imgs = mxGetNumberOfElements(mx_img);
	for(i = 0; i < num_imgs; i++)
	{
		ImportImage(mx_img, i, src_img);
		hres = DirectX::Convert(src_img, fmt, filter, threshold, out);
		if(FAILED(hres))
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "ConversionError", "There was an error converting the image.");
		}
		DirectX::TexMetadata out_meta = out.GetMetadata();
		mxSetField(ddsret, i, "Metadata", ExportMetadata(out_meta));
		mxSetField(ddsret, i, "Images", ExportImages(out.GetImages(), out_meta.arraySize, out_meta.mipLevels, out_meta.depth, out_meta.dimension));
	}
	plhs[0] = ddsret;
}

static void ConvertDDS(int nlhs, mxArray** plhs, int nrhs, const mxArray** prhs)
{
	size_t i, j, num_dds, num_imgs;
	DirectX::TexMetadata src_metadata = {};
	DirectX::Image* src_imgs;
	DirectX::ScratchImage out;
	DWORD filter = DirectX::TEX_FILTER_DEFAULT;
	float threshold = DirectX::TEX_THRESHOLD_DEFAULT;
	if(nrhs < 2)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughInputsError", "Not enough arguments. Please supply a Dds and a format string.");
	}
	else if(nrhs > 4)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyInputsError", "Too many arguments.");
	}
	const mxArray* mx_dds = prhs[0];
	
	DXGI_FORMAT fmt = ParseFormat(prhs[1]);
	
	if(nrhs > 2)
	{
		ImportFlags(prhs[2], filterflag_map, filter);
	}
	
	if(nrhs > 3)
	{
		threshold = (float)mxGetScalar(prhs[3]);
	}
	
	mxArray* ddsret = mxCreateStructMatrix(mxGetM(mx_dds), mxGetN(mx_dds), 2, ddsret_fieldnames);
	num_dds = mxGetNumberOfElements(mx_dds);
	for(i = 0; i < num_dds; i++)
	{
		mxArray* mx_metadata = mxGetField(mx_dds, i, "Metadata");
		mxArray* mx_images   = mxGetField(mx_dds, i, "Images");
		ImportMetadata(mx_metadata, src_metadata);
		num_imgs = mxGetNumberOfElements(mx_images);
		src_imgs = (DirectX::Image*)mxMalloc(num_imgs * sizeof(DirectX::Image));
		for(j = 0; j < num_imgs; j++)
		{
			ImportImage(mx_images, j, *(src_imgs + j));
		}
		
		hres = DirectX::Convert(src_imgs, num_imgs, src_metadata, fmt, filter, threshold, out);
		if(FAILED(hres))
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "ConversionError", "There was an error converting the image.");
		}
		DirectX::TexMetadata out_meta = out.GetMetadata();
		mxSetField(ddsret, i, "Metadata", ExportMetadata(out_meta));
		mxSetField(ddsret, i, "Images", ExportImages(out.GetImages(), out_meta.arraySize, out_meta.mipLevels, out_meta.depth, out_meta.dimension));
		mxFree(src_imgs);
	}
	plhs[0] = ddsret;
	
}


static void FlipRotateImage(int nlhs, mxArray** plhs, int nrhs, const mxArray** prhs)
{
	size_t i, num_imgs;
	DirectX::Image src_img = {};
	DirectX::ScratchImage out;
	DWORD flags = DirectX::TEX_FR_ROTATE0;
	if(nrhs < 2)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughInputsError", "Not enough arguments. Please supply a DdsSlice and flags.");
	}
	else if(nrhs > 2)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyInputsError", "Too many arguments.");
	}
	
	const mxArray* mx_img = prhs[0];
	
	ImportFlags(prhs[1], fr_map, flags);
	
	mxArray* ddsret = mxCreateStructMatrix(mxGetM(mx_img), mxGetN(mx_img), 2, ddsret_fieldnames);
	num_imgs = mxGetNumberOfElements(mx_img);
	for(i = 0; i < num_imgs; i++)
	{
		ImportImage(mx_img, i, src_img);
		hres = DirectX::FlipRotate(src_img, flags, out);
		if(FAILED(hres))
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "FlipRotateError", "There was an error while rotating or flipping the image.");
		}
		DirectX::TexMetadata out_meta = out.GetMetadata();
		mxSetField(ddsret, i, "Metadata", ExportMetadata(out_meta));
		mxSetField(ddsret, i, "Images", ExportImages(out.GetImages(), out_meta.arraySize, out_meta.mipLevels, out_meta.depth, out_meta.dimension));
	}
	plhs[0] = ddsret;
}

static void FlipRotateDDS(int nlhs, mxArray** plhs, int nrhs, const mxArray** prhs)
{
	size_t i, j, num_dds, num_imgs;
	DirectX::TexMetadata src_metadata = {};
	DirectX::Image* src_imgs;
	DirectX::ScratchImage out;
	DWORD flags = DirectX::TEX_FR_ROTATE0;
	if(nrhs < 2)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughInputsError", "Not enough arguments. Please supply a DdsSlice and flags.");
	}
	else if(nrhs > 2)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyInputsError", "Too many arguments.");
	}
	const mxArray* mx_dds = prhs[0];
	
	ImportFlags(prhs[1], fr_map, flags);
	
	mxArray* ddsret = mxCreateStructMatrix(mxGetM(mx_dds), mxGetN(mx_dds), 2, ddsret_fieldnames);
	num_dds = mxGetNumberOfElements(mx_dds);
	for(i = 0; i < num_dds; i++)
	{
		mxArray* mx_metadata = mxGetField(mx_dds, i, "Metadata");
		mxArray* mx_images   = mxGetField(mx_dds, i, "Images");
		ImportMetadata(mx_metadata, src_metadata);
		num_imgs = mxGetNumberOfElements(mx_images);
		src_imgs = (DirectX::Image*)mxMalloc(num_imgs * sizeof(DirectX::Image));
		for(j = 0; j < num_imgs; j++)
		{
			ImportImage(mx_images, j, *(src_imgs + j));
		}
		
		hres = DirectX::FlipRotate(src_imgs, num_imgs, src_metadata, flags, out);
		if(FAILED(hres))
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "FlipRotateError", "There was an error while rotating or flipping the image.");
		}
		DirectX::TexMetadata out_meta = out.GetMetadata();
		mxSetField(ddsret, i, "Metadata", ExportMetadata(out_meta));
		mxSetField(ddsret, i, "Images", ExportImages(out.GetImages(), out_meta.arraySize, out_meta.mipLevels, out_meta.depth, out_meta.dimension));
		mxFree(src_imgs);
	}
	plhs[0] = ddsret;
	
}

static void DecompressImage(int nlhs, mxArray** plhs, int nrhs, const mxArray** prhs)
{
	size_t i, num_imgs;
	DirectX::Image src_img = {};
	DirectX::ScratchImage out;
	DXGI_FORMAT fmt = DXGI_FORMAT_UNKNOWN;
	if(nrhs < 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughInputsError", "Not enough arguments. Please supply a DdsSlice.");
	}
	else if(nrhs > 2)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyInputsError", "Too many arguments.");
	}
	
	const mxArray* mx_img = prhs[0];
	if(nrhs == 2)
	{
		fmt = ParseFormat(prhs[1]);
	}
	
	mxArray* ddsret = mxCreateStructMatrix(mxGetM(mx_img), mxGetN(mx_img), 2, ddsret_fieldnames);
	num_imgs = mxGetNumberOfElements(mx_img);
	for(i = 0; i < num_imgs; i++)
	{
		ImportImage(mx_img, i, src_img);
		hres = DirectX::Decompress(src_img, fmt, out);
		if(FAILED(hres))
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "DecompressError", "There was an error while decompressing the image.");
		}
		DirectX::TexMetadata out_meta = out.GetMetadata();
		mxSetField(ddsret, i, "Metadata", ExportMetadata(out_meta));
		mxSetField(ddsret, i, "Images", ExportImages(out.GetImages(), out_meta.arraySize, out_meta.mipLevels, out_meta.depth, out_meta.dimension));
	}
	plhs[0] = ddsret;
}

static void DecompressDDS(int nlhs, mxArray** plhs, int nrhs, const mxArray** prhs)
{
	size_t i, j, num_dds, num_imgs;
	DirectX::TexMetadata src_metadata = {};
	DirectX::Image* src_imgs;
	DDSMex::DDS out;
	DXGI_FORMAT fmt = DXGI_FORMAT_UNKNOWN;
	if(nrhs < 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughInputsError", "Not enough arguments. Please supply a DdsSlice.");
	}
	else if(nrhs > 2)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyInputsError", "Too many arguments.");
	}
	
	const mxArray* mx_dds = prhs[0];
	if(nrhs == 2)
	{
		fmt = ParseFormat(prhs[1]);
	}
	
	mxArray* ddsret = mxCreateStructMatrix(mxGetM(mx_dds), mxGetN(mx_dds), 2, ddsret_fieldnames);
	num_dds = mxGetNumberOfElements(mx_dds);
	for(i = 0; i < num_dds; i++)
	{
		mxArray* mx_metadata = mxGetField(mx_dds, i, "Metadata");
		mxArray* mx_images   = mxGetField(mx_dds, i, "Images");
		ImportMetadata(mx_metadata, src_metadata);
		num_imgs = mxGetNumberOfElements(mx_images);
		src_imgs = (DirectX::Image*)mxMalloc(num_imgs * sizeof(DirectX::Image));
		for(j = 0; j < num_imgs; j++)
		{
			ImportImage(mx_images, j, *(src_imgs + j));
		}
		
		hres = DirectX::Decompress(src_imgs, num_imgs, src_metadata, fmt, out);
		if(FAILED(hres))
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "FlipRotateError", "There was an error while rotating or flipping the image.");
		}
		DirectX::TexMetadata out_meta = out.GetMetadata();
		mxSetField(ddsret, i, "Metadata", ExportMetadata(out_meta));
		mxSetField(ddsret, i, "Images", ExportImages(out.GetImages(), out_meta.arraySize, out_meta.mipLevels, out_meta.depth, out_meta.dimension));
		mxFree(src_imgs);
	}
	plhs[0] = ddsret;
}

static void FormMatrix(DirectX::Image raw_img, mxArray*& mx_rgb, mxArray*& mx_a)
{
	size_t i, j, src_idx, dst_idx;
	if(!DirectX::IsValid(raw_img.format))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InvalidFormatError", "Cannot create matrix from image with invalid format.");
	}
	if(DirectX::IsCompressed(raw_img.format))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "CompressedFormatError", "Cannot create matrix from compressed image.");
	}
	// assume 8-bit unsigned normalized sRGB for now
	mwSize rgb_sz[3] = {raw_img.height, raw_img.width, 3};
	mx_rgb = mxCreateNumericArray(3, rgb_sz, mxUINT8_CLASS, mxREAL);
	if(DirectX::HasAlpha(raw_img.format))
	{
		mx_a = mxCreateNumericMatrix(raw_img.height, raw_img.width, mxUINT8_CLASS, mxREAL);
		auto r_data = (uint8_t*)mxGetData(mx_rgb);
		auto g_data = (uint8_t*)mxGetData(mx_rgb) + raw_img.height * raw_img.width;
		auto b_data = (uint8_t*)mxGetData(mx_rgb) + 2 * raw_img.height * raw_img.width;
		auto a_data = (uint8_t*)mxGetData(mx_a);
		for(i = 0; i < raw_img.height; i++)
		{
			for(j = 0; j < raw_img.width; j++)
			{
				dst_idx = i + j*raw_img.height;
				src_idx = (i*raw_img.width + j) * 4;
				r_data[dst_idx] = raw_img.pixels[src_idx];
				g_data[dst_idx] = raw_img.pixels[src_idx + 1];
				b_data[dst_idx] = raw_img.pixels[src_idx + 2];
				a_data[dst_idx] = raw_img.pixels[src_idx + 3];
			}
		}
	}
	else
	{
		mx_a = mxCreateNumericMatrix(0, 0, mxUINT8_CLASS, mxREAL);
		auto r_data = (uint8_t*)mxGetData(mx_rgb);
		auto g_data = (uint8_t*)mxGetData(mx_rgb) + raw_img.height * raw_img.width;
		auto b_data = (uint8_t*)mxGetData(mx_rgb) + 2 * raw_img.height * raw_img.width;
		for(i = 0; i < raw_img.height; i++)
		{
			for(j = 0; j < raw_img.width; j++)
			{
				dst_idx = i + j*raw_img.height;
				src_idx = (i*raw_img.width + j) * 4;
				r_data[dst_idx] = raw_img.pixels[src_idx];
				g_data[dst_idx] = raw_img.pixels[src_idx + 1];
				b_data[dst_idx] = raw_img.pixels[src_idx + 2];
			}
		}
	}
}

static void ToImageMatrix(int nlhs, mxArray** plhs, int nrhs, const mxArray** prhs)
{
	mxArray* mx_out_rgb;
	mxArray* mx_out_a;
	DXGI_FORMAT srgb_fmt = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	DDSMex::DDS out;
	DirectX::Image src_img = {};
	size_t i, num_imgs;
	if(nrhs < 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughInputsError", "Not enough arguments. Please supply a DdsSlice.");
	}
	else if(nrhs > 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyInputsError", "Too many arguments.");
	}
	const mxArray* mx_img = prhs[0];
	num_imgs = mxGetNumberOfElements(mx_img);
	if(num_imgs < 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughInputsError", "Not enough arguments. Please supply a DdsSlice.");
	}
	else if(num_imgs == 1)
	{
		ImportImage(mx_img, 0, src_img);
		if(!DirectX::HasAlpha(src_img.format))
		{
			srgb_fmt = DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;
		}
		if(DirectX::IsCompressed(src_img.format))
		{
			// decompress image so we can convert it
			DirectX::ScratchImage tmp;
			hres = DirectX::Decompress(src_img, DXGI_FORMAT_UNKNOWN, tmp);
			if(FAILED(hres))
			{
				MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "DecompressError", "There was an error while decompressing the image.");
			}
			// const DirectX::Image src_img_decomp = tmp.GetImage(0,0,0);
			// src_img = *tmp.GetImage(0, 0, 0);
			// convert image to unsigned normalized 8-bit sRGB with alpha
			hres = DirectX::Convert(*tmp.GetImage(0, 0, 0), srgb_fmt, DirectX::TEX_FILTER_SRGB_OUT, DirectX::TEX_THRESHOLD_DEFAULT, out);
			if(FAILED(hres))
			{
				MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "ConversionError", "There was an error converting the image to sRGB.");
			}
			FormMatrix(*out.GetImage(0, 0, 0), mx_out_rgb, mx_out_a);
		}
		else
		{
			// convert image to unsigned normalized 8-bit sRGB with alpha
			hres = DirectX::Convert(src_img, srgb_fmt, DirectX::TEX_FILTER_SRGB_OUT, DirectX::TEX_THRESHOLD_DEFAULT, out);
			if(FAILED(hres))
			{
				MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "ConversionError", "There was an error converting the image to sRGB.");
			}
			FormMatrix(*out.GetImage(0, 0, 0), mx_out_rgb, mx_out_a);
		}
	}
	else
	{
		mx_out_rgb = mxCreateCellMatrix(mxGetM(mx_img), mxGetN(mx_img));
		mx_out_a   = mxCreateCellMatrix(mxGetM(mx_img), mxGetN(mx_img));
		for(i = 0; i < num_imgs; i++)
		{
			mxArray* rgb_mat, *a_mat;
			ImportImage(mx_img, i, src_img);
			if(!DirectX::HasAlpha(src_img.format))
			{
				srgb_fmt = DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;
			}
			if(DirectX::IsCompressed(src_img.format))
			{
				// decompress image so we can convert it
				hres = DirectX::Decompress(src_img, DXGI_FORMAT_UNKNOWN, out);
				if(FAILED(hres))
				{
					MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "DecompressError", "There was an error while decompressing the image.");
				}
				src_img = *out.GetImage(0, 0, 0);
				out = DirectX::ScratchImage();
			}
			// convert image to unsigned normalized 8-bit sRGB with alpha
			hres = DirectX::Convert(src_img, srgb_fmt, DirectX::TEX_FILTER_SRGB_OUT, DirectX::TEX_THRESHOLD_DEFAULT, out);
			if(FAILED(hres))
			{
				MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "ConversionError", "There was an error converting the image to sRGB.");
			}
			FormMatrix(*out.GetImages(), rgb_mat, a_mat);
			mxSetCell(mx_out_rgb, i, rgb_mat);
			mxSetCell(mx_out_a, i, a_mat);
		}
	}
	plhs[0] = mx_out_rgb;
	if(nlhs > 1)
	{
		plhs[1] = mx_out_a;
	}
}

static void ToMatrix(int nlhs, mxArray** plhs, int nrhs, const mxArray** prhs)
{
	mxArray* mx_out_rgb;
	mxArray* mx_out_a;
	DirectX::Image src_img = {};
	size_t i, num_imgs;
	if(nrhs < 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughInputsError", "Not enough arguments. Please supply a DdsSlice.");
	}
	else if(nrhs > 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyInputsError", "Too many arguments.");
	}
	const mxArray* mx_img = prhs[0];
	num_imgs = mxGetNumberOfElements(mx_img);
	if(num_imgs < 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughInputsError", "Not enough arguments. Please supply a DdsSlice.");
	}
	else if(num_imgs == 1)
	{
		ImportImage(mx_img, 0, src_img);
		FormMatrix(src_img, mx_out_rgb, mx_out_a);
	}
	else
	{
		mx_out_rgb = mxCreateCellMatrix(mxGetM(mx_img), mxGetN(mx_img));
		mx_out_a   = mxCreateCellMatrix(mxGetM(mx_img), mxGetN(mx_img));
		for(i = 0; i < num_imgs; i++)
		{
			mxArray* rgb_mat, *a_mat;
			ImportImage(mx_img, i, src_img);
			FormMatrix(src_img, rgb_mat, a_mat);
			mxSetCell(mx_out_rgb, i, rgb_mat);
			mxSetCell(mx_out_a, i, a_mat);
		}
	}
	plhs[0] = mx_out_rgb;
	if(nlhs > 1)
	{
		plhs[1] = mx_out_a;
	}
}

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

/* The gateway function. */
void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	
	void (* op_func)(int, mxArray* [], int, const mxArray* []) = nullptr;
	
	if(nrhs < 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughInputsError", "Not enough arguments. Please supply a directive.");
	}
	
	if(!mxIsChar(prhs[0]))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InvalidDirectiveError", "The directive supplied must be of class 'char'.");
	}
	
	if(MEXUtils::CompareString(prhs[0], "READ"))
	{
		op_func = Read;
	}
	else if(MEXUtils::CompareString(prhs[0], "READ_METADATA"))
	{
		op_func = ReadMetadata;
	}
	else if(MEXUtils::CompareString(prhs[0], "CONVERT"))
	{
		op_func = ConvertImage;
	}
	else if(MEXUtils::CompareString(prhs[0], "CONVERT_DDS"))
	{
		op_func = ConvertDDS;
	}
	else if(MEXUtils::CompareString(prhs[0], "FLIP_ROTATE"))
	{
		op_func = FlipRotateImage;
	}
	else if(MEXUtils::CompareString(prhs[0], "FLIP_ROTATE_DDS"))
	{
		op_func = FlipRotateDDS;
	}
	else if(MEXUtils::CompareString(prhs[0], "DECOMPRESS"))
	{
		op_func = DecompressImage;
	}
	else if(MEXUtils::CompareString(prhs[0], "DECOMPRESS_DDS"))
	{
		op_func = DecompressDDS;
	}
	else if(MEXUtils::CompareString(prhs[0], "IMAGE_MATRIX"))
	{
		op_func = ToImageMatrix;
	}
	else
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InvalidDirectiveError", "The directive supplied does not correspond to an operation.");
	}
	
	op_func(nlhs, plhs, nrhs - 1, prhs + 1);
	
}