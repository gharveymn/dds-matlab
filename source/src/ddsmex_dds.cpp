#include "mex.h"
#include "ddsmex_maps.hpp"
#include "ddsmex_dds.hpp"
#include "ddsmex_mexerror.hpp"
#include "ddsmex_mexutils.hpp"

using namespace DDSMEX;

DDS::DDS(const mxArray* mx_metadata, const mxArray* mx_images)
{
	// Import metadata first, then determine which Initialize function to use
	DirectX::TexMetadata metadata = {};
	ImportMetadata(mx_metadata, metadata);
	this->_flags = (DWORD)*(uint32_T*)mxGetData(mxGetField(mx_metadata, 0, "Flags"));
	this->Initialize(metadata, this->_flags);
	ImportImages(mx_images, (DirectX::Image*)this->GetImages(), metadata.arraySize, metadata.mipLevels, metadata.depth, metadata.dimension);
}

void DDS::ImportMetadata(const mxArray* mx_metadata, DirectX::TexMetadata& metadata)
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

void DDS::ImportImages(const mxArray* mx_images, DirectX::Image* images, size_t array_size, size_t mip_levels, size_t depth, DirectX::TEX_DIMENSION dimension)
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

mxArray* DDS::ExportMetadata()
{
	return ExportMetadata(this->GetMetadata(), this->_flags);
}

mxArray* DDS::ExportMetadata(const DirectX::TexMetadata& metadata)
{
	const char* fieldnames[] = {"Width", "Height", "Depth", "ArraySize", "MipLevels", "MiscFlags", "MiscFlags2", "Format", "Dimension", "IsCubeMap", "IsPMAlpha", "IsVolumeMap"};
	mxArray* mx_metadata = mxCreateStructMatrix(1, 1, 12, fieldnames);
	MEXUtils::SetScalarField(mx_metadata, 0, "Width", mxUINT64_CLASS, metadata.width);
	MEXUtils::SetScalarField(mx_metadata, 0, "Height", mxUINT64_CLASS, metadata.height);
	MEXUtils::SetScalarField(mx_metadata, 0, "Depth", mxUINT64_CLASS, metadata.depth);
	MEXUtils::SetScalarField(mx_metadata, 0, "ArraySize", mxUINT64_CLASS, metadata.arraySize);
	MEXUtils::SetScalarField(mx_metadata, 0, "MipLevels", mxUINT64_CLASS, metadata.mipLevels);
	MEXUtils::SetScalarField(mx_metadata, 0, "MiscFlags", mxUINT32_CLASS, metadata.miscFlags);
	MEXUtils::SetScalarField(mx_metadata, 0, "MiscFlags2", mxUINT32_CLASS, metadata.miscFlags2);
	mxSetField(mx_metadata, 0, "Format", ExportFormat(metadata.format));
	MEXUtils::SetScalarField(mx_metadata, 0, "Dimension", mxUINT8_CLASS, metadata.dimension - 1);
	mxSetField(mx_metadata, 0, "IsCubeMap", mxCreateLogicalScalar(metadata.IsCubemap()));
	mxSetField(mx_metadata, 0, "IsPMAlpha", mxCreateLogicalScalar(metadata.IsPMAlpha()));
	mxSetField(mx_metadata, 0, "IsVolumeMap", mxCreateLogicalScalar(metadata.IsVolumemap()));
	return mx_metadata;
}

mxArray* DDS::ExportMetadata(const DirectX::TexMetadata& metadata, DWORD flags)
{
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
	MEXUtils::SetScalarField(mx_metadata, 0, "Flags", mxUINT32_CLASS, flags);
	mxSetField(mx_metadata, 0, "IsCubeMap", mxCreateLogicalScalar(metadata.IsCubemap()));
	mxSetField(mx_metadata, 0, "IsPMAlpha", mxCreateLogicalScalar(metadata.IsPMAlpha()));
	mxSetField(mx_metadata, 0, "IsVolumeMap", mxCreateLogicalScalar(metadata.IsVolumemap()));
	return mx_metadata;
}

mxArray* DDS::ExportFormat(DXGI_FORMAT fmt)
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
	mxSetField(mx_fmt, 0, "Name", mxCreateString(g_format_map[fmt].c_str()));
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

mwIndex DDS::ComputeIndexMEX(size_t mip, size_t item, size_t slice)
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
		}
		default:
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_INTERNAL, "ImageDimensionError", "The image was of unexpected dimensionality (%d).", metadata.dimension - 1);
		}
	}
	return ret;
}

