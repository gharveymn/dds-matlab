#include "mex.h"
#include <string>
#include "dxtmex_dxtimagearray.hpp"
#include "dxtmex_mexerror.hpp"
#include "dxtmex_mexutils.hpp"
#include "dxtmex_maps.hpp"
#include "dxtmex_flags.hpp"

using namespace DXTMEX;

/* MATLAB:DXTImageSlice constructor */
DXTImage::DXTImage(const mxArray* mx_width, const mxArray* mx_height, const mxArray* mx_row_pitch, const mxArray* mx_slice_pitch, const mxArray* mx_pixels, const mxArray* mx_formatid, const mxArray* mx_flags)
{
	if((mx_width == nullptr) || (mx_height == nullptr) || (mx_row_pitch == nullptr) || (mx_slice_pitch == nullptr) || (mx_pixels == nullptr) || (mx_formatid == nullptr) || (mx_flags == nullptr))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER|MEU_SEVERITY_INTERNAL, "InvalidImportError", "An imported DXTImageSlice field was unexpectedly empty.");
	}
	this->_type = DXTImage::UNKNOWN;
	this->_dds_flags = (DirectX::DDS_FLAGS)*(uint32_T*)mxGetData(mx_flags);
	
	auto width =      (size_t)*(uint64_T*)mxGetData(mx_width);
	auto height =     (size_t)*(uint64_T*)mxGetData(mx_height);
	auto format =     (DXGI_FORMAT)*(uint32_T*)mxGetData(mx_formatid);
	
	this->Initialize2D(format, width, height, 1, 1, this->GetFlags());
	
	DirectX::Image image = *this->GetImages();
	if(image.rowPitch != (size_t)*(uint64_T*)mxGetData(mx_row_pitch))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER|MEU_SEVERITY_INTERNAL, "InvalidImportError", "The imported row pitch of the DXTImageSlice differs from the expected row pitch.");
	}
	else if(image.slicePitch != (size_t)*(uint64_T*)mxGetData(mx_slice_pitch))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER|MEU_SEVERITY_INTERNAL, "InvalidImportError", "The imported slice pitch of the DXTImageSlice differs from the expected slice pitch.");
	}
	memcpy(image.pixels, mxGetData(mx_pixels), image.slicePitch * sizeof(uint8_T));
}

/* MATLAB:DXTImage constructor */
DXTImage::DXTImage(const mxArray* mx_metadata, const mxArray* mx_images)
{
	this->SetImageType(DXTImage::UNKNOWN);
	this->SetFlags(DirectX::DDS_FLAGS_NONE);
	if(mx_metadata == nullptr)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER|MEU_SEVERITY_INTERNAL, "NotEnoughInputsError", "DXTImage import metadata was unexpectedly empty.");
	}
	if(mx_images == nullptr)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER|MEU_SEVERITY_INTERNAL, "NotEnoughInputsError", "DXTImage import images was unexpectedly empty.");
	}
	DirectX::TexMetadata metadata = {};
	ImportMetadata(mx_metadata, metadata);
	this->SetFlags((DirectX::DDS_FLAGS)*(uint32_T*)mxGetData(mxGetField(mxGetField(mx_metadata, 0, "Flags"), 0, "Value")));
	char* str_imtype = mxArrayToString(mxGetField(mx_metadata, 0, "Type"));
	this->SetImageType(GetImageTypeIDFromString(str_imtype));
	this->Initialize(metadata, this->GetFlags());
	ImportImages(mx_images, (DirectX::Image*)this->GetImages(), metadata.arraySize, metadata.mipLevels, metadata.depth, metadata.dimension);
}

void DXTImage::ImportMetadata(const mxArray* mx_metadata, DirectX::TexMetadata& metadata)
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

void DXTImage::ImportImages(const mxArray* mx_images, DirectX::Image* images, size_t array_size, size_t mip_levels, size_t depth, DirectX::TEX_DIMENSION dimension)
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

