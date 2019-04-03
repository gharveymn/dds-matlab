#include "mex.h"
#include "dxtmex_maps.hpp"
#include "dxtmex_dxtimage.hpp"
#include "dxtmex_mexerror.hpp"
#include "dxtmex_mexutils.hpp"

using namespace DXTMEX;

static std::map<std::string, DXTImageArray::operation> g_directive_map
{
	{"READ_DDS_FILE",                    DXTImageArray::READ_DDS_FILE                   },
	{"READ_DDS_META",                    DXTImageArray::READ_DDS_META                   },
	{"FLIP_ROTATE",                      DXTImageArray::FLIP_ROTATE                     },
	{"RESIZE",                           DXTImageArray::RESIZE                          },
	{"CONVERT",                          DXTImageArray::CONVERT                         },
	{"CONVERT_TO_SINGLE_PLANE",          DXTImageArray::CONVERT_TO_SINGLE_PLANE         },
	{"GENERATE_MIPMAPS",                 DXTImageArray::GENERATE_MIPMAPS                },
	{"GENERATE_MIPMAPS_3D",              DXTImageArray::GENERATE_MIPMAPS_3D             },
	{"SCALE_MIPMAPS_ALPHA_FOR_COVERAGE", DXTImageArray::SCALE_MIPMAPS_ALPHA_FOR_COVERAGE},
	{"PREMULTIPLY_ALPHA",                DXTImageArray::PREMULTIPLY_ALPHA               },
	{"COMPRESS",                         DXTImageArray::COMPRESS                        },
	{"DECOMPRESS",                       DXTImageArray::DECOMPRESS                      },
	{"COPY_RECTANGLE",                   DXTImageArray::COPY_RECTANGLE                  },
	{"COMPUTE_NORMAL_MAP",               DXTImageArray::COMPUTE_NORMAL_MAP              },
	{"COMPUTE_MSE",                      DXTImageArray::COMPUTE_MSE                     },
	{"SAVE_FILE",                        DXTImageArray::SAVE_FILE                       },
	{"TO_IMAGE",                         DXTImageArray::TO_IMAGE                        },
	{"TO_MATRIX",                        DXTImageArray::TO_MATRIX                       }
};

DXTImage::DXTImage(const mxArray* mx_width, const mxArray* mx_height, const mxArray* mx_row_pitch, const mxArray* mx_slice_pitch, const mxArray* mx_pixels, const mxArray* mx_formatid, const mxArray* mx_flags)
{
	if((mx_width == nullptr) || (mx_height == nullptr) || (mx_row_pitch == nullptr) || (mx_slice_pitch == nullptr) || (mx_pixels == nullptr) || (mx_formatid == nullptr) || (mx_flags == nullptr))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER|MEU_SEVERITY_INTERNAL, "InvalidImportError", "An imported DXTImageSlice field was unexpectedly empty.");
	}
	this->_flags = (DWORD)*(uint32_T*)mxGetData(mx_flags);
	
	auto width =      (size_t)*(uint64_T*)mxGetData(mx_width);
	auto height =     (size_t)*(uint64_T*)mxGetData(mx_height);
	auto format =     (DXGI_FORMAT)*(uint32_T*)mxGetData(mx_formatid);
	
	this->Initialize2D(format, width, height, 1, 1, this->_flags);
	
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

DXTImage::DXTImage(const mxArray* mx_metadata, const mxArray* mx_images)
{
	if(mx_metadata == nullptr)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER|MEU_SEVERITY_INTERNAL, "NotEnoughInputsError", "Dds import metadata was unexpectedly empty.");
	}
	if(mx_images == nullptr)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER|MEU_SEVERITY_INTERNAL, "NotEnoughInputsError", "Dds import images was unexpectedly empty.");
	}
	DirectX::TexMetadata metadata = {};
	ImportMetadata(mx_metadata, metadata);
	this->_flags = (DWORD)*(uint32_T*)mxGetData(mxGetField(mx_metadata, 0, "Flags"));
	this->Initialize(metadata, this->_flags);
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

mxArray* DXTImage::ExportMetadata()
{
	return ExportMetadata(this->GetMetadata(), this->_flags);
}

mxArray* DXTImage::ExportMetadata(const DirectX::TexMetadata& metadata)
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

mxArray* DXTImage::ExportMetadata(const DirectX::TexMetadata& metadata, DWORD flags)
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
	const char* fieldnames[] = {"Width", "Height", "RowPitch", "SlicePitch", "Pixels", "FormatID", "Flags"};
	DirectX::TexMetadata metadata = this->GetMetadata();
	size_t depth = metadata.depth;
	
	mx_images = mxCreateStructMatrix(max(metadata.arraySize, metadata.depth), metadata.mipLevels, 7, fieldnames);
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
				MEXUtils::SetScalarField(mx_images, dst_idx, "Flags", mxUINT32_CLASS, this->_flags);
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
		mx_dxtimage_rgb = mxCreateCellMatrix(max(metadata.arraySize, metadata.depth), metadata.mipLevels);
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
		mx_dxtimage_rgb = mxCreateCellMatrix(max(metadata.arraySize, metadata.depth), metadata.mipLevels);
		mx_dxtimage_a = mxCreateCellMatrix(max(metadata.arraySize, metadata.depth), metadata.mipLevels);
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

