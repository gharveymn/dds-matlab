#include "mex.h"
#include "ddsmex.hpp"
#include "ddsmex_dds.hpp"
#include "ddsmex_mexerror.hpp"
#include "ddsmex_mexutils.hpp"

using namespace DDSMex;

DDSImage::DDSImage(const mxArray* mx_metadata, const mxArray* mx_images)
{
	// Import metadata first, then determine which Initialize function to use
	DirectX::TexMetadata metadata = {};
	ImportMetadata(mx_metadata, metadata);
	this->m_flags = (DWORD)*(uint32_T*)mxGetData(mxGetField(mx_metadata, 0, "Flags"));
	this->Initialize(metadata, this->m_flags);
	ImportImages(mx_images, (DirectX::Image*)this->GetImages(), metadata.arraySize, metadata.mipLevels, metadata.depth, metadata.dimension);
}

void DDSImage::ImportMetadata(const mxArray* mx_metadata, DirectX::TexMetadata& metadata)
{
	metadata.width      = (size_t)*(uint64_T*)mxGetData(mxGetField(mx_metadata, 0, "Width"));
	metadata.height     = (size_t)*(uint64_T*)mxGetData(mxGetField(mx_metadata, 0, "Height"));
	metadata.depth      = (size_t)*(uint64_T*)mxGetData(mxGetField(mx_metadata, 0, "Depth"));
	metadata.arraySize  = (size_t)*(uint64_T*)mxGetData(mxGetField(mx_metadata, 0, "ArraySize"));
	metadata.mipLevels  = (size_t)*(uint64_T*)mxGetData(mxGetField(mx_metadata, 0, "MipLevels"));
	metadata.miscFlags  = (uint32_t)*(uint32_T*)mxGetData(mxGetField(mx_metadata, 0, "MiscFlags"));
	metadata.miscFlags2 = (uint32_t)*(uint32_T*)mxGetData(mxGetField(mx_metadata, 0, "MiscFlags2"));
	metadata.format     = (DXGI_FORMAT)*(uint32_T*)mxGetData(mxGetField(mxGetField(mx_metadata, 0, "Format"), 0, "ID"));
	metadata.dimension  = (DirectX::TEX_DIMENSION)(*(uint8_T*)mxGetData(mxGetField(mx_metadata, 0, "Dimension")) + 1);
}

void DDSImage::ImportImages(const mxArray* mx_images, DirectX::Image* images, size_t array_size, size_t mip_levels, size_t depth, DirectX::TEX_DIMENSION dimension)
{
	mwIndex i, j, src_idx, dst_idx;
	switch (dimension)
	{
		case DirectX::TEX_DIMENSION_TEXTURE1D:
		case DirectX::TEX_DIMENSION_TEXTURE2D:
		{
			for(i = 0; i < array_size; i++)
			{
				for(j = 0; j < mip_levels; j++)
				{
					src_idx = i + j*mip_levels;
					dst_idx = i*mip_levels + j;
					images[dst_idx].width =      (size_t)*(uint64_T*)mxGetData(mxGetField(mx_images, src_idx, "Width"));
					images[dst_idx].height =     (size_t)*(uint64_T*)mxGetData(mxGetField(mx_images, src_idx, "Height"));
					images[dst_idx].format =     (DXGI_FORMAT)*(uint32_T*)mxGetData(mxGetField(mx_images, src_idx, "FormatID"));
					images[dst_idx].rowPitch =   (size_t)*(uint64_T*)mxGetData(mxGetField(mx_images, src_idx, "RowPitch"));
					images[dst_idx].slicePitch = (size_t)*(uint64_T*)mxGetData(mxGetField(mx_images, src_idx, "SlicePitch"));
					memcpy(images[dst_idx].pixels, mxGetData(mxGetField(mx_images, src_idx, "Pixels")), images[dst_idx].slicePitch * sizeof(uint8_T));
				}
			}
			break;
		}
		case DirectX::TEX_DIMENSION_TEXTURE3D:
		{
			for(i = 0; i < depth*mip_levels; i++)
			{
				images[i].width =      (size_t)*(uint64_T*)mxGetData(mxGetField(mx_images, i, "Width"));
				images[i].height =     (size_t)*(uint64_T*)mxGetData(mxGetField(mx_images, i, "Height"));
				images[i].format =     (DXGI_FORMAT)*(uint32_T*)mxGetData(mxGetField(mx_images, i, "FormatID"));
				images[i].rowPitch =   (size_t)*(uint64_T*)mxGetData(mxGetField(mx_images, i, "RowPitch"));
				images[i].slicePitch = (size_t)*(uint64_T*)mxGetData(mxGetField(mx_images, i, "SlicePitch"));
				memcpy(images[i].pixels, mxGetData(mxGetField(mx_images, i, "Pixels")), images[i].slicePitch * sizeof(uint8_T));
			}
			break;
		}
		default:
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_INTERNAL, "ImageDimensionError", "The image was of unexpected dimensionality (%d).", dimension - 1);
		}
	}
}