/* for ReadMetadata functions */
mxArray* DXTImage::ExportMetadata(const DirectX::TexMetadata& metadata, DXTImage::IMAGE_TYPE type)
{
	const char* fieldnames[] = {"Type", "Width", "Height", "Depth", "ArraySize", "MipLevels", "MiscFlags", "MiscFlags2", "Format", "Dimension", "AlphaMode", "IsCubeMap", "IsPMAlpha", "IsVolumeMap", "Flags"};
	mxArray* mx_metadata = mxCreateStructMatrix(1, 1, ARRAYSIZE(fieldnames), fieldnames);
	mxSetField(mx_metadata, 0, "Type", mxCreateString(GetImageTypeStringFromID(type).c_str()));
	MEXUtils::SetScalarField(mx_metadata, 0, "Width", mxUINT64_CLASS, metadata.width);
	MEXUtils::SetScalarField(mx_metadata, 0, "Height", mxUINT64_CLASS, metadata.height);
	MEXUtils::SetScalarField(mx_metadata, 0, "Depth", mxUINT64_CLASS, metadata.depth);
	MEXUtils::SetScalarField(mx_metadata, 0, "ArraySize", mxUINT64_CLASS, metadata.arraySize);
	MEXUtils::SetScalarField(mx_metadata, 0, "MipLevels", mxUINT64_CLASS, metadata.mipLevels);
	MEXUtils::SetScalarField(mx_metadata, 0, "MiscFlags", mxUINT32_CLASS, metadata.miscFlags);
	MEXUtils::SetScalarField(mx_metadata, 0, "MiscFlags2", mxUINT32_CLASS, metadata.miscFlags2);
	mxSetField(mx_metadata, 0, "Format", ExportFormat(metadata.format));
	MEXUtils::SetScalarField(mx_metadata, 0, "Dimension", mxUINT8_CLASS, metadata.dimension - 1);
	mxSetField(mx_metadata, 0, "AlphaMode", mxCreateString(GetAlphaModeStringFromID(metadata.GetAlphaMode()).c_str()));
	mxSetField(mx_metadata, 0, "IsCubeMap", mxCreateLogicalScalar(metadata.IsCubemap()));
	mxSetField(mx_metadata, 0, "IsPMAlpha", mxCreateLogicalScalar(metadata.IsPMAlpha()));
	mxSetField(mx_metadata, 0, "IsVolumeMap", mxCreateLogicalScalar(metadata.IsVolumemap()));
	return mx_metadata;
}

/* for image read functions */
mxArray* DXTImage::ExportMetadata()
{
	const DirectX::TexMetadata& metadata = this->GetMetadata();
	const char* fieldnames[] = {"Type", "Width", "Height", "Depth", "ArraySize", "MipLevels", "MiscFlags", "MiscFlags2", "Format", "Dimension", "AlphaMode", "IsCubeMap", "IsPMAlpha", "IsVolumeMap", "Flags"};
	mxArray* mx_metadata = mxCreateStructMatrix(1, 1, ARRAYSIZE(fieldnames), fieldnames);
	mxSetField(mx_metadata, 0, "Type", mxCreateString(GetImageTypeStringFromID(this->_type).c_str()));
	MEXUtils::SetScalarField(mx_metadata, 0, "Width", mxUINT64_CLASS, metadata.width);
	MEXUtils::SetScalarField(mx_metadata, 0, "Height", mxUINT64_CLASS, metadata.height);
	MEXUtils::SetScalarField(mx_metadata, 0, "Depth", mxUINT64_CLASS, metadata.depth);
	MEXUtils::SetScalarField(mx_metadata, 0, "ArraySize", mxUINT64_CLASS, metadata.arraySize);
	MEXUtils::SetScalarField(mx_metadata, 0, "MipLevels", mxUINT64_CLASS, metadata.mipLevels);
	MEXUtils::SetScalarField(mx_metadata, 0, "MiscFlags", mxUINT32_CLASS, metadata.miscFlags);
	MEXUtils::SetScalarField(mx_metadata, 0, "MiscFlags2", mxUINT32_CLASS, metadata.miscFlags2);
	mxSetField(mx_metadata, 0, "Format", ExportFormat(metadata.format));
	MEXUtils::SetScalarField(mx_metadata, 0, "Dimension", mxUINT8_CLASS, metadata.dimension - 1);
	mxSetField(mx_metadata, 0, "AlphaMode", mxCreateString(GetAlphaModeStringFromID(metadata.GetAlphaMode()).c_str()));
	mxSetField(mx_metadata, 0, "IsCubeMap", mxCreateLogicalScalar(metadata.IsCubemap()));
	mxSetField(mx_metadata, 0, "IsPMAlpha", mxCreateLogicalScalar(metadata.IsPMAlpha()));
	mxSetField(mx_metadata, 0, "IsVolumeMap", mxCreateLogicalScalar(metadata.IsVolumemap()));
	mxSetField(mx_metadata, 0, "Flags", g_ddsflags.ExportFlags(this->_dds_flags));
	return mx_metadata;
}