DXTImageArray::DXTImageArray(size_t m, size_t n, DWORD flags) : _sz_m(m), _sz_n(n), _size(m*n)
{
	size_t i;
	this->_arr = new(std::nothrow) DXTImage[this->_size];
	if(this->_arr)
	{
		for(i = 0; i < this->_size; i++)
		{
			this->_arr[i].SetFlags(flags);
		}
	}
	else
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_INTERNAL|MEU_SEVERITY_SYSTEM, "MemoryAllocationError", "Could not allocate memory for the DXTImage array.");
	}
}

DXTImage* DXTImageArray::AllocateDXTImageArray(size_t num, DXTImage* in)
{
	size_t i;
	auto out = new(std::nothrow) DXTImage[num];
	if(out)
	{
		for(i = 0; i < num; i++)
		{
			out[i].SetFlags(in[i].GetFlags());
		}
	}
	else
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_INTERNAL|MEU_SEVERITY_SYSTEM, "MemoryAllocationError", "Could not allocate memory for the DXTImage array.");
	}
	return out;
}

DXTImage* DXTImageArray::AllocateDXTImageArray(size_t num)
{
	auto out = new(std::nothrow) DXTImage[num];
	if(!out)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_INTERNAL|MEU_SEVERITY_SYSTEM, "MemoryAllocationError", "Could not allocate memory for the DXTImage array.");
	}
	return out;
}

DXTImage* DXTImageArray::AllocateDXTImageArray(size_t num, DWORD flags)
{
	size_t i;
	auto out = new(std::nothrow) DXTImage[num];
	if(out)
	{
		for(i = 0; i < num; i++)
		{
			out[i].SetFlags(flags);
		}
	}
	else
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_INTERNAL|MEU_SEVERITY_SYSTEM, "MemoryAllocationError", "Could not allocate memory for the DXTImage array.");
	}
	return out;
}

void DXTImageArray::ReadDDSFile(int nrhs, const mxArray** prhs)
{
	size_t i;
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
		this->_sz_m = mxGetM(mx_filenames);
		this->_sz_n = mxGetN(mx_filenames);
		this->_size = this->_sz_m * this->_sz_n;
		this->_arr  = AllocateDXTImageArray(this->_size, flags);
		for(i = 0; i < this->_size; i++)
		{
			filename = ParseFilename(mxGetCell(mx_filenames, i));
			hres = DirectX::LoadFromDDSFile(filename, flags, nullptr, this->_arr[i]);
			if(FAILED(hres))
			{
				MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER | MEU_SEVERITY_HRESULT, "FileReadError", "There was an error while reading the file.");
			}
			mxFree(filename);
		}
	}
	else
	{
		this->_sz_m = 1;
		this->_sz_n = 1;
		this->_size = 1;
		this->_arr  = AllocateDXTImageArray(this->_size, flags);
		filename = ParseFilename(mx_filenames);
		hres = DirectX::LoadFromDDSFile(filename, flags, nullptr, this->_arr[0]);
		if(FAILED(hres))
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER | MEU_SEVERITY_HRESULT, "FileReadError", "There was an error while reading the file.");
		}
		mxFree(filename);
	}
}

void DXTImageArray::Import(int nrhs, const mxArray* prhs[])
{
	size_t i;
	if(nrhs < 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER|MEU_SEVERITY_INTERNAL, "NotEnoughInputsError", "Not enough arguments. Please supply a Dds object.");
	}
	
	const mxArray* in_struct = prhs[0];
	
	if(!mxIsStruct(in_struct))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER|MEU_SEVERITY_INTERNAL, "InvalidImportError", "Import object must be of class 'struct'.");
	}
	
	if(mxIsEmpty(in_struct))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER|MEU_SEVERITY_INTERNAL, "InvalidImportError", "Import object must not be empty.");
	}
	
	this->_sz_m = mxGetM(in_struct);
	this->_sz_n = mxGetN(in_struct);
	this->_size = this->_sz_m * this->_sz_n;
	this->_arr  = AllocateDXTImageArray(this->_size);
	if(DXTImage::IsDXTImageImport(in_struct))
	{
		for(i = 0; i < this->_size; i++)
		{
			this->_arr[i] = DXTImage(mxGetField(in_struct, i, "Metadata"), mxGetField(in_struct, i, "Images"));
		}
	}
	else if(DXTImage::IsDXTImageSliceImport(in_struct))
	{
		for(i = 0; i < this->_size; i++)
		{
			this->_arr[i] = DXTImage(mxGetField(in_struct, i, "Width"),
			                    mxGetField(in_struct, i, "Height"),
			                    mxGetField(in_struct, i, "RowPitch"),
			                    mxGetField(in_struct, i, "SlicePitch"),
			                    mxGetField(in_struct, i, "Pixels"),
			                    mxGetField(in_struct, i, "FormatID"),
			                    mxGetField(in_struct, i, "Flags"));
		}
	}
	else
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER|MEU_SEVERITY_INTERNAL, "InvalidImportError", "The import object was invalid.");
	}
	
}

bool DXTImage::IsDXTImageImport(const mxArray* in)
{
	if(mxIsEmpty(in))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER|MEU_SEVERITY_INTERNAL, "InvalidImportError", "Import object must not be empty.");
	}
	return (mxGetField(in, 0, "Metadata") != nullptr) &&
	       (mxGetField(in, 0, "Images")   != nullptr);
}