mxArray* DDS::ExportImages()
{
	mxArray* tmp,* mx_images;
	mwIndex i, j, k, dst_idx;
	const char* fieldnames[] = {"Width", "Height", "RowPitch", "SlicePitch", "Pixels"};
	DirectX::TexMetadata metadata = this->GetMetadata();
	size_t depth = metadata.depth;
	
	mx_images = mxCreateStructMatrix(max(metadata.arraySize, metadata.depth), metadata.mipLevels, 5, fieldnames);
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

void DDS::PrepareImages(DDS &out)
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

void DDS::ToImage(mxArray*& mx_dds_rgb)
{
	DDS prepared_images;
	this->PrepareImages(prepared_images);
	prepared_images.FormMatrix(mx_dds_rgb);
}

void DDS::ToImage(mxArray*& mx_dds_rgb, mxArray*& mx_dds_a)
{
	DDS prepared_images;
	this->PrepareImages(prepared_images);
	prepared_images.FormMatrix(mx_dds_rgb, mx_dds_a);
}

void DDS::ToMatrix(mxArray*& mx_dds_rgb, bool combine_alpha)
{
	this->FormMatrix(mx_dds_rgb, combine_alpha);
}

void DDS::ToMatrix(mxArray*& mx_dds_rgb, mxArray*& mx_dds_a)
{
	this->FormMatrix(mx_dds_rgb, mx_dds_a);
}

void DDS::FormMatrix(mxArray*& mx_dds_rgb, bool combine_alpha)
{
	mxArray* tmp_rgb;
	size_t i, j, k;
	DirectX::TexMetadata metadata = this->GetMetadata();
	size_t depth = metadata.depth;
	
	if(this->GetImageCount() == 1)
	{
		FormMatrix(this->GetImage(0, 0, 0), mx_dds_rgb, combine_alpha);
	}
	else
	{
		mx_dds_rgb = mxCreateCellMatrix(max(metadata.arraySize, metadata.depth), metadata.mipLevels);
		for(i = 0; i < metadata.mipLevels; i++)
		{
			for(j = 0; j < metadata.arraySize; j++)
			{
				for(k = 0; k < depth; k++)
				{
					FormMatrix(this->GetImage(i, j, k), tmp_rgb, combine_alpha);
					mxSetCell(mx_dds_rgb, this->ComputeIndexMEX(i, j, k), tmp_rgb);
				}
			}
			if(depth > 1)
			{
				depth >>= 1u;
			}
		}
	}
}

void DDS::FormMatrix(mxArray*& mx_dds_rgb, mxArray*& mx_dds_a)
{
	mxArray* tmp_rgb,* tmp_a;
	size_t i, j, k;
	DirectX::TexMetadata metadata = this->GetMetadata();
	size_t depth = metadata.depth;
	
	if(this->GetImageCount() == 1)
	{
		FormMatrix(this->GetImage(0, 0, 0), mx_dds_rgb, mx_dds_a);
	}
	else
	{
		mx_dds_rgb = mxCreateCellMatrix(max(metadata.arraySize, metadata.depth), metadata.mipLevels);
		mx_dds_a = mxCreateCellMatrix(max(metadata.arraySize, metadata.depth), metadata.mipLevels);
		for(i = 0; i < metadata.mipLevels; i++)
		{
			for(j = 0; j < metadata.arraySize; j++)
			{
				for(k = 0; k < depth; k++)
				{
					FormMatrix(this->GetImage(i, j, k), tmp_rgb, tmp_a);
					mxSetCell(mx_dds_rgb, this->ComputeIndexMEX(i, j, k), tmp_rgb);
					mxSetCell(mx_dds_a, this->ComputeIndexMEX(i, j, k), tmp_a);
				}
			}
			if(depth > 1)
			{
				depth >>= 1u;
			}
		}
	}
}

void DDS::FormMatrix(const DirectX::Image* raw_img, mxArray*& mx_ddsslice_rgb, bool combine_alpha)
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
		mx_ddsslice_rgb = mxCreateNumericArray(3, rgb_sz, mxUINT8_CLASS, mxREAL);
		auto r_data = (uint8_t*)mxGetData(mx_ddsslice_rgb);
		auto g_data = (uint8_t*)mxGetData(mx_ddsslice_rgb) + raw_img->height*raw_img->width;
		auto b_data = (uint8_t*)mxGetData(mx_ddsslice_rgb) + 2*raw_img->height*raw_img->width;
		auto a_data = (uint8_t*)mxGetData(mx_ddsslice_rgb) + 3*raw_img->height*raw_img->width;
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
		mx_ddsslice_rgb = mxCreateNumericArray(3, rgb_sz, mxUINT8_CLASS, mxREAL);
		auto r_data = (uint8_t*)mxGetData(mx_ddsslice_rgb);
		auto g_data = (uint8_t*)mxGetData(mx_ddsslice_rgb) + raw_img->height*raw_img->width;
		auto b_data = (uint8_t*)mxGetData(mx_ddsslice_rgb) + 2*raw_img->height*raw_img->width;
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

void DDS::FormMatrix(const DirectX::Image* raw_img, mxArray*& mx_ddsslice_rgb, mxArray*& mx_ddsslice_a)
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
	mx_ddsslice_rgb = mxCreateNumericArray(3, rgb_sz, mxUINT8_CLASS, mxREAL);
	if(DirectX::HasAlpha(raw_img->format))
	{
		mx_ddsslice_a = mxCreateNumericMatrix(raw_img->height, raw_img->width, mxUINT8_CLASS, mxREAL);
		auto r_data = (uint8_t*)mxGetData(mx_ddsslice_rgb);
		auto g_data = (uint8_t*)mxGetData(mx_ddsslice_rgb) + raw_img->height * raw_img->width;
		auto b_data = (uint8_t*)mxGetData(mx_ddsslice_rgb) + 2 * raw_img->height * raw_img->width;
		auto a_data = (uint8_t*)mxGetData(mx_ddsslice_a);
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
		mx_ddsslice_a = mxCreateNumericMatrix(0, 0, mxUINT8_CLASS, mxREAL);
		auto r_data = (uint8_t*)mxGetData(mx_ddsslice_rgb);
		auto g_data = (uint8_t*)mxGetData(mx_ddsslice_rgb) + raw_img->height * raw_img->width;
		auto b_data = (uint8_t*)mxGetData(mx_ddsslice_rgb) + 2 * raw_img->height * raw_img->width;
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

DDSArray::DDSArray(size_t m, size_t n, DWORD flags) : _sz_m(m), _sz_n(n), _size(m*n)
{
	size_t i;
	this->_arr = new(std::nothrow) DDS[this->_size];
	if(this->_arr)
	{
		for(i = 0; i < this->_size; i++)
		{
			this->_arr[i].SetFlags(flags);
		}
	}
	else
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_INTERNAL|MEU_SEVERITY_SYSTEM, "MemoryAllocationError", "Could not allocate memory for DDS objects.");
	}
}