mxArray* DXTImage::ExportFormat(DXGI_FORMAT fmt)
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
	mxArray* mx_fmt = mxCreateStructMatrix(1, 1, ARRAYSIZE(fmt_fields), fmt_fields);
	mxSetField(mx_fmt, 0, "Name", mxCreateString(GetFormatStringFromID(fmt).c_str()));
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

mwIndex DXTImage::ComputeIndexMEX(size_t mip, size_t item, size_t slice)
{
	auto ret = size_t(-1);
	const DirectX::TexMetadata& metadata = this->GetMetadata();
	if (mip >= metadata.mipLevels)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_INTERNAL, "ImageIndexError", "The requested mipmap exceeds the number of mipmaps in the image array.");
	}
	
	switch (metadata.dimension)
	{
		case DirectX::TEX_DIMENSION_TEXTURE1D:
		case DirectX::TEX_DIMENSION_TEXTURE2D:
		{
			if(slice > 0)
			{
				MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_INTERNAL, "ImageIndexError", "The requested slice exceeds the image depth because the image is %dD.", metadata.dimension - 1);
			}
			
			if(item >= metadata.arraySize)
			{
				MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_INTERNAL, "ImageIndexError", "The requested item exceeds the number of items in the image array.");
			}
			ret = item + mip*metadata.arraySize;
			break;
		}
		case DirectX::TEX_DIMENSION_TEXTURE3D:
		{
			if(item > 0)
			{
				// No support for arrays of volumes
				MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_INTERNAL, "ImageIndexError", "Arrays of volumes are not supported.");
			}
			else
			{
				size_t index = 0;
				size_t depth = metadata.depth;
				
				for(size_t level = 0; level < mip; ++level)
				{
					index += depth;
					if(depth > 1)
					{
						depth >>= 1u;
					}
				}
				
				if(slice >= depth)
				{
					MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_INTERNAL, "ImageIndexError", "The requested slice exceeds the image depth.");
				}
				
				index += slice;
				
				ret = index;
			}
			break;
		}
		default:
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_INTERNAL, "ImageDimensionError", "The image was of unexpected dimensionality (%d).", metadata.dimension - 1);
		}
	}
	return ret;
}