bool DXTImage::IsDXTImageSliceImport(const mxArray* in)
{
	if(mxIsEmpty(in))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER|MEU_SEVERITY_INTERNAL, "InvalidImportError", "Import object must not be empty.");
	}
	return (mxGetField(in, 0, "Width")      != nullptr) &&
		  (mxGetField(in, 0, "Height")     != nullptr) &&
		  (mxGetField(in, 0, "FormatID")   != nullptr) &&
		  (mxGetField(in, 0, "RowPitch")   != nullptr) &&
		  (mxGetField(in, 0, "SlicePitch") != nullptr) &&
		  (mxGetField(in, 0, "Pixels")     != nullptr) &&
	       (mxGetField(in, 0, "Flags")      != nullptr);
}

void DXTImageArray::ReadMetadata(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
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
	
	plhs[0] = DXTImage::ExportMetadata(metadata);
	mxFree(filename);
	
}

wchar_t* DXTImageArray::ParseFilename(const mxArray* mx_filename)
{
	mwIndex i;
	if(!mxIsChar(mx_filename))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InputError", "Filenames must be of type 'char'.");
	}
	const mxChar* mx_str = mxGetChars(mx_filename);
	mwSize mx_strlen = mxGetNumberOfElements(mx_filename);
	auto filename = (wchar_t*)mxMalloc((mx_strlen + 1)*sizeof(wchar_t));
	for(i = 0; i < mx_strlen; i++)
	{
		filename[i] = mx_str[i];
	}
	filename[mx_strlen] = 0;
	return filename;
}

void DXTImageArray::ParseFilename(const mxArray* mx_filename, std::wstring& filename)
{
	mwIndex i;
	if(!mxIsChar(mx_filename))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InputError", "Filenames must be of type 'char'.");
	}
	const mxChar* mx_str = mxGetChars(mx_filename);
	mwSize mx_strlen = mxGetNumberOfElements(mx_filename);
	filename.clear();
	for(i = 0; i < mx_strlen; i++)
	{
		filename += mx_str[i];
	}
}

void DXTImageArray::ParseFlags(const mxArray* mx_flags, BiMap &map, DWORD &flags)
{
	mwIndex i;
	mxArray* curr_flag;
	
	if(mxIsCell(mx_flags))
	{
		mwSize num_flags = mxGetNumberOfElements(mx_flags);
		for(i = 0; i < num_flags; i++)
		{
			curr_flag = mxGetCell(mx_flags, i);
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
	else if(mxIsChar(mx_flags))
	{
		MEXUtils::ToUpper((mxArray*)mx_flags);
		char* flagname = mxArrayToString(mx_flags);
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

DXGI_FORMAT DXTImageArray::ParseFormat(const mxArray* mx_fmt)
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

void DXTImageArray::FlipRotate(MEXF_IN)
{
	size_t i;
	DXTImage* new_arr = AllocateDXTImageArray(this->_size, this->_arr);
	DWORD fr_flags = DirectX::TEX_FR_ROTATE0;
	if(nrhs < 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughArgumentsError", "Not enough arguments. Please supply a flag.");
	}
	else if(nrhs > 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyArgumentsError", "Too many arguments.");
	}
	
	DXTImageArray::ParseFlags(prhs[0], g_frflag_map, fr_flags);
	
	for(i = 0; i < this->_size; i++)
	{
		DXTImage& pre_op  = this->GetDXTImage(i);
		DXTImage& post_op = new_arr[i];
		hres = DirectX::FlipRotate(pre_op.GetImages(), pre_op.GetImageCount(), pre_op.GetMetadata(), fr_flags, post_op);
		if(FAILED(hres))
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "FlipRotateError", "There was an error while rotating or flipping the image.");
		}
	}
	delete[](this->_arr);
	this->_arr = new_arr;
}

void DXTImageArray::Resize(MEXF_IN)
{
	size_t i;
	DXTImage* new_arr = AllocateDXTImageArray(this->_size, this->_arr);
	size_t w, h;
	DWORD filter_flags = DirectX::TEX_FILTER_DEFAULT;
	if(nrhs < 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughArgumentsError", "No enough arguments. Please supply a size.");
	}
	if(nrhs > 3)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyArgumentsError", "Too many arguments.");
	}
	
	if(!mxIsDouble(prhs[0]))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InvalidSizeError", "Input size must be of class 'double'.");
	}
	
	if(mxIsScalar(prhs[0]))
	{
		/* 2 scalar arguments with optional filter flag argument */
		if(nrhs < 2)
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InvalidSizeError", "Missing required width argument.");
		}
		h = (size_t)mxGetScalar(prhs[0]);
		
		/* expect width as scalar*/
		if(!mxIsDouble(prhs[1]) || !mxIsScalar(prhs[1]))
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InvalidSizeError", "Input width must be a scalar double.");
		}
		w = (size_t)mxGetScalar(prhs[1]);
		if(nrhs > 2)
		{
			DXTImageArray::ParseFlags(prhs[2], g_filterflag_map, filter_flags);
		}
	}
	else
	{
		/* 2 element vector with optional filter flag argument */
		if(nrhs > 2)
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyArgumentsError", "Too many arguments.");
		}
		if(mxGetNumberOfElements(prhs[0]) != 2)
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InvalidSizeError", "Input size must have either 1 or 2 elements.");
		}
		auto data = (double*)mxGetData(prhs[0]);
		h = (size_t)*(data);
		w = (size_t)*(data + 1);
		if(nrhs == 2)
		{
			DXTImageArray::ParseFlags(prhs[2], g_filterflag_map, filter_flags);
		}
	}
	
	for(i = 0; i < this->_size; i++)
	{
		DXTImage& pre_op  = this->GetDXTImage(i);
		DXTImage& post_op = new_arr[i];
		hres = DirectX::Resize(pre_op.GetImages(), pre_op.GetImageCount(), pre_op.GetMetadata(), w, h, filter_flags, post_op);
		if(FAILED(hres))
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "ResizeError", "There was an error while resizing the image.");
		}
	}
	delete[](this->_arr);
	this->_arr = new_arr;
}

