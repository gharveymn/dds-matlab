#include "mex.h"
#include "mlerrorutils.h"
#include "directxtex.h"
#include "directxtex.inl"
#include "ddsio.h"

namespace ddsml
{
	template<class T>
	static void SetScalarField(mxArray* mx_struct, mwIndex idx, const char* field, mxClassID classid, T data)
	{
		mxArray* scalar = mxCreateNumericMatrix(1, 1, classid, mxREAL);
		*(T*)mxGetData(scalar) = data;
		mxSetField(mx_struct, idx, field, scalar);
	}
	
	
	static void mxToUpper(mxArray* mx_str)
	{
		size_t i;
		size_t n = mxGetNumberOfElements(mx_str);
		auto data = (mxChar*)mxGetData(mx_str);
		for(i = 0; i < n; i++)
		{
			*(data + i) = (mxChar)toupper(*(data + i));
			
		}
	}
	
	static void mxToLower(mxArray* mx_str)
	{
		size_t i;
		size_t n = mxGetNumberOfElements(mx_str);
		auto data = (mxChar*)mxGetData(mx_str);
		for(i = 0; i < n; i++)
		{
			*(data + i) = (mxChar)tolower(*(data + i));
			
		}
	}
	
	static wchar_t* ExtractFilename(const mxArray* mx_filename)
	{
		mwIndex i;
		if(!mxIsChar(mx_filename))
		{
			meu_PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InputError", "Filenames must be of type 'char'.");
		}
		mxChar* mx_str = mxGetChars(mx_filename);
		mwSize mx_strlen = mxGetNumberOfElements(mx_filename);
		auto filename = (wchar_t*)mxMalloc((mx_strlen + 1)*sizeof(wchar_t));
		for(i = 0; i < mx_strlen; i++)
		{
			filename[i] = mx_str[i];
		}
		filename[mx_strlen] = 0;
		return filename;
	}
	
	
	static bool CompareString(const mxArray* mx_arr, const char cmpstr[])
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
	
	
	static mxArray* ExtractFormat(DXGI_FORMAT fmt)
	{
		const char* fmt_fields[] = {
								"Name",
								"ID",
								"IsValid",
								"IsCompressed",
								"IsPacked",
								"IsVideo",
								"IsPlanar",
								"IsPalettized",
								"IsDepthStencil",
								"IsSRGB",
								"IsTypeless",
								"HasAlpha",
								"BitsPerPixel",
								"BitsPerColor"
							  };
		mxArray* mx_fmt = mxCreateStructMatrix(1, 1, 14, fmt_fields);
		mxSetField(mx_fmt, 0, "Name", mxCreateString(format_map[fmt].c_str()));
		SetScalarField(mx_fmt, 0, "ID", mxUINT32_CLASS, fmt);
		mxSetField(mx_fmt, 0, "IsValid",        mxCreateLogicalScalar(DirectX::IsValid(fmt)));
		mxSetField(mx_fmt, 0, "IsCompressed",   mxCreateLogicalScalar(DirectX::IsCompressed(fmt)));
		mxSetField(mx_fmt, 0, "IsPacked",       mxCreateLogicalScalar(DirectX::IsPacked(fmt)));
		mxSetField(mx_fmt, 0, "IsVideo",        mxCreateLogicalScalar(DirectX::IsVideo(fmt)));
		mxSetField(mx_fmt, 0, "IsPlanar",       mxCreateLogicalScalar(DirectX::IsPlanar(fmt)));
		mxSetField(mx_fmt, 0, "IsPalettized",   mxCreateLogicalScalar(DirectX::IsPalettized(fmt)));
		mxSetField(mx_fmt, 0, "IsDepthStencil", mxCreateLogicalScalar(DirectX::IsDepthStencil(fmt)));
		mxSetField(mx_fmt, 0, "IsSRGB",         mxCreateLogicalScalar(DirectX::IsSRGB(fmt)));
		mxSetField(mx_fmt, 0, "IsTypeless",     mxCreateLogicalScalar(DirectX::IsTypeless(fmt)));
		mxSetField(mx_fmt, 0, "HasAlpha",       mxCreateLogicalScalar(DirectX::HasAlpha(fmt)));
		SetScalarField(mx_fmt, 0, "BitsPerPixel", mxUINT64_CLASS,     DirectX::BitsPerPixel(fmt));
		SetScalarField(mx_fmt, 0, "BitsPerColor", mxUINT64_CLASS,     DirectX::BitsPerColor(fmt));
		return mx_fmt;
	}
	
	
	static void ParseFlags(const mxArray* flags_array, BiMap& map, DWORD& flags)
	{
		mwIndex i;
		mxArray* curr_flag;
		
		if(mxIsCell(flags_array))
		{
			mwSize num_flags = mxGetNumberOfElements(flags_array);
			for(i = 0; i < num_flags; i++)
			{
				curr_flag = mxGetCell(flags_array, i);
				if(!mxIsChar(curr_flag))
				{
					meu_PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InputError", "Flags must be of type 'char'.");
				}
				mxToUpper(curr_flag);
				char* flagname = mxArrayToString(curr_flag);
				auto found = map.find(flagname);
				if(found != map.s_end())
				{
					flags |= found->second;
				}
				else
				{
					mxFree(flagname);
					meu_PrintMexError(MEU_FL, MEU_SEVERITY_USER, "UnexpectedFlagError", "An unexpected flag was encountered during input parsing.");
				}
				mxFree(flagname);
			}
		}
		else if(mxIsChar(flags_array))
		{
			mxToUpper((mxArray*)flags_array);
			char* flagname = mxArrayToString(flags_array);
			auto found = map.find(flagname);
			if(found != map.s_end())
			{
				flags |= found->second;
			}
			else
			{
				mxFree(flagname);
				meu_PrintMexError(MEU_FL, MEU_SEVERITY_USER, "UnexpectedFlagError", "An unexpected flag was encountered during input parsing.");
			}
			mxFree(flagname);
		}
		else
		{
			meu_PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InputError", "Flags must be contained by a cell array or be a character array.");
		}
	}
	
	
	static mxArray* ExtractMetadata(DirectX::TexMetadata metadata)
	{
		const char* fieldnames[] = {"Width", "Height", "Depth", "ArraySize", "MipLevels", "MiscFlags", "MiscFlags2", "Format", "Dimension"};
		mxArray* mx_metadata = mxCreateStructMatrix(1, 1, 9, fieldnames);
		SetScalarField(mx_metadata, 0, "Width", mxUINT64_CLASS, metadata.width);
		SetScalarField(mx_metadata, 0, "Height", mxUINT64_CLASS, metadata.height);
		SetScalarField(mx_metadata, 0, "Depth", mxUINT64_CLASS, metadata.depth);
		SetScalarField(mx_metadata, 0, "ArraySize", mxUINT64_CLASS, metadata.arraySize);
		SetScalarField(mx_metadata, 0, "MipLevels", mxUINT64_CLASS, metadata.mipLevels);
		SetScalarField(mx_metadata, 0, "MiscFlags", mxUINT32_CLASS, metadata.miscFlags);
		SetScalarField(mx_metadata, 0, "MiscFlags2", mxUINT32_CLASS, metadata.miscFlags2);
		mxSetField(mx_metadata, 0, "Format", ExtractFormat(metadata.format));
		SetScalarField(mx_metadata, 0, "Dimension", mxUINT8_CLASS, metadata.dimension - 1);
		return mx_metadata;
	}
	
	
	static void ParseMetadata(const mxArray* mx_metadata, DirectX::TexMetadata& metadata)
	{
		metadata.width =      (size_t)*(uint64_T*)mxGetData(mxGetField(mx_metadata, 0, "Width"));
		metadata.height =     (size_t)*(uint64_T*)mxGetData(mxGetField(mx_metadata, 0, "Height"));
		metadata.depth =      (size_t)*(uint64_T*)mxGetData(mxGetField(mx_metadata, 0, "Depth"));
		metadata.arraySize =  (size_t)*(uint64_T*)mxGetData(mxGetField(mx_metadata, 0, "ArraySize"));
		metadata.mipLevels =  (size_t)*(uint64_T*)mxGetData(mxGetField(mx_metadata, 0, "MipLevels"));
		metadata.miscFlags =  (uint32_t)*(uint32_T*)mxGetData(mxGetField(mx_metadata, 0, "MiscFlags"));
		metadata.miscFlags2 = (uint32_t)*(uint32_T*)mxGetData(mxGetField(mx_metadata, 0, "MiscFlags2"));
		metadata.format =     (DXGI_FORMAT)*(uint32_T*)mxGetData(mxGetField(mxGetField(mx_metadata, 0, "Format"), 0, "ID"));
		metadata.dimension =  (DirectX::TEX_DIMENSION)(*(uint8_T*)mxGetData(mxGetField(mx_metadata, 0, "Dimension")) + 1);
	}
	
	
	static void ParseImage(const mxArray* mx_image, size_t idx, DirectX::Image& image)
	{
		image.width =      (size_t)*(uint64_T*)mxGetData(mxGetField(mx_image, idx, "Width"));
		image.height =     (size_t)*(uint64_T*)mxGetData(mxGetField(mx_image, idx, "Height"));
		image.format =     (DXGI_FORMAT)*(uint32_T*)mxGetData(mxGetField(mx_image, idx, "FormatID"));
		image.rowPitch =   (size_t)*(uint64_T*)mxGetData(mxGetField(mx_image, idx, "RowPitch"));
		image.slicePitch = (size_t)*(uint64_T*)mxGetData(mxGetField(mx_image, idx, "SlicePitch"));
		image.pixels =     (uint8_t*)mxGetData(mxGetField(mx_image, idx, "Pixels"));
	}


/*
static mxArray* RowToColumn2(uint8_t* src, size_t row_pitch, size_t slice_pitch, size_t bpp)
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
	
	static mxArray* ExtractImages(const DirectX::Image* images, size_t array_size, size_t mip_levels, size_t depth, DirectX::TEX_DIMENSION dimension)
	{
		mxArray* tmp;
		mwIndex i;
		const char* fieldnames[] = {"Width", "Height", "Depth", "RowPitch", "SlicePitch", "Pixels"};
		mxArray* mx_images = mxCreateStructMatrix(array_size, mip_levels, 6, fieldnames);
		
		switch (dimension)
		{
			case DirectX::TEX_DIMENSION_TEXTURE1D:
			case DirectX::TEX_DIMENSION_TEXTURE2D:
			{
				size_t num_images = array_size * mip_levels;
				for(i = 0; i < num_images; i++)
				{
					SetScalarField(mx_images, i, "Width", mxUINT64_CLASS, images[i].width);
					SetScalarField(mx_images, i, "Height", mxUINT64_CLASS, images[i].height);
					SetScalarField(mx_images, i, "Depth", mxUINT64_CLASS, depth);
					SetScalarField(mx_images, i, "RowPitch", mxUINT64_CLASS, images[i].rowPitch);
					SetScalarField(mx_images, i, "SlicePitch", mxUINT64_CLASS, images[i].slicePitch);
					tmp = mxCreateNumericMatrix(images[i].slicePitch, 1, mxUINT8_CLASS, mxREAL);
					memcpy(mxGetData(tmp), images[i].pixels, images[i].slicePitch*sizeof(uint8_T));
					mxSetField(mx_images, i, "Pixels", tmp);
				}
				break;
			}
			case DirectX::TEX_DIMENSION_TEXTURE3D:
			{
				for(i = 0; i < mip_levels; i++)
				{
					SetScalarField(mx_images, i, "Width", mxUINT64_CLASS, images[i].width);
					SetScalarField(mx_images, i, "Height", mxUINT64_CLASS, images[i].height);
					SetScalarField(mx_images, i, "Depth", mxUINT64_CLASS, depth);
					SetScalarField(mx_images, i, "RowPitch", mxUINT64_CLASS, images[i].rowPitch);
					SetScalarField(mx_images, i, "SlicePitch", mxUINT64_CLASS, images[i].slicePitch);
					tmp = mxCreateNumericMatrix(images[i].slicePitch * depth, 1, mxUINT8_CLASS, mxREAL);
					memcpy(mxGetData(tmp), images[i].pixels, images[i].slicePitch*depth*sizeof(uint8_T));
					mxSetField(mx_images, i, "Pixels", tmp);
					
					if(depth > 1)
					{
						depth >>=1;
					}
					
				}
				break;
			}
			default:
			{
				meu_PrintMexError(MEU_FL, MEU_SEVERITY_INTERNAL, "ImageDimensionError", "The image was of unexpected dimensionality (%d).", dimension);
			}
		}
		return mx_images;
	}
	
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
			meu_PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughInputsError", "Not enough arguments. Please supply a file name.");
		}
		else if(nrhs > 2)
		{
			meu_PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyInputsError", "Too many arguments.");
		}
		const mxArray* mx_filenames = prhs[0];
		
		if(nrhs == 2)
		{
			ParseFlags(prhs[1], ctrlflag_map, flags);
		}
		
		if(mxIsCell(mx_filenames))
		{
			ddsret = mxCreateStructMatrix(mxGetM(mx_filenames), mxGetN(mx_filenames), 2, ddsret_fieldnames);
			size_t num_fns = mxGetNumberOfElements(mx_filenames);
			for(i = 0; i < num_fns; i++)
			{
				filename = ExtractFilename(mxGetCell(mx_filenames, i));
				hres = DirectX::LoadFromDDSFile(filename, flags, &metadata, out);
				if(FAILED(hres))
				{
					meu_PrintMexError(MEU_FL, MEU_SEVERITY_USER | MEU_SEVERITY_HRESULT, "FileReadError", "There was an error while reading the file.");
				}
				mxSetField(ddsret, i, "Metadata", ExtractMetadata(metadata));
				mxSetField(ddsret, i, "Images", ExtractImages(out.GetImages(), metadata.arraySize, metadata.mipLevels, metadata.depth, metadata.dimension));
				mxFree(filename);
			}
		}
		else
		{
			ddsret = mxCreateStructMatrix(1, 1, 2, ddsret_fieldnames);
			filename = ExtractFilename(mx_filenames);
			hres = DirectX::LoadFromDDSFile(filename, flags, &metadata, out);
			if(FAILED(hres))
			{
				meu_PrintMexError(MEU_FL, MEU_SEVERITY_USER | MEU_SEVERITY_HRESULT, "FileReadError", "There was an error while reading the file.");
			}
			mxSetField(ddsret, 0, "Metadata", ExtractMetadata(metadata));
			mxSetField(ddsret, 0, "Images", ExtractImages(out.GetImages(), metadata.arraySize, metadata.mipLevels, metadata.depth, metadata.dimension));
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
			meu_PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughInputsError", "Not enough arguments. Please supply a file name.");
		}
		else if(nrhs > 2)
		{
			meu_PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyInputsError", "Too many arguments.");
		}
		wchar_t* filename = ExtractFilename(prhs[0]);
		
		if(nrhs == 2)
		{
			ParseFlags(prhs[1], ctrlflag_map, flags);
		}
		
		hres = DirectX::GetMetadataFromDDSFile(filename, flags, metadata);
		if(FAILED(hres))
		{
			meu_PrintMexError(MEU_FL, MEU_SEVERITY_USER | MEU_SEVERITY_HRESULT, "FileReadError", "There was an error while reading the file.");
		}
		
		plhs[0] = ExtractMetadata(metadata);
		mxFree(filename);
		
	}
	
	
	static DXGI_FORMAT ParseFormat(const mxArray* mx_fmt)
	{
		DXGI_FORMAT fmt = DXGI_FORMAT_UNKNOWN;
		if(!mxIsChar(mx_fmt))
		{
			meu_PrintMexError(MEU_FL, MEU_SEVERITY_USER | MEU_SEVERITY_INTERNAL, "FormatParsingError", "Format input must be of class 'char'.");
		}
		mxToUpper((mxArray*)mx_fmt);
		char* fmtname = mxArrayToString(mx_fmt);
		auto found = format_map.find(fmtname);
		if(found != format_map.s_end())
		{
			fmt = (DXGI_FORMAT)found->second;
		}
		else
		{
			mxFree(fmtname);
			meu_PrintMexError(MEU_FL, MEU_SEVERITY_USER, "UnexpectedFlagError", "An unexpected flag was encountered during input parsing.");
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
			meu_PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughInputsError", "Not enough arguments. Please supply a DdsImage and a format string");
		}
		else if(nrhs > 4)
		{
			meu_PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyInputsError", "Too many arguments.");
		}
		
		const mxArray* mx_img = prhs[0];
		
		DXGI_FORMAT fmt = ParseFormat(prhs[1]);
		
		if(nrhs > 2)
		{
			ParseFlags(prhs[2], filterflag_map, filter);
		}
		
		if(nrhs > 3)
		{
			threshold = (float)mxGetScalar(prhs[3]);
		}
		
		mxArray* ddsret = mxCreateStructMatrix(mxGetM(mx_img), mxGetN(mx_img), 2, ddsret_fieldnames);
		num_imgs = mxGetNumberOfElements(mx_img);
		for(i = 0; i < num_imgs; i++)
		{
			ParseImage(mx_img, i, src_img);
			hres = DirectX::Convert(src_img, fmt, filter, threshold, out);
			if(FAILED(hres))
			{
				meu_PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "ConversionError", "There was an error converting the image.");
			}
			DirectX::TexMetadata out_meta = out.GetMetadata();
			mxSetField(ddsret, i, "Metadata", ExtractMetadata(out_meta));
			mxSetField(ddsret, i, "Images", ExtractImages(out.GetImages(), out_meta.arraySize, out_meta.mipLevels, out_meta.depth, out_meta.dimension));
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
			meu_PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughInputsError", "Not enough arguments. Please supply a Dds and a format string.");
		}
		else if(nrhs > 4)
		{
			meu_PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyInputsError", "Too many arguments.");
		}
		const mxArray* mx_dds = prhs[0];
		
		DXGI_FORMAT fmt = ParseFormat(prhs[1]);
		
		if(nrhs > 2)
		{
			ParseFlags(prhs[2], filterflag_map, filter);
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
			ParseMetadata(mx_metadata, src_metadata);
			num_imgs = mxGetNumberOfElements(mx_images);
			src_imgs = (DirectX::Image*)mxMalloc(num_imgs * sizeof(DirectX::Image));
			for(j = 0; j < num_imgs; j++)
			{
				ParseImage(mx_images, j, *(src_imgs + j));
			}
			
			hres = DirectX::Convert(src_imgs, num_imgs, src_metadata, fmt, filter, threshold, out);
			if(FAILED(hres))
			{
				meu_PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "ConversionError", "There was an error converting the image.");
			}
			DirectX::TexMetadata out_meta = out.GetMetadata();
			mxSetField(ddsret, i, "Metadata", ExtractMetadata(out_meta));
			mxSetField(ddsret, i, "Images", ExtractImages(out.GetImages(), out_meta.arraySize, out_meta.mipLevels, out_meta.depth, out_meta.dimension));
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
			meu_PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughInputsError", "Not enough arguments. Please supply a DdsImage and flags.");
		}
		else if(nrhs > 2)
		{
			meu_PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyInputsError", "Too many arguments.");
		}
		
		const mxArray* mx_img = prhs[0];
		
		ParseFlags(prhs[1], fr_map, flags);
		
		mxArray* ddsret = mxCreateStructMatrix(mxGetM(mx_img), mxGetN(mx_img), 2, ddsret_fieldnames);
		num_imgs = mxGetNumberOfElements(mx_img);
		for(i = 0; i < num_imgs; i++)
		{
			ParseImage(mx_img, i, src_img);
			hres = DirectX::FlipRotate(src_img, flags, out);
			if(FAILED(hres))
			{
				meu_PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "FlipRotateError", "There was an error while rotating or flipping the image.");
			}
			DirectX::TexMetadata out_meta = out.GetMetadata();
			mxSetField(ddsret, i, "Metadata", ExtractMetadata(out_meta));
			mxSetField(ddsret, i, "Images", ExtractImages(out.GetImages(), out_meta.arraySize, out_meta.mipLevels, out_meta.depth, out_meta.dimension));
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
			meu_PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughInputsError", "Not enough arguments. Please supply a DdsImage and flags.");
		}
		else if(nrhs > 2)
		{
			meu_PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyInputsError", "Too many arguments.");
		}
		const mxArray* mx_dds = prhs[0];
		
		ParseFlags(prhs[1], fr_map, flags);
		
		mxArray* ddsret = mxCreateStructMatrix(mxGetM(mx_dds), mxGetN(mx_dds), 2, ddsret_fieldnames);
		num_dds = mxGetNumberOfElements(mx_dds);
		for(i = 0; i < num_dds; i++)
		{
			mxArray* mx_metadata = mxGetField(mx_dds, i, "Metadata");
			mxArray* mx_images   = mxGetField(mx_dds, i, "Images");
			ParseMetadata(mx_metadata, src_metadata);
			num_imgs = mxGetNumberOfElements(mx_images);
			src_imgs = (DirectX::Image*)mxMalloc(num_imgs * sizeof(DirectX::Image));
			for(j = 0; j < num_imgs; j++)
			{
				ParseImage(mx_images, j, *(src_imgs + j));
			}
			
			hres = DirectX::FlipRotate(src_imgs, num_imgs, src_metadata, flags, out);
			if(FAILED(hres))
			{
				meu_PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "FlipRotateError", "There was an error while rotating or flipping the image.");
			}
			DirectX::TexMetadata out_meta = out.GetMetadata();
			mxSetField(ddsret, i, "Metadata", ExtractMetadata(out_meta));
			mxSetField(ddsret, i, "Images", ExtractImages(out.GetImages(), out_meta.arraySize, out_meta.mipLevels, out_meta.depth, out_meta.dimension));
			mxFree(src_imgs);
		}
		plhs[0] = ddsret;
		
	}
	
	// FlipRotate                      | DONE
	// Resize                          |
	// Convert                         | DONE
	// ConvertToSinglePlane            |
	// GenerateMipMaps                 |
	// GenerateMipMaps3D               |
	// ScaleMipMapsAlphaForCoverage    |
	// PremultiplyAlpha                |
	// Compress                        |
	// Decompress                      |
	// ComputeNormalMap                |
	// ComputeMSE                      |
	
	// ToImageMatrix                   |
	
	static void ToImageMatrix(int nlhs, mxArray** plhs, int nrhs, const mxArray** prhs)
	{
	
	}

/* The gateway function. */
	void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
	{
		
		void (* op_func)(int, mxArray* [], int, const mxArray* []) = nullptr;
		
		meu_SetLibraryName("dds");
		
		if(nrhs < 1)
		{
			meu_PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughInputsError", "Not enough arguments. Please supply a directive.");
		}
		
		if(!mxIsChar(prhs[0]))
		{
			meu_PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InvalidDirectiveError", "The directive supplied must be of class 'char'.");
		}
		
		if(CompareString(prhs[0], "READ"))
		{
			op_func = Read;
		}
		else if(CompareString(prhs[0], "READ_METADATA"))
		{
			op_func = ReadMetadata;
		}
		else if(CompareString(prhs[0], "CONVERT"))
		{
			op_func = ConvertImage;
		}
		else if(CompareString(prhs[0], "CONVERT_DDS"))
		{
			op_func = ConvertDDS;
		}
		else if(CompareString(prhs[0], "FLIPROTATE"))
		{
			op_func = FlipRotateImage;
		}
		else if(CompareString(prhs[0], "FLIPROTATE_DDS"))
		{
			op_func = FlipRotateDDS;
		}
		else
		{
			meu_PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InvalidDirectiveError", "The directive supplied does not correspond to an operation.");
		}
		
		op_func(nlhs, plhs, nrhs - 1, prhs + 1);
		
	}
}