mxArray* DXTImage::ExportImages()
{
	mxArray* tmp,* mx_images;
	mwIndex i, j, k, dst_idx;
	const char* fieldnames[] = {"Width", "Height", "RowPitch", "SlicePitch", "Pixels", "FormatID", "FlagsValue"};
	DirectX::TexMetadata metadata = this->GetMetadata();
	size_t depth = metadata.depth;
	
	mx_images = mxCreateStructMatrix(MAX(metadata.arraySize, metadata.depth), metadata.mipLevels, ARRAYSIZE(fieldnames), fieldnames);
	for(i = 0; i < metadata.mipLevels; i++)
	{
		for(j = 0; j < metadata.arraySize; j++)
		{
			for(k = 0; k < depth; k++)
			{
				dst_idx = this->ComputeIndexMEX(i, j, k);
				const DirectX::Image* image = this->GetImage(i, j, k);
				MEXUtils::SetScalarField(mx_images, dst_idx, "Width", mxUINT64_CLASS, image->width);
				MEXUtils::SetScalarField(mx_images, dst_idx, "Height", mxUINT64_CLASS, image->height);
				MEXUtils::SetScalarField(mx_images, dst_idx, "RowPitch", mxUINT64_CLASS, image->rowPitch);
				MEXUtils::SetScalarField(mx_images, dst_idx, "SlicePitch", mxUINT64_CLASS, image->slicePitch);
				MEXUtils::SetScalarField(mx_images, dst_idx, "FormatID", mxUINT32_CLASS, image->format);
				MEXUtils::SetScalarField(mx_images, dst_idx, "FlagsValue", mxUINT32_CLASS, this->GetFlags());
				tmp = mxCreateNumericMatrix(image->slicePitch, 1, mxUINT8_CLASS, mxREAL);
				memcpy(mxGetData(tmp), image->pixels, image->slicePitch*sizeof(uint8_T));
				mxSetField(mx_images, dst_idx, "Pixels", tmp);
			}
		}
		if (depth > 1)
		{
			depth >>= 1u;
		}
	}
	return mx_images;
}

void DXTImage::PrepareImages(DXTImage &out)
{
	DXGI_FORMAT srgb_fmt= DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	const DirectX::TexMetadata metadata = this->GetMetadata();
	
	/* prepare the images */
	if(!DirectX::HasAlpha(metadata.format))
	{
		srgb_fmt = DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;
	}
	if(DirectX::IsCompressed(metadata.format))
	{
		// decompress image so we can convert it
		DirectX::ScratchImage tmp;
		hres = DirectX::Decompress(this->GetImages(), this->GetImageCount(), this->GetMetadata(), DXGI_FORMAT_UNKNOWN, tmp);
		if(FAILED(hres))
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "DecompressError", "There was an error while decompressing the image.");
		}
		// convert image to unsigned normalized 8-bit sRGB with alpha
		hres = DirectX::Convert(tmp.GetImages(), tmp.GetImageCount(), tmp.GetMetadata(), srgb_fmt, DirectX::TEX_FILTER_SRGB_OUT, DirectX::TEX_THRESHOLD_DEFAULT, out);
		if(FAILED(hres))
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "ConversionError", "There was an error converting the image to sRGB.");
		}
	}
	else
	{
		// convert image to unsigned normalized 8-bit sRGB with alpha
		hres = DirectX::Convert(this->GetImages(), this->GetImageCount(), this->GetMetadata(), srgb_fmt, DirectX::TEX_FILTER_SRGB_OUT, DirectX::TEX_THRESHOLD_DEFAULT, out);
		if(FAILED(hres))
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "ConversionError", "There was an error converting the image to sRGB.");
		}
	}
}

void DXTImage::ToImage(mxArray*& mx_dxtimage_rgb)
{
	DXTImage prepared_images;
	this->PrepareImages(prepared_images);
	prepared_images.FormMatrix(mx_dxtimage_rgb);
}

void DXTImage::ToImage(mxArray*& mx_dxtimage_rgb, mxArray*& mx_dxtimage_a)
{
	DXTImage prepared_images;
	this->PrepareImages(prepared_images);
	prepared_images.FormMatrix(mx_dxtimage_rgb, mx_dxtimage_a);
}

void DXTImage::ToMatrix(mxArray*& mx_dxtimage_rgb, bool combine_alpha)
{
	this->FormMatrix(mx_dxtimage_rgb, combine_alpha);
}

void DXTImage::ToMatrix(mxArray*& mx_dxtimage_rgb, mxArray*& mx_dxtimage_a)
{
	this->FormMatrix(mx_dxtimage_rgb, mx_dxtimage_a);
}