void DXTImageArray::Convert(MEXF_IN)
{
	size_t i;
	DXGI_FORMAT fmt;
	DXTImage* new_arr = AllocateDXTImageArray(this->_size, this->_arr);
	DWORD filter_flags = DirectX::TEX_FILTER_DEFAULT;
	float threshold = DirectX::TEX_THRESHOLD_DEFAULT;
	if(nrhs < 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughArgumentsError", "Not enough arguments. Please supply a format");
	}
	else if(nrhs > 3)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyArgumentsError", "Too many arguments.");
	}
	
	
	fmt = DXTImageArray::ParseFormat(prhs[0]);
	
	if(nrhs > 1)
	{
		DXTImageArray::ParseFlags(prhs[1], g_filterflag_map, filter_flags);
	}
	
	if(nrhs > 2)
	{
		threshold = (float)mxGetScalar(prhs[2]);
	}
	
	for(i = 0; i < this->_size; i++)
	{
		DXTImage& pre_op  = this->GetDXTImage(i);
		DXTImage& post_op = new_arr[i];
		hres = DirectX::Convert(pre_op.GetImages(), pre_op.GetImageCount(), pre_op.GetMetadata(), fmt, filter_flags, threshold, post_op);
		if(FAILED(hres))
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "ConvertError", "There was an error while converting the image.");
		}
	}
	delete[](this->_arr);
	this->_arr = new_arr;
}

void DXTImageArray::ConvertToSinglePlane(MEXF_IN)
{
	size_t i;
	DXTImage* new_arr = AllocateDXTImageArray(this->_size, this->_arr);
	if(nrhs > 0)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyArgumentsError", "Too many arguments.");
	}
	
	for(i = 0; i < this->_size; i++)
	{
		DXTImage& pre_op  = this->GetDXTImage(i);
		DXTImage& post_op = new_arr[i];
		hres = DirectX::ConvertToSinglePlane(pre_op.GetImages(), pre_op.GetImageCount(), pre_op.GetMetadata(), post_op);
		if(FAILED(hres))
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "ConvertToSinglePlaneError", "There was an error while converting the image to a single plane.");
		}
	}
	delete[](this->_arr);
	this->_arr = new_arr;
}

void DXTImageArray::GenerateMipMaps(MEXF_IN)
{
	size_t i;
	DXTImage* new_arr = AllocateDXTImageArray(this->_size, this->_arr);
	size_t levels = 0;
	DWORD filter_flags = DirectX::TEX_FILTER_DEFAULT;
	if(nrhs > 2)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyArgumentsError", "Too many arguments.");
	}
	
	if(nrhs > 0)
	{
		if(mxIsDouble(prhs[0]))
		{
			if(!mxIsScalar(prhs[0]))
			{
				MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InvalidInputError", "The number of levels must be scalar.");
			}
			levels = (size_t)mxGetScalar(prhs[0]);
			if(nrhs > 1)
			{
				DXTImageArray::ParseFlags(prhs[1], g_filterflag_map, filter_flags);
			}
		}
		else if(mxIsChar(prhs[0]))
		{
			if(nrhs > 1)
			{
				MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyArgumentsError", "Filter flags must be the last argument.");
			}
			DXTImageArray::ParseFlags(prhs[1], g_filterflag_map, filter_flags);
		}
		else
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InvalidInputError", "The first argument must either be of class 'double' or class 'char'.");
		}
	}
	
	for(i = 0; i < this->_size; i++)
	{
		DXTImage& pre_op  = this->GetDXTImage(i);
		DXTImage& post_op = new_arr[i];
		switch(pre_op.GetMetadata().dimension)
		{
			case DirectX::TEX_DIMENSION_TEXTURE1D:
			case DirectX::TEX_DIMENSION_TEXTURE2D:
			{
				hres = DirectX::GenerateMipMaps(pre_op.GetImages(), pre_op.GetImageCount(), pre_op.GetMetadata(), filter_flags, levels, post_op);
				if(FAILED(hres))
				{
					MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "GenerateMipMipsError", "There was an error while generating mipmaps for the image.");
				}
			}
			case DirectX::TEX_DIMENSION_TEXTURE3D:
			{
				hres = DirectX::GenerateMipMaps3D(pre_op.GetImages(), pre_op.GetImageCount(), pre_op.GetMetadata(), filter_flags, levels, post_op);
				if(FAILED(hres))
				{
					MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "GenerateMipMips3DError", "There was an error while generating mipmaps for the image.");
				}
				break;
			}
			default:
			{
				MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_INTERNAL, "ImageDimensionError", "The image was of unexpected dimensionality (%d).", pre_op.GetMetadata().dimension - 1);
			}
		}
	}
	delete[](this->_arr);
	this->_arr = new_arr;
}