mxArray* DDSImage::ExportMetadata()
{
	DirectX::TexMetadata metadata = this->GetMetadata();
	const char* fieldnames[] = {"Width", "Height", "Depth", "ArraySize", "MipLevels", "MiscFlags", "MiscFlags2", "Format", "Dimension", "Flags", "IsCubeMap", "IsPMAlpha", "IsVolumeMap"};
	mxArray* mx_metadata = mxCreateStructMatrix(1, 1, 13, fieldnames);
	MEXUtils::SetScalarField(mx_metadata, 0, "Width", mxUINT64_CLASS, metadata.width);
	MEXUtils::SetScalarField(mx_metadata, 0, "Height", mxUINT64_CLASS, metadata.height);
	MEXUtils::SetScalarField(mx_metadata, 0, "Depth", mxUINT64_CLASS, metadata.depth);
	MEXUtils::SetScalarField(mx_metadata, 0, "ArraySize", mxUINT64_CLASS, metadata.arraySize);
	MEXUtils::SetScalarField(mx_metadata, 0, "MipLevels", mxUINT64_CLASS, metadata.mipLevels);
	MEXUtils::SetScalarField(mx_metadata, 0, "MiscFlags", mxUINT32_CLASS, metadata.miscFlags);
	MEXUtils::SetScalarField(mx_metadata, 0, "MiscFlags2", mxUINT32_CLASS, metadata.miscFlags2);
	mxSetField(mx_metadata, 0, "Format", ExportFormat(metadata.format));
	MEXUtils::SetScalarField(mx_metadata, 0, "Dimension", mxUINT8_CLASS, metadata.dimension - 1);
	MEXUtils::SetScalarField(mx_metadata, 0, "Flags", mxUINT32_CLASS, this->m_flags);
	mxSetField(mx_metadata, 0, "IsCubeMap", mxCreateLogicalScalar(metadata.IsCubemap()));
	mxSetField(mx_metadata, 0, "IsPMAlpha", mxCreateLogicalScalar(metadata.IsPMAlpha()));
	mxSetField(mx_metadata, 0, "IsVolumeMap", mxCreateLogicalScalar(metadata.IsVolumemap()));
	return mx_metadata;
}

mxArray* DDSImage::ExportFormat(DXGI_FORMAT fmt)
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
	MEXUtils::SetScalarField(mx_fmt, 0, "ID", mxUINT32_CLASS, fmt);
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
	MEXUtils::SetScalarField(mx_fmt, 0, "BitsPerPixel", mxUINT64_CLASS,     DirectX::BitsPerPixel(fmt));
	MEXUtils::SetScalarField(mx_fmt, 0, "BitsPerColor", mxUINT64_CLASS,     DirectX::BitsPerColor(fmt));
	return mx_fmt;
}