void DXTImage::FormMatrix(mxArray*& mx_dxtimage_rgb, bool combine_alpha)
{
	mxArray* tmp_rgb;
	size_t i, j, k;
	
	if(this->GetImageCount() == 1)
	{
		FormMatrix(this->GetImage(0, 0, 0), mx_dxtimage_rgb, combine_alpha);
	}
	else
	{
		DirectX::TexMetadata metadata = this->GetMetadata();
		size_t depth = metadata.depth;
		mx_dxtimage_rgb = mxCreateCellMatrix(MAX(metadata.arraySize, metadata.depth), metadata.mipLevels);
		for(i = 0; i < metadata.mipLevels; i++)
		{
			for(j = 0; j < metadata.arraySize; j++)
			{
				for(k = 0; k < depth; k++)
				{
					FormMatrix(this->GetImage(i, j, k), tmp_rgb, combine_alpha);
					mxSetCell(mx_dxtimage_rgb, this->ComputeIndexMEX(i, j, k), tmp_rgb);
				}
			}
			if(depth > 1)
			{
				depth >>= 1u;
			}
		}
	}
}

void DXTImage::FormMatrix(mxArray*& mx_dxtimage_rgb, mxArray*& mx_dxtimage_a)
{
	mxArray* tmp_rgb,* tmp_a;
	size_t i, j, k;
	
	if(this->GetImageCount() == 1)
	{
		FormMatrix(this->GetImage(0, 0, 0), mx_dxtimage_rgb, mx_dxtimage_a);
	}
	else
	{
		DirectX::TexMetadata metadata = this->GetMetadata();
		size_t depth = metadata.depth;
		mx_dxtimage_rgb = mxCreateCellMatrix(MAX(metadata.arraySize, metadata.depth), metadata.mipLevels);
		mx_dxtimage_a = mxCreateCellMatrix(MAX(metadata.arraySize, metadata.depth), metadata.mipLevels);
		for(i = 0; i < metadata.mipLevels; i++)
		{
			for(j = 0; j < metadata.arraySize; j++)
			{
				for(k = 0; k < depth; k++)
				{
					FormMatrix(this->GetImage(i, j, k), tmp_rgb, tmp_a);
					mxSetCell(mx_dxtimage_rgb, this->ComputeIndexMEX(i, j, k), tmp_rgb);
					mxSetCell(mx_dxtimage_a, this->ComputeIndexMEX(i, j, k), tmp_a);
				}
			}
			if(depth > 1)
			{
				depth >>= 1u;
			}
		}
	}
}