void DXTImageArray::ScaleMipMapsAlphaForCoverage(MEXF_IN)
{
	size_t i, j;
	DXTImage* new_arr = AllocateDXTImageArray(this->_size, this->_arr);
	float alpha_ref = DirectX::TEX_THRESHOLD_DEFAULT;
	if(nrhs > 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyArgumentsError", "Too many arguments.");
	}
	
	if(nrhs == 1)
	{
		if(!mxIsDouble(prhs[0]) || !mxIsScalar(prhs[0]))
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InvalidSizeError", "Reference alpha level must be a scalar double.");
		}
		
		alpha_ref = (float)mxGetScalar(prhs[0]);
		
		if(alpha_ref < 0 || alpha_ref > 1)
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InvalidSizeError", "Reference alpha level must be between 0 and 1.");
		}
	}
	
	for(i = 0; i < this->_size; i++)
	{
		DXTImage& pre_op  = this->GetDXTImage(i);
		DXTImage& post_op = new_arr[i];
		const DirectX::TexMetadata& metadata = pre_op.GetMetadata();
		for(j = 0; j < metadata.mipLevels; j++)
		{
			const DirectX::Image* image = pre_op.GetImage(0, j, 0);
			hres = DirectX::ScaleMipMapsAlphaForCoverage(image, metadata.mipLevels, metadata, j, alpha_ref, post_op);
			if(FAILED(hres))
			{
				MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "ScaleMipMapsAlphaForCoverageError", "There was an error while scaling the mipmaps alpha for coverage.");
			}
		}
	}
	delete[](this->_arr);
	this->_arr = new_arr;
}

void DXTImageArray::PremultiplyAlpha(MEXF_IN)
{
	size_t i;
	DXTImage* new_arr = AllocateDXTImageArray(this->_size, this->_arr);
	DWORD pmalpha_flags = DirectX::TEX_PMALPHA_DEFAULT;
	if(nrhs > 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyArgumentsError", "Too many arguments.");
	}
	
	if(nrhs == 1)
	{
		DXTImageArray::ParseFlags(prhs[0], g_pmflag_map, pmalpha_flags);
	}
	
	for(i = 0; i < this->_size; i++)
	{
		DXTImage& pre_op  = this->GetDXTImage(i);
		DXTImage& post_op = new_arr[i];
		hres = DirectX::PremultiplyAlpha(pre_op.GetImages(), pre_op.GetImageCount(), pre_op.GetMetadata(), pmalpha_flags, post_op);
		if(FAILED(hres))
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "PremultiplyAlphaError", "There was an error while premultiplying the alpha of the image.");
		}
	}
	delete[](this->_arr);
	this->_arr = new_arr;
}

void DXTImageArray::Compress(MEXF_IN)
{
	size_t i;
	DXGI_FORMAT fmt;
	DXTImage* new_arr = AllocateDXTImageArray(this->_size, this->_arr);
	DWORD compress_flags  = DirectX::TEX_COMPRESS_DEFAULT;
	float threshold = DirectX::TEX_THRESHOLD_DEFAULT;
	if(nrhs < 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughArgumentsError", "No enough arguments. Please supply a format.");
	}
	if(nrhs > 3)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyArgumentsError", "Too many arguments.");
	}
	
	fmt = DXTImageArray::ParseFormat(prhs[0]);
	
	if(nrhs > 1)
	{
		if(mxIsChar(prhs[1]))
		{
			DXTImageArray::ParseFlags(prhs[1], g_compressflag_map, compress_flags);
			if(nrhs > 2)
			{
				if(!mxIsScalar(prhs[2]))
				{
					MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InvalidInputError", "The alpha threshold must be scalar.");
				}
				threshold = (float)mxGetScalar(prhs[2]);
			}
		}
		else if(mxIsDouble(prhs[1]))
		{
			if(nrhs > 1)
			{
				MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyArgumentsError", "The alpha threshold must be the last argument.");
			}
			if(!mxIsScalar(prhs[1]))
			{
				MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InvalidInputError", "The alpha threshold must be scalar.");
			}
			threshold = (float)mxGetScalar(prhs[1]);
		}
		else
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InvalidInputError", "The optional second argument must either be of class 'double' or class 'char'.");
		}
	}
	
	for(i = 0; i < this->_size; i++)
	{
		DXTImage& pre_op  = this->GetDXTImage(i);
		DXTImage& post_op = new_arr[i];
		hres = DirectX::Compress(pre_op.GetImages(), pre_op.GetImageCount(), pre_op.GetMetadata(), fmt, compress_flags, threshold, post_op);
		if(FAILED(hres))
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "CompressError", "There was an error while compressing the image.");
		}
	}
	delete[](this->_arr);
	this->_arr = new_arr;
}

