#include "mex.h"
#include <string>
#include "dxtmex_dxtimagearray.hpp"
#include "dxtmex_mexerror.hpp"
#include "dxtmex_mexutils.hpp"
#include "dxtmex_maps.hpp"
#include "dxtmex_flags.hpp"
#include "dxtmex_pixel.hpp"

using namespace DXTMEX;


/* MATLAB:DXTImageSlice constructor */
DXTImage::DXTImage(const mxArray* mx_width, const mxArray* mx_height, const mxArray* mx_row_pitch, const mxArray* mx_slice_pitch, const mxArray* mx_pixels, const mxArray* mx_formatid, const mxArray* mx_flags)
{
	if((mx_width == nullptr) || (mx_height == nullptr) || (mx_row_pitch == nullptr) || (mx_slice_pitch == nullptr) || (mx_pixels == nullptr) || (mx_formatid == nullptr) || (mx_flags == nullptr))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER | MEU_SEVERITY_INTERNAL, "InvalidImportError", "An imported DXTImageSlice field was unexpectedly empty.");
	}
	this->_type = IMAGE_TYPE::UNKNOWN;
	this->_dds_flags = (DirectX::DDS_FLAGS) * (mxUint32*)mxGetData(mx_flags);

	const auto width = (size_t) * (mxUint64*)mxGetData(mx_width);
	const auto height = (size_t) * (mxUint64*)mxGetData(mx_height);
	const auto format = (DXGI_FORMAT) * (mxUint32*)mxGetData(mx_formatid);

	this->Initialize2D(format, width, height, 1, 1, this->GetFlags());

	DirectX::Image image = *this->GetImages();
	if(image.rowPitch != (size_t) * (mxUint64*)mxGetData(mx_row_pitch))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER | MEU_SEVERITY_INTERNAL, "InvalidImportError", "The imported row pitch of the DXTImageSlice differs from the expected row pitch.");
	}
	else if(image.slicePitch != (size_t) * (mxUint64*)mxGetData(mx_slice_pitch))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER | MEU_SEVERITY_INTERNAL, "InvalidImportError", "The imported slice pitch of the DXTImageSlice differs from the expected slice pitch.");
	}
	memcpy(image.pixels, mxGetData(mx_pixels), image.slicePitch * sizeof(mxUint8));
}

/* MATLAB:DXTImage constructor */
DXTImage::DXTImage(const mxArray* mx_metadata, const mxArray* mx_images) : _type(IMAGE_TYPE::UNKNOWN), _dds_flags(DirectX::DDS_FLAGS_NONE)
{
	if(mx_metadata == nullptr)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER | MEU_SEVERITY_INTERNAL, "NotEnoughInputsError", "DXTImage import metadata was unexpectedly empty.");
	}
	if(mx_images == nullptr)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER | MEU_SEVERITY_INTERNAL, "NotEnoughInputsError", "DXTImage import images was unexpectedly empty.");
	}
	DirectX::TexMetadata metadata = {};
	ImportMetadata(mx_metadata, metadata);
	this->SetFlags((DirectX::DDS_FLAGS) * (mxUint32*)mxGetData(mxGetField(mxGetField(mx_metadata, 0, "Flags"), 0, "Value")));
	const char* str_imtype = mxArrayToString(mxGetField(mx_metadata, 0, "Type"));
	this->SetImageType(g_imagetype_map.FindIDFromString(str_imtype));
	this->Initialize(metadata, this->GetFlags());
	ImportImages(mx_images, (DirectX::Image*)this->GetImages(), metadata.arraySize, metadata.mipLevels, metadata.depth, metadata.dimension);
}

void DXTImage::ImportMetadata(const mxArray* mx_metadata, DirectX::TexMetadata& metadata)
{
	metadata.width = (size_t) * (mxUint64*)mxGetData(mxGetField(mx_metadata, 0, "Width"));
	metadata.height = (size_t) * (mxUint64*)mxGetData(mxGetField(mx_metadata, 0, "Height"));
	metadata.depth = (size_t) * (mxUint64*)mxGetData(mxGetField(mx_metadata, 0, "Depth"));
	metadata.arraySize = (size_t) * (mxUint64*)mxGetData(mxGetField(mx_metadata, 0, "ArraySize"));
	metadata.mipLevels = (size_t) * (mxUint64*)mxGetData(mxGetField(mx_metadata, 0, "MipLevels"));
	metadata.miscFlags = (uint32_t) * (mxUint32*)mxGetData(mxGetField(mx_metadata, 0, "MiscFlags"));
	metadata.miscFlags2 = (uint32_t) * (mxUint32*)mxGetData(mxGetField(mx_metadata, 0, "MiscFlags2"));
	metadata.format = (DXGI_FORMAT) * (mxUint32*)mxGetData(mxGetField(mxGetField(mx_metadata, 0, "Format"), 0, "ID"));
	metadata.dimension = (DirectX::TEX_DIMENSION)(*(mxUint8*)mxGetData(mxGetField(mx_metadata, 0, "Dimension")) + 1);
}