void DXTImage::FormMatrix(const DirectX::Image* raw_img, mxArray*& mx_dxtimageslice_rgb, bool combine_alpha)
{
	size_t i, j, src_idx, dst_idx;
	if(!DirectX::IsValid(raw_img->format))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InvalidFormatError", "Cannot create matrix from image with invalid format.");
	}
	if(DirectX::IsCompressed(raw_img->format))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "CompressedFormatError", "Cannot create matrix from compressed image.");
	}
	if(combine_alpha)
	{
		// assume 8-bit unsigned normalized sRGB for now
		mwSize rgb_sz[3] = {raw_img->height, raw_img->width, 4};
		mx_dxtimageslice_rgb = mxCreateNumericArray(3, rgb_sz, mxUINT8_CLASS, mxREAL);
		auto r_data = (uint8_t*)mxGetData(mx_dxtimageslice_rgb);
		auto g_data = (uint8_t*)mxGetData(mx_dxtimageslice_rgb) + raw_img->height*raw_img->width;
		auto b_data = (uint8_t*)mxGetData(mx_dxtimageslice_rgb) + 2*raw_img->height*raw_img->width;
		auto a_data = (uint8_t*)mxGetData(mx_dxtimageslice_rgb) + 3*raw_img->height*raw_img->width;
		for(i = 0; i < raw_img->height; i++)
		{
			for(j = 0; j < raw_img->width; j++)
			{
				dst_idx = i + j*raw_img->height;
				src_idx = (i*raw_img->width + j)*4;
				r_data[dst_idx] = raw_img->pixels[src_idx];
				g_data[dst_idx] = raw_img->pixels[src_idx + 1];
				b_data[dst_idx] = raw_img->pixels[src_idx + 2];
				a_data[dst_idx] = raw_img->pixels[src_idx + 3];
			}
		}
	}
	else
	{
		// assume 8-bit unsigned normalized sRGB for now
		mwSize rgb_sz[3] = {raw_img->height, raw_img->width, 3};
		mx_dxtimageslice_rgb = mxCreateNumericArray(3, rgb_sz, mxUINT8_CLASS, mxREAL);
		auto r_data = (uint8_t*)mxGetData(mx_dxtimageslice_rgb);
		auto g_data = (uint8_t*)mxGetData(mx_dxtimageslice_rgb) + raw_img->height*raw_img->width;
		auto b_data = (uint8_t*)mxGetData(mx_dxtimageslice_rgb) + 2*raw_img->height*raw_img->width;
		for(i = 0; i < raw_img->height; i++)
		{
			for(j = 0; j < raw_img->width; j++)
			{
				dst_idx = i + j*raw_img->height;
				src_idx = (i*raw_img->width + j)*4;
				r_data[dst_idx] = raw_img->pixels[src_idx];
				g_data[dst_idx] = raw_img->pixels[src_idx + 1];
				b_data[dst_idx] = raw_img->pixels[src_idx + 2];
			}
		}
	}
}

void DXTImage::FormMatrix(const DirectX::Image* raw_img, mxArray*& mx_dxtimageslice_rgb, mxArray*& mx_dxtimageslice_a)
{
	size_t i, j, src_idx, dst_idx;
	if(!DirectX::IsValid(raw_img->format))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InvalidFormatError", "Cannot create matrix from image with invalid format.");
	}
	if(DirectX::IsCompressed(raw_img->format))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "CompressedFormatError", "Cannot create matrix from compressed image.");
	}
	// assume 8-bit unsigned normalized sRGB for now
	mwSize rgb_sz[3] = {raw_img->height, raw_img->width, 3};
	mx_dxtimageslice_rgb = mxCreateNumericArray(3, rgb_sz, mxUINT8_CLASS, mxREAL);
	if(DirectX::HasAlpha(raw_img->format))
	{
		mx_dxtimageslice_a = mxCreateNumericMatrix(raw_img->height, raw_img->width, mxUINT8_CLASS, mxREAL);
		auto r_data = (uint8_t*)mxGetData(mx_dxtimageslice_rgb);
		auto g_data = (uint8_t*)mxGetData(mx_dxtimageslice_rgb) + raw_img->height * raw_img->width;
		auto b_data = (uint8_t*)mxGetData(mx_dxtimageslice_rgb) + 2 * raw_img->height * raw_img->width;
		auto a_data = (uint8_t*)mxGetData(mx_dxtimageslice_a);
		for(i = 0; i < raw_img->height; i++)
		{
			for(j = 0; j < raw_img->width; j++)
			{
				dst_idx = i + j*raw_img->height;
				src_idx = (i*raw_img->width + j) * 4;
				r_data[dst_idx] = raw_img->pixels[src_idx];
				g_data[dst_idx] = raw_img->pixels[src_idx + 1];
				b_data[dst_idx] = raw_img->pixels[src_idx + 2];
				a_data[dst_idx] = raw_img->pixels[src_idx + 3];
			}
		}
	}
	else
	{
		mx_dxtimageslice_a = mxCreateNumericMatrix(0, 0, mxUINT8_CLASS, mxREAL);
		auto r_data = (uint8_t*)mxGetData(mx_dxtimageslice_rgb);
		auto g_data = (uint8_t*)mxGetData(mx_dxtimageslice_rgb) + raw_img->height * raw_img->width;
		auto b_data = (uint8_t*)mxGetData(mx_dxtimageslice_rgb) + 2 * raw_img->height * raw_img->width;
		for(i = 0; i < raw_img->height; i++)
		{
			for(j = 0; j < raw_img->width; j++)
			{
				dst_idx = i + j*raw_img->height;
				src_idx = (i*raw_img->width + j) * 4;
				r_data[dst_idx] = raw_img->pixels[src_idx];
				g_data[dst_idx] = raw_img->pixels[src_idx + 1];
				b_data[dst_idx] = raw_img->pixels[src_idx + 2];
			}
		}
	}
}