void DXTImageArray::Decompress(MEXF_IN)
{
	size_t i;
	DXTImage* new_arr = AllocateDXTImageArray(this->_size, this->_arr);
	DXGI_FORMAT fmt = DXGI_FORMAT_UNKNOWN;
	if(nrhs > 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyArgumentsError", "Too many arguments.");
	}
	
	if(nrhs == 1)
	{
		fmt = DXTImageArray::ParseFormat(prhs[0]);
	}
	
	for(i = 0; i < this->_size; i++)
	{
		DXTImage& pre_op  = this->GetDXTImage(i);
		DXTImage& post_op = new_arr[i];
		hres = DirectX::Decompress(pre_op.GetImages(), pre_op.GetImageCount(), pre_op.GetMetadata(), fmt, post_op);
		if(FAILED(hres))
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "DecompressError", "There was an error while decompressing the image.");
		}
	}
	delete[](this->_arr);
	this->_arr = new_arr;
}

void DXTImageArray::ComputeNormalMap(MEXF_IN)
{
	size_t i;
	DXTImage* new_arr = AllocateDXTImageArray(this->_size, this->_arr);
	DXGI_FORMAT fmt;
	float amplitude;
	DWORD cn_flags  = DirectX::CNMAP_DEFAULT;
	if(nrhs < 2)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughArgumentsError", "No enough arguments. Please supply a format and an amplitude.");
	}
	if(nrhs > 3)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyArgumentsError", "Too many arguments.");
	}
	
	fmt = DXTImageArray::ParseFormat(prhs[0]);
	
	if(!mxIsDouble(prhs[1]) || !mxIsScalar(prhs[1]))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InvalidSizeError", "Amplitude must be a scalar double.");
	}
	amplitude = (float)mxGetScalar(prhs[1]);
	
	if(nrhs == 3)
	{
		DXTImageArray::ParseFlags(prhs[2], g_cnflag_map, cn_flags);
	}
	
	for(i = 0; i < this->_size; i++)
	{
		DXTImage& pre_op  = this->GetDXTImage(i);
		DXTImage& post_op = new_arr[i];
		hres = DirectX::ComputeNormalMap(pre_op.GetImages(), pre_op.GetImageCount(), pre_op.GetMetadata(), cn_flags, amplitude, fmt, post_op);
		if(FAILED(hres))
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "ComputeNormalMapError", "There was an error while computing the normal map.");
		}
	}
	delete[](this->_arr);
	this->_arr = new_arr;
}

void DXTImageArray::CopyRectangle(DXTImageArray& src, DXTImageArray& dst, MEXF_IN)
{
	size_t i, j;
	DWORD filter_flags  = DirectX::TEX_FILTER_DEFAULT;
	size_t out_x;
	size_t out_y;
	if(nrhs < 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughArgumentsError", "No enough arguments. Please supply a 4-element vector specifying the rectangle to copy, and "
															"a 2-element vector specifying its destination coordinates.");
	}
	if(nrhs > 3)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyArgumentsError", "Too many arguments.");
	}
	
	if(!mxIsDouble(prhs[0]) || mxGetNumberOfElements(prhs[0]) != 4)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InvalidSizeError", "The rectangle specifier must be of class 'double' and have 4 elements.");
	}
	auto data = (double*)mxGetData(prhs[0]);
	const DirectX::Rect rect((size_t)data[1], (size_t)data[0], (size_t)data[3], (size_t)data[2]);
	
	if(!mxIsDouble(prhs[1]) || mxGetNumberOfElements(prhs[1]) != 2)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InvalidSizeError", "The destination specifier must be of class 'double' and have 2 elements.");
	}
	data = (double*)mxGetData(prhs[0]);
	out_x = (size_t)data[1];
	out_y = (size_t)data[0];
	
	if(nrhs == 3)
	{
		DXTImageArray::ParseFlags(prhs[2], g_filterflag_map, filter_flags);
	}
	
	if(src._size != dst._size)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InputMismatchError", "The source and destination arguments are not the same size.");
	}
	
	for(i = 0; i < src._size; i++)
	{
		DXTImage& pre_op  = src.GetDXTImage(i);
		DXTImage& post_op = dst.GetDXTImage(i);
		
		if(pre_op.GetImageCount() != post_op.GetImageCount())
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InputMismatchError", "The source and destination arguments are not the same size.");
		}
		
		auto pre_op_imgs  = pre_op.GetImages();
		auto post_op_imgs = post_op.GetImages();
		for(j = 0; j < pre_op.GetImageCount(); j++)
		{
			hres = DirectX::CopyRectangle(*(pre_op_imgs + j), rect, *(post_op_imgs + j), filter_flags, out_x, out_y);
			if(FAILED(hres))
			{
				MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "CopyRectangleError", "There was an error while copying over the rectangle.");
			}
		}
	}
}