DDS* DDSArray::AllocateDDSArray(size_t num, DDS* in)
{
	size_t i;
	DDS* out = new(std::nothrow) DDS[num];
	if(out)
	{
		for(i = 0; i < num; i++)
		{
			out[i].SetFlags(in[i].GetFlags());
		}
	}
	else
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_INTERNAL|MEU_SEVERITY_SYSTEM, "MemoryAllocationError", "Could not allocate memory for DDS objects.");
	}
	return out;
}

DDS* DDSArray::AllocateDDSArray(size_t num, DWORD flags)
{
	size_t i;
	DDS* out = new(std::nothrow) DDS[num];
	if(out)
	{
		for(i = 0; i < num; i++)
		{
			out[i].SetFlags(flags);
		}
	}
	else
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_INTERNAL|MEU_SEVERITY_SYSTEM, "MemoryAllocationError", "Could not allocate memory for DDS objects.");
	}
	return out;
}

DDSArray DDSArray::ReadFile(int nrhs, const mxArray* prhs[])
{
	size_t i;
	DDSArray dds_array;
	DWORD flags = DirectX::CP_FLAGS_NONE;
	wchar_t* filename;
	
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
		ParseFlags(prhs[1], g_ctrlflag_map, flags);
	}
	
	if(mxIsCell(mx_filenames))
	{
		dds_array = DDSArray(mxGetM(mx_filenames), mxGetN(mx_filenames), flags);
		for(i = 0; i < dds_array._size; i++)
		{
			filename = ParseFilename(mxGetCell(mx_filenames, i));
			hres = DirectX::LoadFromDDSFile(filename, flags, nullptr, dds_array.GetDDS(i));
			if(FAILED(hres))
			{
				MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER | MEU_SEVERITY_HRESULT, "FileReadError", "There was an error while reading the file.");
			}
			mxFree(filename);
		}
	}
	else
	{
		dds_array = DDSArray(1, 1, flags);
		filename = ParseFilename(mx_filenames);
		hres = DirectX::LoadFromDDSFile(filename, flags, nullptr, dds_array.GetDDS(0));
		if(FAILED(hres))
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER | MEU_SEVERITY_HRESULT, "FileReadError", "There was an error while reading the file.");
		}
		mxFree(filename);
	}
	return dds_array;
}