mxArray* DDSImage::ExportImages()
{
	mxArray* tmp,* mx_images;
	mwIndex i, j, src_idx, dst_idx;
	const char* fieldnames[] = {"Width", "Height", "Depth", "RowPitch", "SlicePitch", "Pixels"};
	
	const DirectX::Image* images = this->GetImages();
	DirectX::TexMetadata metadata = this->GetMetadata();
	
	switch (metadata.dimension)
	{
		case DirectX::TEX_DIMENSION_TEXTURE1D:
		case DirectX::TEX_DIMENSION_TEXTURE2D:
		{
			mx_images = mxCreateStructMatrix(metadata.arraySize, metadata.mipLevels, 6, fieldnames);
			for(i = 0; i < metadata.arraySize; i++)
			{
				for(j = 0; j < metadata.mipLevels; j++)
				{
					src_idx = i*metadata.mipLevels + j;
					dst_idx = i + j*metadata.mipLevels;
					MEXUtils::SetScalarField(mx_images, dst_idx, "Width", mxUINT64_CLASS, images[src_idx].width);
					MEXUtils::SetScalarField(mx_images, dst_idx, "Height", mxUINT64_CLASS, images[src_idx].height);
					MEXUtils::SetScalarField(mx_images, dst_idx, "RowPitch", mxUINT64_CLASS, images[src_idx].rowPitch);
					MEXUtils::SetScalarField(mx_images, dst_idx, "SlicePitch", mxUINT64_CLASS, images[src_idx].slicePitch);
					tmp = mxCreateNumericMatrix(images[src_idx].slicePitch, 1, mxUINT8_CLASS, mxREAL);
					memcpy(mxGetData(tmp), images[src_idx].pixels, images[src_idx].slicePitch*sizeof(uint8_T));
					mxSetField(mx_images, dst_idx, "Pixels", tmp);
				}
			}
			return mx_images;
		}
		case DirectX::TEX_DIMENSION_TEXTURE3D:
		{
			mx_images = mxCreateStructMatrix(metadata.depth, metadata.mipLevels, 6, fieldnames);
			for(i = 0; i < metadata.depth*metadata.mipLevels; i++)
			{
				MEXUtils::SetScalarField(mx_images, i, "Width", mxUINT64_CLASS, images[i].width);
				MEXUtils::SetScalarField(mx_images, i, "Height", mxUINT64_CLASS, images[i].height);
				MEXUtils::SetScalarField(mx_images, i, "RowPitch", mxUINT64_CLASS, images[i].rowPitch);
				MEXUtils::SetScalarField(mx_images, i, "SlicePitch", mxUINT64_CLASS, images[i].slicePitch);
				tmp = mxCreateNumericMatrix(images[i].slicePitch, 1, mxUINT8_CLASS, mxREAL);
				memcpy(mxGetData(tmp), images[i].pixels, images[i].slicePitch*sizeof(uint8_T));
				mxSetField(mx_images, i, "Pixels", tmp);
				if(metadata.depth > 1)
				{
					metadata.depth >>= 1u;
				}
			}
			return mx_images;
		}
		default:
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_INTERNAL, "ImageDimensionError", "The image was of unexpected dimensionality (%d).", metadata.dimension - 1);
		}
	}
	return nullptr;
}

DDS DDS::ReadFile(int nrhs, const mxArray* prhs[])
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
}

class DDS
{

private:
	wchar_t* ImportFilename(const mxArray* mx_filename)
	{
		mwIndex i;
		if(!mxIsChar(mx_filename))
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InputError", "Filenames must be of type 'char'.");
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
	
	void ImportFlags(const mxArray* flags_array, BiMap &map, DWORD &flags)
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
					MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InputError", "Flags must be of type 'char'.");
				}
				MEXUtils::ToUpper(curr_flag);
				char* flagname = mxArrayToString(curr_flag);
				auto found = map.find(flagname);
				if(found != map.s_end())
				{
					flags |= found->second;
				}
				else
				{
					mxFree(flagname);
					MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "UnexpectedFlagError", "An unexpected flag was encountered during input parsing.");
				}
				mxFree(flagname);
			}
		}
		else if(mxIsChar(flags_array))
		{
			MEXUtils::ToUpper((mxArray*)flags_array);
			char* flagname = mxArrayToString(flags_array);
			auto found = map.find(flagname);
			if(found != map.s_end())
			{
				flags |= found->second;
			}
			else
			{
				mxFree(flagname);
				MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "UnexpectedFlagError", "An unexpected flag was encountered during input parsing.");
			}
			mxFree(flagname);
		}
		else
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InputError", "Flags must be contained by a cell array or be a character array.");
		}
	}
	
	
};