void DXTImageArray::ComputeMSE(DXTImageArray& dxtimagearray1, DXTImageArray& dxtimagearray2, MEXF_SIG)
{
	size_t i;
	DWORD cmse_flags = DirectX::CMSE_DEFAULT;
	if(nrhs > 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyArgumentsError", "Too many arguments.");
	}
	
	if(nrhs == 1)
	{
		DXTImageArray::ParseFlags(prhs[2], g_cmseflag_map, cmse_flags);
	}
	
	if(dxtimagearray1._size != dxtimagearray2._size)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InputMismatchError", "The comparison arguments are not the same size.");
	}
	
	if(dxtimagearray1._size == 1)
	{
		if(nlhs > 1)
		{
			mxArray* tmp[2];
			plhs[0] = mxCreateCellMatrix(dxtimagearray1.GetM(), dxtimagearray1.GetN());
			plhs[1] = mxCreateCellMatrix(dxtimagearray1.GetM(), dxtimagearray1.GetN());
			for(i = 0; i < dxtimagearray1.GetSize(); i++)
			{
				DXTImageArray::ComputeMSE(dxtimagearray1.GetDXTImage(i), dxtimagearray2.GetDXTImage(i), cmse_flags, tmp[0], tmp[1]);
				mxSetCell(plhs[0], i, tmp[0]);
				mxSetCell(plhs[1], i, tmp[1]);
			}
		}
		else
		{
			mxArray* tmp;
			plhs[0] = mxCreateCellMatrix(dxtimagearray1.GetM(), dxtimagearray1.GetN());
			for(i = 0; i < dxtimagearray1.GetSize(); i++)
			{
				DXTImageArray::ComputeMSE(dxtimagearray1.GetDXTImage(i), dxtimagearray2.GetDXTImage(i), cmse_flags, tmp);
				mxSetCell(plhs[0], i, tmp);
			}
		}
	}
	else
	{
		if(nlhs > 1)
		{
			DXTImageArray::ComputeMSE(dxtimagearray1.GetDXTImage(0), dxtimagearray2.GetDXTImage(0), cmse_flags, plhs[0], plhs[1]);
		}
		else
		{
			DXTImageArray::ComputeMSE(dxtimagearray1.GetDXTImage(0), dxtimagearray2.GetDXTImage(0), cmse_flags, plhs[0]);
		}
	}
}

void DXTImageArray::ComputeMSE(DXTImage& dxtimage1, DXTImage& dxtimage2, DWORD cmse_flags, mxArray*& mx_dxtimage_mse, mxArray*& mx_dxtimage_mseV)
{
	int i, j, k;
	mxArray* tmp_mse,* tmp_mseV;
	if(dxtimage1.GetImageCount() != dxtimage2.GetImageCount())
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InputMismatchError", "The source and destination arguments are not the same size.");
	}
	if(dxtimage1.GetImageCount() == 1)
	{
		DXTImageArray::ComputeMSE(dxtimage1.GetImage(0, 0, 0), dxtimage2.GetImage(0, 0, 0), cmse_flags, mx_dxtimage_mse, mx_dxtimage_mseV);
	}
	else
	{
		DirectX::TexMetadata metadata = dxtimage1.GetMetadata();
		size_t depth = metadata.depth;
		mx_dxtimage_mse = mxCreateCellMatrix(max(metadata.arraySize, metadata.depth), metadata.mipLevels);
		mx_dxtimage_mseV = mxCreateCellMatrix(max(metadata.arraySize, metadata.depth), metadata.mipLevels);
		for(i = 0; i < metadata.mipLevels; i++)
		{
			for(j = 0; j < metadata.arraySize; j++)
			{
				for(k = 0; k < depth; k++)
				{
					DXTImageArray::ComputeMSE(dxtimage1.GetImage(i, j, k), dxtimage2.GetImage(i, j, k), cmse_flags, tmp_mse, tmp_mseV);
					mxSetCell(mx_dxtimage_mse,  dxtimage1.ComputeIndexMEX(i, j, k), tmp_mse);
					mxSetCell(mx_dxtimage_mseV, dxtimage1.ComputeIndexMEX(i, j, k), tmp_mseV);
				}
			}
			if(depth > 1)
			{
				depth >>= 1u;
			}
		}
	}
}

void DXTImageArray::ComputeMSE(DXTImage& dxtimage1, DXTImage& dxtimage2, DWORD cmse_flags, mxArray*& mx_dxtimage_mse)
{
	int i, j, k;
	mxArray* tmp_mse;
	if(dxtimage1.GetImageCount() != dxtimage2.GetImageCount())
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InputMismatchError", "The source and destination arguments are not the same size.");
	}
	if(dxtimage1.GetImageCount() == 1)
	{
		DXTImageArray::ComputeMSE(dxtimage1.GetImage(0, 0, 0), dxtimage2.GetImage(0, 0, 0), cmse_flags, mx_dxtimage_mse);
	}
	else
	{
		DirectX::TexMetadata metadata = dxtimage1.GetMetadata();
		size_t depth = metadata.depth;
		mx_dxtimage_mse = mxCreateCellMatrix(max(metadata.arraySize, metadata.depth), metadata.mipLevels);
		for(i = 0; i < metadata.mipLevels; i++)
		{
			for(j = 0; j < metadata.arraySize; j++)
			{
				for(k = 0; k < depth; k++)
				{
					DXTImageArray::ComputeMSE(dxtimage1.GetImage(i, j, k), dxtimage2.GetImage(i, j, k), cmse_flags, tmp_mse);
					mxSetCell(mx_dxtimage_mse,  dxtimage1.ComputeIndexMEX(i, j, k), tmp_mse);
				}
			}
			if(depth > 1)
			{
				depth >>= 1u;
			}
		}
	}
}