DDSArray DDSArray::Import(const mxArray* in_struct)
{
	size_t i;
	DDSArray dds_array(mxGetM(in_struct), mxGetN(in_struct));
	for(i = 0; i < dds_array._size; i++)
	{
		dds_array.GetDDS(i) = DDS(mxGetField(in_struct, i, "Metadata"), mxGetField(in_struct, i, "Images"));
	}
	return dds_array;
}

void DDSArray::ReadMetadata(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
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
	wchar_t* filename = ParseFilename(prhs[0]);
	
	if(nrhs == 2)
	{
		ParseFlags(prhs[1], g_ctrlflag_map, flags);
	}
	
	hres = DirectX::GetMetadataFromDDSFile(filename, flags, metadata);
	if(FAILED(hres))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER | MEU_SEVERITY_HRESULT, "FileReadError", "There was an error while reading the file.");
	}
	
	plhs[0] = DDS::ExportMetadata(metadata);
	mxFree(filename);
	
}

wchar_t* DDSArray::ParseFilename(const mxArray* mx_filename)
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

void DDSArray::ParseFlags(const mxArray* flags_array, BiMap &map, DWORD &flags)
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

DXGI_FORMAT DDSArray::ParseFormat(const mxArray* mx_fmt)
{
	DXGI_FORMAT fmt = DXGI_FORMAT_UNKNOWN;
	if(!mxIsChar(mx_fmt))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER | MEU_SEVERITY_INTERNAL, "FormatParsingError", "Format input must be of class 'char'.");
	}
	MEXUtils::ToUpper((mxArray*)mx_fmt);
	char* fmtname = mxArrayToString(mx_fmt);
	auto found = g_format_map.find(fmtname);
	if(found != g_format_map.s_end())
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

void DDSArray::FlipRotate(MEXF_IN)
{
	size_t i;
	DDS* new_arr = AllocateDDSArray(this->_size, this->_arr);
	DWORD fr_flags = DirectX::TEX_FR_ROTATE0;
	if(nrhs < 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughInputsError", "Not enough arguments. Please supply a flag.");
	}
	else if(nrhs > 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyInputsError", "Too many arguments.");
	}
	
	DDSArray::ParseFlags(prhs[0], g_frflag_map, fr_flags);
	
	for(i = 0; i < this->_size; i++)
	{
		DDS& pre_op  = this->GetDDS(i);
		DDS& post_op = new_arr[i];
		hres = DirectX::FlipRotate(pre_op.GetImages(), pre_op.GetImageCount(), pre_op.GetMetadata(), fr_flags, post_op);
		if(FAILED(hres))
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "FlipRotateError", "There was an error while rotating or flipping the image.");
		}
	}
	delete[](this->_arr);
	this->_arr = new_arr;
}

void DDSArray::Convert(MEXF_IN)
{
	size_t i;
	DXGI_FORMAT fmt;
	DDS* new_arr = AllocateDDSArray(this->_size, this->_arr);
	DWORD filter = DirectX::TEX_FILTER_DEFAULT;
	float threshold = DirectX::TEX_THRESHOLD_DEFAULT;
	if(nrhs < 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughInputsError", "Not enough arguments. Please supply a format");
	}
	else if(nrhs > 3)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyInputsError", "Too many arguments.");
	}
	
	
	fmt = ParseFormat(prhs[0]);
	
	if(nrhs > 1)
	{
		ParseFlags(prhs[1], g_filterflag_map, filter);
	}
	
	if(nrhs > 2)
	{
		threshold = (float)mxGetScalar(prhs[2]);
	}
	
	for(i = 0; i < this->_size; i++)
	{
		DDS& pre_op  = this->GetDDS(i);
		DDS& post_op = new_arr[i];
		hres = DirectX::Convert(pre_op.GetImages(), pre_op.GetImageCount(), pre_op.GetMetadata(), fmt, filter, threshold, post_op);
		if(FAILED(hres))
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "ConversionError", "There was an error converting the image.");
		}
	}
	delete[](this->_arr);
	this->_arr = new_arr;
}