void DXTImage::ImportImages(const mxArray * mx_images, DirectX::Image * images, size_t array_size, size_t mip_levels, size_t depth, DirectX::TEX_DIMENSION dimension)
{
	mwIndex i, j, src_idx, dst_idx;
	switch(dimension)
	{
		case DirectX::TEX_DIMENSION_TEXTURE1D:
		case DirectX::TEX_DIMENSION_TEXTURE2D:
		{
			for(i = 0; i < array_size; i++)
			{
				for(j = 0; j < mip_levels; j++)
				{
					src_idx = i + j * mip_levels;
					dst_idx = i * mip_levels + j;
					images[dst_idx].width = (size_t) * (mxUint64*)mxGetData(mxGetField(mx_images, src_idx, "Width"));
					images[dst_idx].height = (size_t) * (mxUint64*)mxGetData(mxGetField(mx_images, src_idx, "Height"));
					images[dst_idx].format = (DXGI_FORMAT) * (mxUint32*)mxGetData(mxGetField(mx_images, src_idx, "FormatID"));
					images[dst_idx].rowPitch = (size_t) * (mxUint64*)mxGetData(mxGetField(mx_images, src_idx, "RowPitch"));
					images[dst_idx].slicePitch = (size_t) * (mxUint64*)mxGetData(mxGetField(mx_images, src_idx, "SlicePitch"));
					memcpy(images[dst_idx].pixels, mxGetData(mxGetField(mx_images, src_idx, "Pixels")), images[dst_idx].slicePitch * sizeof(mxUint8));
				}
			}
			break;
		}
		case DirectX::TEX_DIMENSION_TEXTURE3D:
		{
			for(i = 0; i < depth * mip_levels; i++)
			{
				images[i].width = (size_t) * (mxUint64*)mxGetData(mxGetField(mx_images, i, "Width"));
				images[i].height = (size_t) * (mxUint64*)mxGetData(mxGetField(mx_images, i, "Height"));
				images[i].format = (DXGI_FORMAT) * (mxUint32*)mxGetData(mxGetField(mx_images, i, "FormatID"));
				images[i].rowPitch = (size_t) * (mxUint64*)mxGetData(mxGetField(mx_images, i, "RowPitch"));
				images[i].slicePitch = (size_t) * (mxUint64*)mxGetData(mxGetField(mx_images, i, "SlicePitch"));
				memcpy(images[i].pixels, mxGetData(mxGetField(mx_images, i, "Pixels")), images[i].slicePitch * sizeof(mxUint8));
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
mxArray* DXTImage::ExportMetadata(const DirectX::TexMetadata & metadata, DXTImage::IMAGE_TYPE type)
{
	const char* fieldnames[] = {"Type", "Width", "Height", "Depth", "ArraySize", "MipLevels", "MiscFlags", "MiscFlags2", "Format", "Dimension", "AlphaMode", "IsCubeMap", "IsPMAlpha", "IsVolumeMap", "Flags"};
	mxArray* mx_metadata = mxCreateStructMatrix(1, 1, ARRAYSIZE(fieldnames), fieldnames);
	mxSetField(mx_metadata, 0, "Type", mxCreateString(g_imagetype_map.FindStringFromID(type).c_str()));
	MEXUtils::SetScalarField(mx_metadata, 0, "Width", mxUINT64_CLASS, metadata.width);
	MEXUtils::SetScalarField(mx_metadata, 0, "Height", mxUINT64_CLASS, metadata.height);
	MEXUtils::SetScalarField(mx_metadata, 0, "Depth", mxUINT64_CLASS, metadata.depth);
	MEXUtils::SetScalarField(mx_metadata, 0, "ArraySize", mxUINT64_CLASS, metadata.arraySize);
	MEXUtils::SetScalarField(mx_metadata, 0, "MipLevels", mxUINT64_CLASS, metadata.mipLevels);
	MEXUtils::SetScalarField(mx_metadata, 0, "MiscFlags", mxUINT32_CLASS, metadata.miscFlags);
	MEXUtils::SetScalarField(mx_metadata, 0, "MiscFlags2", mxUINT32_CLASS, metadata.miscFlags2);
	mxSetField(mx_metadata, 0, "Format", ExportFormat(metadata.format));
	MEXUtils::SetScalarField(mx_metadata, 0, "Dimension", mxUINT8_CLASS, metadata.dimension - 1);
	mxSetField(mx_metadata, 0, "AlphaMode", mxCreateString(g_alphamode_map.FindStringFromID(metadata.GetAlphaMode()).c_str()));
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
	mxSetField(mx_metadata, 0, "Type", mxCreateString(g_imagetype_map.FindStringFromID(this->_type).c_str()));
	MEXUtils::SetScalarField(mx_metadata, 0, "Width", mxUINT64_CLASS, metadata.width);
	MEXUtils::SetScalarField(mx_metadata, 0, "Height", mxUINT64_CLASS, metadata.height);
	MEXUtils::SetScalarField(mx_metadata, 0, "Depth", mxUINT64_CLASS, metadata.depth);
	MEXUtils::SetScalarField(mx_metadata, 0, "ArraySize", mxUINT64_CLASS, metadata.arraySize);
	MEXUtils::SetScalarField(mx_metadata, 0, "MipLevels", mxUINT64_CLASS, metadata.mipLevels);
	MEXUtils::SetScalarField(mx_metadata, 0, "MiscFlags", mxUINT32_CLASS, metadata.miscFlags);
	MEXUtils::SetScalarField(mx_metadata, 0, "MiscFlags2", mxUINT32_CLASS, metadata.miscFlags2);
	mxSetField(mx_metadata, 0, "Format", ExportFormat(metadata.format));
	MEXUtils::SetScalarField(mx_metadata, 0, "Dimension", mxUINT8_CLASS, metadata.dimension - 1);
	mxSetField(mx_metadata, 0, "AlphaMode", mxCreateString(g_alphamode_map.FindStringFromID(metadata.GetAlphaMode()).c_str()));
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
	mxSetField(mx_fmt, 0, "Name", mxCreateString(g_format_map.FindStringFromID(fmt).c_str()));
	MEXUtils::SetScalarField(mx_fmt, 0, "ID", mxUINT32_CLASS, fmt);
	mxSetField(mx_fmt, 0, "IsValid", mxCreateLogicalScalar(DirectX::IsValid(fmt)));
	mxSetField(mx_fmt, 0, "IsCompressed", mxCreateLogicalScalar(DirectX::IsCompressed(fmt)));
	mxSetField(mx_fmt, 0, "IsPacked", mxCreateLogicalScalar(DirectX::IsPacked(fmt)));
	mxSetField(mx_fmt, 0, "IsVideo", mxCreateLogicalScalar(DirectX::IsVideo(fmt)));
	mxSetField(mx_fmt, 0, "IsPlanar", mxCreateLogicalScalar(DirectX::IsPlanar(fmt)));
	mxSetField(mx_fmt, 0, "IsPalettized", mxCreateLogicalScalar(DirectX::IsPalettized(fmt)));
	mxSetField(mx_fmt, 0, "IsDepthStencil", mxCreateLogicalScalar(DirectX::IsDepthStencil(fmt)));
	mxSetField(mx_fmt, 0, "IsSRGB", mxCreateLogicalScalar(DirectX::IsSRGB(fmt)));
	mxSetField(mx_fmt, 0, "IsTypeless", mxCreateLogicalScalar(DirectX::IsTypeless(fmt)));
	mxSetField(mx_fmt, 0, "HasAlpha", mxCreateLogicalScalar(DirectX::HasAlpha(fmt)));
	MEXUtils::SetScalarField(mx_fmt, 0, "BitsPerPixel", mxUINT64_CLASS, DirectX::BitsPerPixel(fmt));
	MEXUtils::SetScalarField(mx_fmt, 0, "BitsPerColor", mxUINT64_CLASS, DirectX::BitsPerColor(fmt));
	return mx_fmt;
}

mwIndex DXTImage::ComputeIndexMEX(size_t mip, size_t item, size_t slice)
{
	auto ret = size_t(-1);
	const DirectX::TexMetadata & metadata = this->GetMetadata();
	if(mip >= metadata.mipLevels)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_INTERNAL, "ImageIndexError", "The requested mipmap exceeds the number of mipmaps in the image array.");
	}

	switch(metadata.dimension)
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
			ret = item + mip * metadata.arraySize;
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
	mxArray* tmp, * mx_images;
	mwIndex i, j, k, dst_idx;
	const char* fieldnames[] = {"Width", "Height", "RowPitch", "SlicePitch", "Pixels", "FormatID", "FlagsValue"};
	DirectX::TexMetadata metadata = this->GetMetadata();
	size_t depth = metadata.depth;

	mx_images = mxCreateStructMatrix(std::max(metadata.arraySize, metadata.depth), metadata.mipLevels, ARRAYSIZE(fieldnames), fieldnames);
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
				memcpy(mxGetData(tmp), image->pixels, image->slicePitch * sizeof(mxUint8));
				mxSetField(mx_images, dst_idx, "Pixels", tmp);
			}
		}
		if(depth > 1)
		{
			depth >>= 1u;
		}
	}
	return mx_images;
}