void DXTImage::WriteHDR(std::wstring &filename, std::wstring &ext, bool remove_idx_if_singular)
{
	size_t i, j, k;
	if(this->GetImageCount() > 1 || !remove_idx_if_singular)
	{
		const DirectX::TexMetadata &metadata = this->GetMetadata();
		size_t depth = metadata.depth;
		for(i = 0; i < metadata.mipLevels; i++)
		{
			for(j = 0; j < metadata.arraySize; j++)
			{
				for(k = 0; k < depth; k++)
				{
					std::wstring out_fn = filename + std::to_wstring(i).append(std::to_wstring(j)).append(std::to_wstring(k)).append(ext);
					hres = DirectX::SaveToHDRFile(*this->GetImage(i, j, k), out_fn.c_str());
					if(FAILED(hres))
					{
						MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "SaveToHDRFileError", "There was an error while saving the HDR file.");
					}
				}
			}
			if(depth > 1)
			{
				depth >>= 1u;
			}
		}
	}
	else
	{
		std::wstring out_fn = filename + ext;
		hres = DirectX::SaveToHDRFile(*this->GetImage(0, 0, 0), out_fn.c_str());
		if(FAILED(hres))
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "SaveToHDRFileError", "There was an error while saving the HDR file.");
		}
	}
}

void DXTImage::WriteHDR(std::wstring &filename, size_t mip, size_t item, size_t slice)
{
	hres = DirectX::SaveToHDRFile(*this->GetImage(mip, item, slice), filename.c_str());
	if(FAILED(hres))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "SaveToHDRFileError", "There was an error while saving the HDR file.");
	}
}

void DXTImage::WriteTGA(std::wstring &filename, std::wstring &ext, bool remove_idx_if_singular)
{
	size_t i, j, k;
	if(this->GetImageCount() > 1 || !remove_idx_if_singular)
	{
		const DirectX::TexMetadata &metadata = this->GetMetadata();
		size_t depth = metadata.depth;
		for(i = 0; i < metadata.mipLevels; i++)
		{
			for(j = 0; j < metadata.arraySize; j++)
			{
				for(k = 0; k < depth; k++)
				{
					std::wstring out_fn = filename + std::to_wstring(i).append(std::to_wstring(j)).append(std::to_wstring(k)).append(ext);
					hres = DirectX::SaveToTGAFile(*this->GetImage(i, j, k), out_fn.c_str());
					if(FAILED(hres))
					{
						MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "SaveToTGAFileError", "There was an error while saving the TGA file.");
					}
				}
			}
			if(depth > 1)
			{
				depth >>= 1u;
			}
		}
	}
	else
	{
		std::wstring out_fn = filename + ext;
		hres = DirectX::SaveToTGAFile(*this->GetImage(0, 0, 0), out_fn.c_str());
		if(FAILED(hres))
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "SaveToTGAFileError", "There was an error while saving the TGA file.");
		}
	}
}

void DXTImage::WriteTGA(std::wstring &filename, size_t mip, size_t item, size_t slice)
{
	hres = DirectX::SaveToTGAFile(*this->GetImage(mip, item, slice), filename.c_str());
	if(FAILED(hres))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "SaveToTGAFileError", "There was an error while saving the TGA file.");
	}
}