void DXTImageArray::ComputeMSE(const DirectX::Image* img1, const DirectX::Image* img2, DWORD cmse_flags, mxArray*& mx_dxtimageslice_mse)
{
	float mse;
	hres = DirectX::ComputeMSE(*img1, *img2, mse, nullptr, cmse_flags);
	if(FAILED(hres))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "ComputeMSEError", "There was an error while computing the mean-squared error.");
	}
	mx_dxtimageslice_mse = mxCreateDoubleScalar((double)mse);
}

void DXTImageArray::ComputeMSE(const DirectX::Image* img1, const DirectX::Image* img2, DWORD cmse_flags, mxArray*& mx_dxtimageslice_mse, mxArray*& mx_dxtimageslice_mseV)
{
	float mse;
	float mseV[4];
	hres = DirectX::ComputeMSE(*img1, *img2, mse, mseV, cmse_flags);
	if(FAILED(hres))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "ComputeMSEError", "There was an error while computing the mean-squared error.");
	}
	mx_dxtimageslice_mse = mxCreateDoubleScalar(mse);
	mx_dxtimageslice_mseV = mxCreateDoubleMatrix(4, 1, mxREAL);
	auto data = (double*)mxGetData(mx_dxtimageslice_mseV);
	data[0] = (double)mseV[0];
	data[1] = (double)mseV[1];
	data[2] = (double)mseV[2];
	data[3] = (double)mseV[3];
}

void DXTImageArray::SaveFile(MEXF_IN)
{
	size_t i;
	std::wstring filename;
	DWORD ctrl_flags = DirectX::DDS_FLAGS_NONE;
	if(nrhs < 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughArgumentsError", "No enough arguments. Please supply a filename.");
	}
	if(nrhs > 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyArgumentsError", "Too many arguments.");
	}
	
	if(nrhs > 1)
	{
		DXTImageArray::ParseFlags(prhs[1], g_ctrlflag_map, ctrl_flags);
	}
	
	if(mxIsChar(prhs[0]))
	{
		DXTImageArray::ParseFilename(prhs[0], filename);
		DXTImage& pre_op = this->GetDXTImage(0);
		hres = DirectX::SaveToDDSFile(pre_op.GetImages(), pre_op.GetImageCount(), pre_op.GetMetadata(), ctrl_flags, filename.c_str());
		if(FAILED(hres))
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "SaveToDDSFileError", "There was an error while saving the DDS file.");
		}
	}
	else if(mxIsCell(prhs[0]))
	{
		
		if(mxGetNumberOfElements(prhs[0]) != this->GetSize())
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InvalidArgumentError", "The number of Dds objects differs from the number of filenames.");
		}
		
		for(i = 0; i < this->_size; i++)
		{
			DXTImageArray::ParseFilename(mxGetCell(prhs[0], i), filename);
			DXTImage& pre_op = this->GetDXTImage(i);
			hres = DirectX::SaveToDDSFile(pre_op.GetImages(), pre_op.GetImageCount(), pre_op.GetMetadata(), ctrl_flags, filename.c_str());
			if(FAILED(hres))
			{
				MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "SaveToDDSFileError", "There was an error while saving the DDS file.");
			}
		}
	}
	else
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InvalidArgumentError", "The filename argument must either be class 'char' or class 'cell'.");
	}
}

void DXTImageArray::ToImage(MEXF_SIG)
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
				mxArray* tmp[2];
				this->_arr[i].ToImage(tmp[0], tmp[1]);
				mxSetCell(plhs[0], i, tmp[0]);
				mxSetCell(plhs[1], i, tmp[1]);
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

void DXTImageArray::ToMatrix(MEXF_SIG)
{
	size_t i;
	bool combine_alpha = false;
	if(nrhs > 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyArgumentsError", "Too many arguments.");
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
				mxArray* tmp[2];
				this->_arr[i].ToMatrix(tmp[0], tmp[1]);
				mxSetCell(plhs[0], i, tmp[0]);
				mxSetCell(plhs[1], i, tmp[1]);
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

void DXTImageArray::ToExport(int nlhs, mxArray* plhs[])
{
	size_t i;
	const char* fieldnames[] = {"Metadata", "Images"};
	mxArray* out = mxCreateStructMatrix(this->_sz_m, this->_sz_n, 2, fieldnames);
	for(i = 0; i < this->_size; i++)
	{
		mxSetField(out, i, "Metadata", this->GetDXTImage(i).ExportMetadata());
		mxSetField(out, i, "Images", this->GetDXTImage(i).ExportImages());
	}
	plhs[0] = out;
}

DXTImageArray::operation DXTImageArray::GetOperation(const mxArray* directive)
{
	DXTImageArray::operation op = NO_OP;
	if(!mxIsChar(directive))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER|MEU_SEVERITY_INTERNAL, "InvalidDirectiveError", "The supplied directive must be of class 'char'.");
	}
	char* directive_str = mxArrayToString(directive);
	auto op_found = g_directive_map.find(directive_str);
	if(op_found != g_directive_map.end())
	{
		op = op_found->second;
	}
	else
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER|MEU_SEVERITY_INTERNAL, "InvalidDirectiveError", "Unrecognized directive '%s'.", directive_str);
	}
	mxFree(directive_str);
	return op;
}