void DDSArray::Decompress(MEXF_IN)
{
	size_t i;
	DDS* new_arr = AllocateDDSArray(this->_size, this->_arr);
	DXGI_FORMAT fmt = DXGI_FORMAT_UNKNOWN;
	if(nrhs > 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyInputsError", "Too many arguments.");
	}
	
	if(nrhs == 1)
	{
		fmt = DDSArray::ParseFormat(prhs[0]);
	}
	
	for(i = 0; i < this->_size; i++)
	{
		DDS& pre_op  = this->GetDDS(i);
		DDS& post_op = new_arr[i];
		hres = DirectX::Decompress(pre_op.GetImages(), pre_op.GetImageCount(), pre_op.GetMetadata(), fmt, post_op);
		if(FAILED(hres))
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "DecompressError", "There was an error while decompressing the image.");
		}
	}
	delete[](this->_arr);
	this->_arr = new_arr;
}

void DDSArray::ToImage(MEXF_SIG)
{
	size_t i;
	if(this->_size == 1)
	{
		if(nlhs > 1)
		{
			this->_arr[0].ToImage(plhs[0], plhs[1]);
		}
		else
		{
			this->_arr[0].ToImage(plhs[0]);
		}
	}
	else
	{
		if(nlhs > 1)
		{
			plhs[0] = mxCreateCellMatrix(this->_sz_m, this->_sz_n);
			plhs[1] = mxCreateCellMatrix(this->_sz_m, this->_sz_n);
			for(i = 0; i < this->_size; i++)
			{
				mxArray* tmp0, * tmp1;
				this->_arr[i].ToImage(tmp0, tmp1);
				mxSetCell(plhs[0], i, tmp0);
				mxSetCell(plhs[1], i, tmp1);
			}
		}
		else
		{
			plhs[0] = mxCreateCellMatrix(this->_sz_m, this->_sz_n);
			for(i = 0; i < this->_size; i++)
			{
				mxArray* tmp;
				this->_arr[i].ToImage(tmp);
				mxSetCell(plhs[0], i, tmp);
			}
			
		}
	}
}

void DDSArray::ToMatrix(MEXF_SIG)
{
	size_t i;
	bool combine_alpha = false;
	if(nrhs > 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyInputsError", "Too many arguments.");
	}
	
	if(nrhs == 1)
	{
		MEXUtils::ToUpper((mxArray*)prhs[0]);
		if(MEXUtils::CompareMEXString(prhs[0], "COMBINEALPHA"))
		{
			if(nlhs > 1)
			{
				MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "MatrixOptionError", "The 'CombineAlpha' option requires either 0 or 1 outputs.");
			}
			combine_alpha = true;
		}
		else
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "MatrixOptionError", "Unrecognized option '%s'.", mxArrayToString(prhs[0]));
		}
	}
	
	if(this->_size == 1)
	{
		if(nlhs > 1)
		{
			this->_arr[0].ToMatrix(plhs[0], plhs[1]);
		}
		else
		{
			this->_arr[0].ToMatrix(plhs[0], combine_alpha);
		}
	}
	else
	{
		if(nlhs > 1)
		{
			plhs[0] = mxCreateCellMatrix(this->_sz_m, this->_sz_n);
			plhs[1] = mxCreateCellMatrix(this->_sz_m, this->_sz_n);
			for(i = 0; i < this->_size; i++)
			{
				mxArray* tmp0,* tmp1;
				this->_arr[i].ToMatrix(tmp0, tmp1);
				mxSetCell(plhs[0], i, tmp0);
				mxSetCell(plhs[1], i, tmp1);
			}
			
			
		}
		else
		{
			plhs[0] = mxCreateCellMatrix(this->_sz_m, this->_sz_n);
			for(i = 0; i < this->_size; i++)
			{
				mxArray* tmp;
				this->_arr[i].ToMatrix(tmp, combine_alpha);
				mxSetCell(plhs[0], i, tmp);
			}
		}
		
	}
}

void DDSArray::ToExport(int nlhs, mxArray* plhs[])
{
	size_t i;
	const char* fieldnames[] = {"Metadata", "Images"};
	mxArray* out = mxCreateStructMatrix(this->_sz_m, this->_sz_n, 2, fieldnames);
	for(i = 0; i < this->_size; i++)
	{
		mxSetField(out, i, "Metadata", this->GetDDS(i).ExportMetadata());
		mxSetField(out, i, "Images", this->GetDDS(i).ExportImages());
	}
	plhs[0] = out;
}