void DXTImage::PrepareImages(DXTImage & out)
{
	DXGI_FORMAT srgb_fmt = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	const DirectX::TexMetadata metadata = this->GetMetadata();

	/* prepare the images */
	if(!DirectX::HasAlpha(metadata.format))
	{
		srgb_fmt = DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;
	}

	if(metadata.format == srgb_fmt)
	{
		out = std::move(*this);
		return;
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

void DXTImage::ToImage(mxArray * &mx_dxtimage_rgb, bool combine_alpha)
{
	DXTImage prepared_images;
	this->PrepareImages(prepared_images);
	prepared_images.ToImageMatrix(mx_dxtimage_rgb, combine_alpha);
}

void DXTImage::ToImage(mxArray * &mx_dxtimage_rgb, mxArray * &mx_dxtimage_a)
{
	DXTImage prepared_images;
	this->PrepareImages(prepared_images);
	prepared_images.ToImageMatrix(mx_dxtimage_rgb, mx_dxtimage_a);
}

void DXTImage::ToImageMatrix(mxArray * &mx_dxtimage_rgb, bool combine_alpha)
{
	size_t i, j, k;
	if(this->GetImageCount() == 1)
	{
		DXGIPixel matrix_constructor(this->GetMetadata().format, this->GetImage(0, 0, 0));
		if(combine_alpha)
		{
			matrix_constructor.ExtractRGBA(mx_dxtimage_rgb);
		}
		else
		{
			matrix_constructor.ExtractRGB(mx_dxtimage_rgb);
		}
	}
	else
	{
		mxArray* tmp_rgb;
		DirectX::TexMetadata metadata = this->GetMetadata();
		size_t depth = metadata.depth;
		mx_dxtimage_rgb = mxCreateCellMatrix(std::max(metadata.arraySize, metadata.depth), metadata.mipLevels);
		for(i = 0; i < metadata.mipLevels; i++)
		{
			for(j = 0; j < metadata.arraySize; j++)
			{
				for(k = 0; k < depth; k++)
				{
					DXGIPixel matrix_constructor(metadata.format, this->GetImage(i, j, k));
					if(combine_alpha)
					{
						matrix_constructor.ExtractRGBA(tmp_rgb);
					}
					else
					{
						matrix_constructor.ExtractRGB(tmp_rgb);
					}
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

void DXTImage::ToImageMatrix(mxArray * &mx_dxtimage_rgb, mxArray * &mx_dxtimage_a)
{
	size_t i, j, k;
	if(this->GetImageCount() == 1)
	{
		DXGIPixel matrix_constructor(this->GetMetadata().format, this->GetImage(0, 0, 0));
		matrix_constructor.ExtractRGBA(mx_dxtimage_rgb, mx_dxtimage_a);
	}
	else
	{
		mxArray* tmp_rgb, * tmp_a;
		DirectX::TexMetadata metadata = this->GetMetadata();
		size_t depth = metadata.depth;
		mx_dxtimage_rgb = mxCreateCellMatrix(std::max(metadata.arraySize, metadata.depth), metadata.mipLevels);
		mx_dxtimage_a = mxCreateCellMatrix(std::max(metadata.arraySize, metadata.depth), metadata.mipLevels);
		for(i = 0; i < metadata.mipLevels; i++)
		{
			for(j = 0; j < metadata.arraySize; j++)
			{
				for(k = 0; k < depth; k++)
				{
					DXGIPixel matrix_constructor(metadata.format, this->GetImage(i, j, k));
					matrix_constructor.ExtractRGBA(tmp_rgb, tmp_a);
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


void DXTImage::ToMatrix(mxArray * &mx_dxtimage_out)
{
	size_t i, j, k;
	if(this->GetImageCount() == 1)
	{
		DXGIPixel matrix_constructor(this->GetMetadata().format, this->GetImage(0, 0, 0));
		matrix_constructor.ExtractAll(mx_dxtimage_out);
	}
	else
	{
		mxArray* tmp_out;
		DirectX::TexMetadata metadata = this->GetMetadata();
		size_t depth = metadata.depth;
		mx_dxtimage_out = mxCreateCellMatrix(std::max(metadata.arraySize, metadata.depth), metadata.mipLevels);
		for(i = 0; i < metadata.mipLevels; i++)
		{
			for(j = 0; j < metadata.arraySize; j++)
			{
				for(k = 0; k < depth; k++)
				{
					DXGIPixel matrix_constructor(metadata.format, this->GetImage(i, j, k));
					matrix_constructor.ExtractAll(tmp_out);
					mxSetCell(mx_dxtimage_out, this->ComputeIndexMEX(i, j, k), tmp_out);
				}
			}
			if(depth > 1)
			{
				depth >>= 1u;
			}
		}
	}
}


void DXTImage::WriteHDR(const std::wstring & filename, std::wstring & ext, bool remove_idx_if_singular)
{
	size_t i, j, k;
	if(this->GetImageCount() > 1 || !remove_idx_if_singular)
	{
		const DirectX::TexMetadata& metadata = this->GetMetadata();
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

void DXTImage::WriteHDR(const std::wstring & filename, size_t mip, size_t item, size_t slice)
{
	hres = DirectX::SaveToHDRFile(*this->GetImage(mip, item, slice), filename.c_str());
	if(FAILED(hres))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "SaveToHDRFileError", "There was an error while saving the HDR file.");
	}
}

void DXTImage::WriteTGA(const std::wstring & filename, std::wstring & ext, bool remove_idx_if_singular)
{
	size_t i, j, k;
	if(this->GetImageCount() > 1 || !remove_idx_if_singular)
	{
		const DirectX::TexMetadata& metadata = this->GetMetadata();
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

void DXTImage::WriteTGA(const std::wstring & filename, size_t mip, size_t item, size_t slice)
{
	hres = DirectX::SaveToTGAFile(*this->GetImage(mip, item, slice), filename.c_str());
	if(FAILED(hres))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "SaveToTGAFileError", "There was an error while saving the TGA file.");
	}
}

DirectX::TexMetadata
MEXToDXT::DeriveMetadata(const mxArray * data_in,
	COLORSPACE input_colorspace,
	DirectX::TEX_ALPHA_MODE alpha_mode,
	bool is_cubemap)
{
	mxClassID class_id = mxGetClassID(data_in);
	mwSize num_dims = mxGetNumberOfDimensions(data_in);
	const mwSize* dims = mxGetDimensions(data_in);

	if(class_id == mxCELL_CLASS)
	{
		if(mxIsEmpty(data_in))
		{
			MEXError::PrintMexError(MEU_FL,
				MEU_SEVERITY_USER,
				"InvalidImportError",
				"Cell array must not be empty");
		}
		else if(!mxIsNumeric(mxGetCell(data_in, 0)) && !mxIsLogical(mxGetCell(data_in, 0)))
		{
			MEXError::PrintMexError(MEU_FL,
				MEU_SEVERITY_USER,
				"InvalidImportError",
				"Cell array contents must be numeric or logical");
		}

		DirectX::TexMetadata metadata = DeriveMetadata(mxGetCell(data_in, 0), input_colorspace, alpha_mode, is_cubemap);

		if(num_dims != 2)
		{
			MEXError::PrintMexError(MEU_FL,
				MEU_SEVERITY_USER,
				"InvalidImportError",
				"Cell array must have two dimensions");
		}

		metadata.arraySize = dims[0];
		metadata.mipLevels = dims[1];
		return metadata;
	}
	else
	{

		DirectX::TexMetadata metadata = {};
		size_t num_channels = 0;

		/* these will be reset if this was actually in a cell array */
		metadata.arraySize = 1;
		metadata.mipLevels = 1;

		/* these won't */
		metadata.miscFlags = is_cubemap ? DirectX::TEX_MISC_FLAG::TEX_MISC_TEXTURECUBE : 0;
		metadata.miscFlags2 = alpha_mode;

		switch(num_dims)
		{
			case 0:
			{
				/* I don't think this is possible */
				MEXError::PrintMexError(MEU_FL,
					MEU_SEVERITY_USER,
					"InvalidImportError",
					"Not enough dimensions.");
				break;
			}
			case 1:
			{
				/* 1D case (don't think this is possible) */
				metadata.width = mxGetNumberOfElements(data_in);
				metadata.height = 1;
				metadata.depth = 1;
				metadata.dimension = DirectX::TEX_DIMENSION_TEXTURE1D;
				num_channels = 1;
				break;
			}
			case 2:
			{
				/* 2D greyscale case */
				metadata.width = dims[1];
				metadata.height = dims[0];
				metadata.depth = 1;
				metadata.dimension = DirectX::TEX_DIMENSION_TEXTURE2D;
				num_channels = 1;
				break;
			}
			case 3:
			{
				/* 2D case (maybe still greyscale if 3rd dimension is 1) */
				metadata.width = dims[1];
				metadata.height = dims[0];
				metadata.depth = 1;
				metadata.dimension = DirectX::TEX_DIMENSION_TEXTURE2D;
				num_channels = dims[2];
				break;
			}
			case 4:
			{
				/* 3D case */
				metadata.width = dims[1];
				metadata.height = dims[0];
				metadata.depth = dims[3];
				metadata.dimension = DirectX::TEX_DIMENSION_TEXTURE3D;
				num_channels = dims[2];
				break;
			}
			default:
			{
				MEXError::PrintMexError(MEU_FL,
					MEU_SEVERITY_USER,
					"InvalidImportError",
					"Too many dimensions.");
			}
		}

		/* now need to get the intermediate format */

		if(num_channels < 1 || 4 < num_channels)
		{
			MEXError::PrintMexError(MEU_FL,
				MEU_SEVERITY_USER,
				"InvalidImportError",
				"The size of the third dimension must be between 1 and 4.");
		}

		switch(class_id)
		{
			case mxLOGICAL_CLASS:
			{
				switch(num_channels)
				{
					case 1:  metadata.format = DXGI_FORMAT_R1_UNORM; break;
					case 2:
					case 3:
					case 4:
					default:
					{
						MEXError::PrintMexError(MEU_FL,
							MEU_SEVERITY_USER,
							"InvalidImportError",
							"Logical input cannot have more than one channel.");
					}
				}
				break;
			}
			case mxINT32_CLASS:  /* convert to unsigned */
			case mxUINT32_CLASS:
			case mxDOUBLE_CLASS: /* narrowing */
			case mxSINGLE_CLASS:
			{
				switch(num_channels)
				{
					case 1:  metadata.format = DXGI_FORMAT_R32_FLOAT; break;
					case 2:  metadata.format = DXGI_FORMAT_R32G32_FLOAT; break;
					case 3:  metadata.format = DXGI_FORMAT_R32G32B32_FLOAT; break;
					case 4:
					default: metadata.format = DXGI_FORMAT_R32G32B32A32_FLOAT; break;
				}
				break;
			}
			case mxINT8_CLASS: /* convert to unsigned */
			case mxUINT8_CLASS:
			{
				switch(num_channels)
				{
					case 1:  metadata.format = DXGI_FORMAT_R8_UNORM; break;
					case 2:  metadata.format = DXGI_FORMAT_R8G8_UNORM; break;
					case 3:  /* NEEDS ALPHA */
					case 4:
					default:
					{
						switch(input_colorspace)
						{
							case COLORSPACE::SRGB:   metadata.format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; break;
							case COLORSPACE::LINEAR: metadata.format = DXGI_FORMAT_R8G8B8A8_UNORM; break;
							default:
							{
								MEXError::PrintMexError(MEU_FL,
									MEU_SEVERITY_USER,
									"InvalidImportError",
									"Unexpected value for the input colorspace (%d).", input_colorspace);
							}
						}
					}
				}
				break;
			}
			case mxINT16_CLASS: /* convert to unsigned */
			case mxUINT16_CLASS:
			{
				switch(num_channels)
				{
					case 1:  metadata.format = DXGI_FORMAT_R16_UNORM; break;
					case 2:  metadata.format = DXGI_FORMAT_R16G16_UNORM; break;
					case 3:  /* NEEDS ALPHA */
					case 4:
					default: metadata.format = DXGI_FORMAT_R16G16B16A16_UNORM; break;
				}
				break;
			}
			default:
			{
				MEXError::PrintMexError(MEU_FL,
					MEU_SEVERITY_USER,
					"InvalidImportError",
					"Unexpected class '%s'.", mxGetClassName(data_in));
			}
		}

		return metadata;

	}


}


void
MEXToDXT::ConvertToOutput(DXGI_FORMAT fmt_out,
	DirectX::TEX_FILTER_FLAGS filter_flags,
	float threshold,
	DirectX::ScratchImage& scimg_out,
	const mxArray* data_in,
	COLORSPACE input_colorspace,
	DirectX::TEX_ALPHA_MODE alpha_mode,
	bool is_cubemap,
	DirectX::CP_FLAGS cp_flags)
{
	if(fmt_out == DXGI_FORMAT_UNKNOWN)
	{
		ConvertToIntermediate(scimg_out, data_in, input_colorspace, alpha_mode, is_cubemap, cp_flags);
	}
	else
	{
		DirectX::ScratchImage tmp;
		ConvertToIntermediate(tmp, data_in, input_colorspace, alpha_mode, is_cubemap, cp_flags);
		hres = DirectX::Convert(tmp.GetImages(), tmp.GetImageCount(), tmp.GetMetadata(), fmt_out, filter_flags, threshold, scimg_out);
		if(FAILED(hres))
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "ConversionError", "There was an error while converting the image.");
		}
	}
}


void
MEXToDXT::ConvertToIntermediate(DirectX::ScratchImage& scimg_out,
                                const mxArray * data_in,
                                COLORSPACE input_colorspace,
                                DirectX::TEX_ALPHA_MODE alpha_mode,
                                bool is_cubemap,
                                DirectX::CP_FLAGS flags)
{

	/* options:
	 * input is linear/SRGB/adobe
	 * raw copy
	 *
	 * if SRGB then must be mxUint8, mxUint16, mxUint32, Single, or Double
	 *
	 * by default:
	 * mxUint8  => DXGI_FORMAT_R8G8B8A8_UNORM_SRGB (direct no-op conversion)
	 * mxUint16 => LINEAR FLOAT => DXGI_FORMAT_R16G16B16A16_UNORM
	 * mxUint32 => LINEAR FLOAT => DXGI_FORMAT_R32G32B32A32_UNORM
	 *
	 * mxInt8   => LINEAR FLOAT => DXGI_FORMAT_R8G8B8A8_SNORM
	 * mxInt16  => LINEAR FLOAT => DXGI_FORMAT_R16G16B16A16_SNORM
	 * mxInt32  => LINEAR FLOAT => DXGI_FORMAT_R32G32B32A32_SNORM
	 *
	 * mxSingle => DXGI_FORMAT_R32G32B32A32_FLOAT
	 * mxDouble => DXGI_FORMAT_R32G32B32A32_FLOAT (loss of precision)
	 *
	 * if linear do direct conversions (these also apply for above after converting to intermediate):
	 * mxUint8  => DXGI_FORMAT_R8G8B8A8_UNORM
	 * mxUint16 => DXGI_FORMAT_R16G16B16A16_UNORM
	 * mxUint32 => DXGI_FORMAT_R32G32B32A32_UNORM
	 *
	 * mxInt8   => DXGI_FORMAT_R8G8B8A8_SNORM
	 * mxInt16  => DXGI_FORMAT_R16G16B16A16_SNORM
	 * mxInt32  => DXGI_FORMAT_R32G32B32A32_SNORM
	 *
	 * mxSingle => DXGI_FORMAT_R32G32B32A32_FLOAT
	 * mxDouble => DXGI_FORMAT_R32G32B32A32_FLOAT (loss of precision)
	 */

	 /*
	  * image arrays are created from cell arrays with non-singular first dimensions
	  * mipmaps are from cell arrays with non-singular second dimensions
	  * volume maps are from numeric array with non-singular 4th dimensions
	  *
	  * should support any combination of the three and let DirectXTex decide if the input is ok
	  */

	const DirectX::TexMetadata metadata = DeriveMetadata(data_in, input_colorspace, alpha_mode, is_cubemap);
	hres = scimg_out.Initialize(metadata, flags);
	if(FAILED(hres))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "InitializationError", "There was an error while saving initializing the image.");
	}

	if(mxIsCell(data_in))
	{
		const mwSize* dims = mxGetDimensions(data_in);

		size_t depth = metadata.depth;
		for(size_t i = 0; i < metadata.mipLevels; i++)
		{
			for(size_t j = 0; j < metadata.arraySize; j++)
			{
				size_t src_idx = j + i * dims[0];
				mxArray* cell_data = mxGetCell(data_in, src_idx);
				if(cell_data == nullptr)
				{
					MEXError::PrintMexError(MEU_FL,
						MEU_SEVERITY_USER,
						"InvalidImportError",
						"Input cell array is incorrectly sized.");
				}

				DoConversion(const_cast<DirectX::Image*>(scimg_out.GetImage(i, j, 0)), cell_data, depth, input_colorspace);

			}
			if(depth > 1)
			{
				depth >>= 1u;
			}
		}
	}
	else
	{
		DoConversion(const_cast<DirectX::Image*>(scimg_out.GetImage(0, 0, 0)), data_in, 1, input_colorspace);
	}

	/* now make sure our data is correctly sized */

}

template <typename MX_TYPE, typename DXT_TYPE, int NCHANNELS>
struct MEXToDXT::Converter<MX_TYPE, DXT_TYPE, NCHANNELS, MEXToDXT::COLORSPACE::LINEAR>
{
	static void ToIntermediate(uint8_t* in_data, DirectX::Image* out_img)
	{
		/* direct copy to image */
		auto in_ptr = reinterpret_cast<MX_TYPE*>(in_data);
		auto out_ptr = reinterpret_cast<DXT_TYPE*>(out_img->pixels);
		size_t num_pixels = out_img->height * out_img->width;
		for(size_t src_idx = 0, dst_idx = 0; dst_idx < num_pixels; dst_idx++, src_idx += out_img->height)
		{
			if(src_idx >= num_pixels)
			{
				src_idx = dst_idx / out_img->width;
			}

			for(int j = 0; j < NCHANNELS; j++)
			{
				out_ptr[(dst_idx * NCHANNELS) + j] = static_cast<DXT_TYPE>(in_ptr[src_idx + (j * num_pixels)]);
			}
		}
	}
};


template <typename MX_TYPE>
struct MEXToDXT::Converter<MX_TYPE, uint16_t, 3, MEXToDXT::COLORSPACE::LINEAR>
{
	static void ToIntermediate(uint8_t* in_data, DirectX::Image* out_img)
	{
		/* direct copy to image */
		auto in_ptr = reinterpret_cast<MX_TYPE*>(in_data);
		auto out_ptr = reinterpret_cast<uint16_t*>(out_img->pixels);
		size_t num_pixels = out_img->height * out_img->width;
		for(size_t src_idx = 0, dst_idx = 0; dst_idx < num_pixels; dst_idx++, src_idx += out_img->height)
		{
			if(src_idx >= num_pixels)
			{
				src_idx = dst_idx / out_img->width;
			}
			for(int j = 0; j < 3; j++)
			{
				out_ptr[(dst_idx * 4) + j] = static_cast<uint16_t>(static_cast<int32_t>(in_ptr[src_idx + (j * num_pixels)]) - std::numeric_limits<int32_t>::min());
			}
			out_ptr[(dst_idx * 4) + 3] = std::numeric_limits<uint16_t>::max(); /* set missing alpha */
		}
	}
};


template <typename MX_TYPE, int NCHANNELS>
struct MEXToDXT::Converter<MX_TYPE, float, NCHANNELS, MEXToDXT::COLORSPACE::LINEAR>
{
	static void ToIntermediate(uint8_t* in_data, DirectX::Image* out_img)
	{
		/* covert to float then to output format */
		auto in_ptr = reinterpret_cast<MX_TYPE*>(in_data);
		auto out_ptr = reinterpret_cast<float*>(out_img->pixels);
		const size_t num_pixels = out_img->height * out_img->width;
		for(size_t src_idx = 0, dst_idx = 0; dst_idx < num_pixels; dst_idx++, src_idx += out_img->height)
		{
			if(src_idx >= num_pixels)
			{
				src_idx = dst_idx / out_img->width;
			}

			for(int j = 0; j < NCHANNELS; j++)
			{
				out_ptr[(dst_idx * NCHANNELS) + j] = static_cast<float>(in_ptr[src_idx + (j * num_pixels)]);
			}
		}
	}
};


template <typename MX_TYPE, typename DXT_TYPE, int NCHANNELS>
struct MEXToDXT::Converter<MX_TYPE, DXT_TYPE, NCHANNELS, MEXToDXT::COLORSPACE::SRGB>
{
	static void ToIntermediate(uint8_t* in_data, DirectX::Image* out_img)
	{
		/* covert to float then to output format */
		auto in_ptr = reinterpret_cast<MX_TYPE*>(in_data);
		auto out_ptr = reinterpret_cast<DXT_TYPE*>(out_img->pixels);
		const size_t num_pixels = out_img->height * out_img->width;
		for(size_t src_idx = 0, dst_idx = 0; dst_idx < num_pixels; dst_idx++, src_idx += out_img->height)
		{
			if(src_idx >= num_pixels)
			{
				src_idx = dst_idx / out_img->width;
			}

			for(int j = 0; j < NCHANNELS; j++)
			{
				float c = DXGIPixel::SRGBToLinearFloat(in_ptr[src_idx + (j * num_pixels)]);
				out_ptr[(dst_idx * NCHANNELS) + j] = DXGIPixel::LinearFloatToUNORM<DXT_TYPE>(c);
			}
		}
	}
};


template <int NCHANNELS>
struct MEXToDXT::Converter<mxUint8, uint8_t, NCHANNELS, MEXToDXT::COLORSPACE::SRGB>
{
	static void ToIntermediate(uint8_t* in_data, DirectX::Image* out_img)
	{
		/* copy to DXGI_FORMAT_R8G8B8A8_SRGB */
		size_t num_pixels = out_img->height * out_img->width;
		for(size_t src_idx = 0, dst_idx = 0; dst_idx < num_pixels; dst_idx++, src_idx += out_img->height)
		{
			if(src_idx >= num_pixels)
			{
				src_idx = dst_idx / out_img->width;
			}
			for(int j = 0; j < NCHANNELS; j++)
			{
				out_img->pixels[(dst_idx * NCHANNELS) + j] = in_data[src_idx + (j * num_pixels)];
			}
		}
	}
};


template <>
struct MEXToDXT::Converter<mxUint8, uint8_t, 3, MEXToDXT::COLORSPACE::SRGB>
{
	static void ToIntermediate(uint8_t* in_data, DirectX::Image* out_img)
	{
		/* copy to DXGI_FORMAT_R8G8B8A8_SRGB */
		size_t num_pixels = out_img->height * out_img->width;
		for(size_t src_idx = 0, dst_idx = 0; dst_idx < num_pixels; dst_idx++, src_idx += out_img->height)
		{
			if(src_idx >= num_pixels)
			{
				src_idx = dst_idx / out_img->width;
			}
			for(int j = 0; j < 3; j++)
			{
				out_img->pixels[(dst_idx * 4) + j] = in_data[src_idx + (j * num_pixels)];
			}
			out_img->pixels[(dst_idx * 4) + 3] = std::numeric_limits<uint8_t>::max(); /* set missing alpha */
		}
	}
};


template <int NCHANNELS>
struct MEXToDXT::Converter<mxInt8, uint8_t, NCHANNELS, MEXToDXT::COLORSPACE::SRGB>
{
	/* nearly direct conversion---just shift everything over by 0x80 */
	static void ToIntermediate(uint8_t* in_data, DirectX::Image* out_img)
	{
		auto in_ptr = reinterpret_cast<mxInt8*>(in_data);
		auto out_ptr = reinterpret_cast<uint8_t*>(out_img->pixels);
		/* copy to DXGI_FORMAT_R8G8B8A8_SRGB */
		size_t num_pixels = out_img->height * out_img->width;
		for(size_t src_idx = 0, dst_idx = 0; dst_idx < num_pixels; dst_idx++, src_idx += out_img->height)
		{
			if(src_idx >= num_pixels)
			{
				src_idx = dst_idx / out_img->width;
			}
			for(int j = 0; j < NCHANNELS; j++)
			{
				out_ptr[(dst_idx * NCHANNELS) + j] = static_cast<uint8_T>(
					static_cast<int16_t>(in_ptr[src_idx + (j * num_pixels)]) - static_cast<int16_t>(std::numeric_limits<mxInt8>::min())
					);
			}
		}
	}
};


template <>
struct MEXToDXT::Converter<mxInt8, uint8_t, 3, MEXToDXT::COLORSPACE::SRGB>
{
	static void ToIntermediate(uint8_t* in_data, DirectX::Image* out_img)
	{
		auto in_ptr = reinterpret_cast<mxInt8*>(in_data);
		auto out_ptr = reinterpret_cast<uint8_t*>(out_img->pixels);
		/* copy to DXGI_FORMAT_R8G8B8A8_SRGB */
		size_t num_pixels = out_img->height * out_img->width;
		for(size_t src_idx = 0, dst_idx = 0; dst_idx < num_pixels; dst_idx++, src_idx += out_img->height)
		{
			if(src_idx >= num_pixels)
			{
				src_idx = dst_idx / out_img->width;
			}
			for(int j = 0; j < 3; j++)
			{
				out_ptr[(dst_idx * 4) + j] = static_cast<uint8_T>(
					static_cast<int16_t>(in_ptr[src_idx + (j * num_pixels)]) - static_cast<int16_t>(std::numeric_limits<mxInt8>::min())
					);
			}
			out_img->pixels[(dst_idx * 4) + 3] = std::numeric_limits<uint8_t>::max(); /* set missing alpha */
		}
	}
};


template <>
struct MEXToDXT::Converter<mxLogical, uint8_t, 1, MEXToDXT::COLORSPACE::SRGB>
{
	static void ToIntermediate(uint8_t* in_data, DirectX::Image* out_img)
	{
		auto in_ptr = reinterpret_cast<mxLogical*>(in_data);
		auto out_ptr = reinterpret_cast<uint8_t*>(out_img->pixels);
		/* copy to DXGI_FORMAT_R1_UNORM */
		for(size_t col_idx = 0; col_idx < out_img->height; col_idx++)
		{
			for(size_t row_idx = 0; row_idx < out_img->rowPitch - 1; row_idx++)
			{
				uint8_t val = 0;
				for(uint32_t bit_idx = 8; bit_idx > 0; bit_idx--)
				{
					uint32_t mask = 1u << (bit_idx - 1);
					/* cast to make sure we don't upcast to int */
					val |= ((static_cast<uint32_t>(in_ptr[row_idx * out_img->height + col_idx] != 0) << (bit_idx - 1))) & mask;
				}
				out_ptr[row_idx + col_idx * out_img->width] = val;
			}
			uint8_t val = 0;
			for(uint32_t bit_idx = static_cast<uint32_t>(out_img->width - ((out_img->rowPitch - 1) * 8)); bit_idx > 0; bit_idx--)
			{
				uint32_t mask = 1u << (bit_idx - 1);
				/* cast to make sure we don't upcast to int */
				val |= ((static_cast<uint32_t>(in_ptr[(out_img->rowPitch - 1) * out_img->height + col_idx] != 0) << (bit_idx - 1))) & mask;
			}
			out_ptr[(out_img->rowPitch - 1) + col_idx * out_img->width] = val;
		}
	}
};


template <typename MX_TYPE>
struct MEXToDXT::Converter<MX_TYPE, uint16_t, 3, MEXToDXT::COLORSPACE::SRGB>
{
	static void ToIntermediate(uint8_t* in_data, DirectX::Image* out_img)
	{
		/* direct copy to image */
		auto in_ptr = reinterpret_cast<MX_TYPE*>(in_data);
		auto out_ptr = reinterpret_cast<uint16_t*>(out_img->pixels);
		size_t num_pixels = out_img->height * out_img->width;
		for(size_t src_idx = 0, dst_idx = 0; dst_idx < num_pixels; dst_idx++, src_idx += out_img->height)
		{
			if(src_idx >= num_pixels)
			{
				src_idx = dst_idx / out_img->width;
			}

			for(int j = 0; j < 3; j++)
			{
				float c = DXGIPixel::SRGBToLinearFloat(in_ptr[src_idx + (j * num_pixels)]);
				out_ptr[(dst_idx * 4) + j] = DXGIPixel::LinearFloatToUNORM<uint16_t>(c);
			}
			out_ptr[(dst_idx * 4) + 3] = std::numeric_limits<uint16_t>::max(); /* set missing alpha */
		}
	}
};


template <typename MX_TYPE, int NCHANNELS>
struct MEXToDXT::Converter<MX_TYPE, float, NCHANNELS, MEXToDXT::COLORSPACE::SRGB>
{
	static void ToIntermediate(uint8_t* in_data, DirectX::Image* out_img)
	{
		/* covert to float then to output format */
		auto in_ptr = reinterpret_cast<MX_TYPE*>(in_data);
		auto out_ptr = reinterpret_cast<float*>(out_img->pixels);
		const size_t num_pixels = out_img->height * out_img->width;
		for(size_t src_idx = 0, dst_idx = 0; dst_idx < num_pixels; dst_idx++, src_idx += out_img->height)
		{
			if(src_idx >= num_pixels)
			{
				src_idx = dst_idx / out_img->width;
			}

			for(int j = 0; j < NCHANNELS; j++)
			{
				out_ptr[(dst_idx * NCHANNELS) + j] = DXGIPixel::SRGBToLinearFloat(in_ptr[src_idx + (j * num_pixels)]);
			}
		}
	}
};


MEXToDXT::f_toir MEXToDXT::GetToIRFunction(mxClassID class_id, int num_channels, COLORSPACE input_colorspace)
{
	switch(class_id)
	{
		case mxLOGICAL_CLASS: return Converter<mxLogical, uint8_t, 1, COLORSPACE::LINEAR>::ToIntermediate;
		case mxDOUBLE_CLASS:  return GetToIRFunction<mxDouble, float>(num_channels, input_colorspace);
		case mxSINGLE_CLASS:  return GetToIRFunction<mxSingle, float>(num_channels, input_colorspace);
		case mxINT8_CLASS:    return GetToIRFunction<mxInt8,   uint8_t>(num_channels, input_colorspace);
		case mxUINT8_CLASS:   return GetToIRFunction<mxUint8,  uint8_t>(num_channels, input_colorspace);
		case mxINT16_CLASS:   return GetToIRFunction<mxInt16,  uint16_t>(num_channels, input_colorspace);
		case mxUINT16_CLASS:  return GetToIRFunction<mxUint16, uint16_t>(num_channels, input_colorspace);
		case mxINT32_CLASS:   return GetToIRFunction<mxInt32,  uint32_t>(num_channels, input_colorspace);
		case mxUINT32_CLASS:  return GetToIRFunction<mxUint32, uint32_t>(num_channels, input_colorspace);
		default:
		{
			MEXError::PrintMexError(MEU_FL,
				MEU_SEVERITY_USER,
				"InvalidImportError",
				"Input array was of unexpected class (%d).", class_id);
			return nullptr;
		}
	}
}


void MEXToDXT::DoConversion(DirectX::Image* img_out, const mxArray* data_in, size_t depth, COLORSPACE input_colorspace)
{
	mxClassID class_id = mxGetClassID(data_in);
	const mwSize num_dims = mxGetNumberOfDimensions(data_in);
	const mwSize* dims = mxGetDimensions(data_in);

	if(depth > 1)
	{
		if(num_dims != 4 || dims[3] != depth)
		{
			MEXError::PrintMexError(MEU_FL,
				MEU_SEVERITY_USER,
				"InvalidImportError",
				"Input array is incorrectly sized.");
		}
	}

	size_t in_slicepitch = dims[0] * dims[1] * mxGetElementSize(data_in);
	int num_channels = 1;
	if(num_dims > 2)
	{
		if(dims[2] == 3 && (mxGetElementSize(data_in) == 1 || mxGetElementSize(data_in) == 2))
		{
			/* in these cases our intermediate has four elements while the input has 3 */
			if(in_slicepitch * 4 != img_out->slicePitch)
			{
				MEXError::PrintMexError(MEU_FL,
					MEU_SEVERITY_USER,
					"InvalidImportError",
					"Input array is incorrectly sized.");
			}
		}
		else if(in_slicepitch * dims[2] != img_out->slicePitch)
		{
			MEXError::PrintMexError(MEU_FL,
				MEU_SEVERITY_USER,
				"InvalidImportError",
				"Input array is incorrectly sized.");
		}
		in_slicepitch *= dims[2];
		num_channels = static_cast<int>(dims[2]);
	}
	else
	{
		if(in_slicepitch != img_out->slicePitch)
		{
			MEXError::PrintMexError(MEU_FL,
				MEU_SEVERITY_USER,
				"InvalidImportError",
				"Input array is incorrectly sized.");
		}
		num_channels = 1;
	}

	f_toir toir = GetToIRFunction(class_id, num_channels, input_colorspace);

	auto ptr = reinterpret_cast<uint8_t*>(mxGetData(data_in));
	for(size_t i = 0; i < depth; i++, ptr += in_slicepitch, img_out++)
	{
		toir(ptr, img_out);
	}
}
