#include <string>

#include "mex.h"
#include "dxtmex_flags.hpp"
#include "dxtmex_maps.hpp"
#include "dxtmex_dxtimagearray.hpp"
#include "dxtmex_mexerror.hpp"
#include "dxtmex_mexutils.hpp"

using namespace DXTMEX;

#include <unordered_map>
static std::unordered_map<std::string, DXTImageArray::operation> g_directive_map
{
	{"READ_DDS",                         DXTImageArray::READ_DDS                        },
	{"READ_HDR",                         DXTImageArray::READ_HDR                        },
	{"READ_TGA",                         DXTImageArray::READ_TGA                        },
	{"WRITE_DDS",                        DXTImageArray::WRITE_DDS                       },
	{"WRITE_HDR",                        DXTImageArray::WRITE_HDR                       },
	{"WRITE_TGA",                        DXTImageArray::WRITE_TGA                       },
	{"READ_DDS_META",                    DXTImageArray::READ_DDS_META                   },
	{"READ_HDR_META",                    DXTImageArray::READ_HDR_META                   },
	{"READ_TGA_META",                    DXTImageArray::READ_TGA_META                   },
	{"IS_DDS",                           DXTImageArray::IS_DDS                          },
	{"IS_HDR",                           DXTImageArray::IS_HDR                          },
	{"IS_TGA",                           DXTImageArray::IS_TGA                          },
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
	{"TO_IMAGE",                         DXTImageArray::TO_IMAGE                        },
	{"TO_MATRIX",                        DXTImageArray::TO_MATRIX                       }
};


DXTImage* DXTImageArray::CopyDXTImageArray()
{
	size_t i;
	auto out = new(std::nothrow) DXTImage[this->GetSize()];
	if(!out)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_INTERNAL|MEU_SEVERITY_SYSTEM, "MemoryAllocationError", "Could not allocate memory for the DXTImage array.");
	}
	else
	{
		for(i = 0; i < this->GetSize(); i++)
		{
			out[i].SetFlags(this->GetDXTImage(i).GetFlags());
			out[i].SetImageType(this->GetDXTImage(i).GetImageType());
		}
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

DXTImage* DXTImageArray::AllocateDXTImageArray(size_t num, DXTImage::IMAGE_TYPE type)
{
	size_t i;
	auto out = new(std::nothrow) DXTImage[num];
	if(!out)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_INTERNAL|MEU_SEVERITY_SYSTEM, "MemoryAllocationError", "Could not allocate memory for the DXTImage array.");
	}
	else
	{
		for(i = 0; i < num; i++)
		{
			out[i].SetImageType(type);
		}
	}
	return out;
}

DXTImage* DXTImageArray::AllocateDXTImageArray(size_t num, DXTImage::IMAGE_TYPE type, DirectX::DDS_FLAGS flags)
{
	size_t i;
	auto out = new(std::nothrow) DXTImage[num];
	if(!out)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_INTERNAL|MEU_SEVERITY_SYSTEM, "MemoryAllocationError", "Could not allocate memory for the DXTImage array.");
	}
	else
	{
		for(i = 0; i < num; i++)
		{
			out[i].SetFlags(flags);
			out[i].SetImageType(type);
		}
	}
	return out;
}

void DXTImageArray::ReadDDS(MEXF_IN)
{
	size_t i;
	DirectX::DDS_FLAGS flags = DirectX::DDS_FLAGS_NONE;
	std::wstring filename;
	
	if(nrhs < 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughInputsError", "Not enough arguments. Please supply a file name.");
	}
	
	const mxArray* mx_filenames = prhs[0];
	
	if(nrhs > 1)
	{
		g_ddsflags.ImportFlags(nrhs - 1, prhs + 1, flags);
	}
	
	if(mxIsCell(mx_filenames))
	{
		this->Initialize(mxGetM(mx_filenames), mxGetN(mx_filenames), DXTImage::DDS, flags);
		for(i = 0; i < this->GetSize(); i++)
		{
			ImportFilename(mxGetCell(mx_filenames, i), filename);
			hres = DirectX::LoadFromDDSFile(filename.c_str(), flags, nullptr, this->GetDXTImage(i));
			if(FAILED(hres))
			{
				MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER | MEU_SEVERITY_HRESULT, "DDSReadError", "There was an error while reading the DDS file.");
			}
		}
	}
	else
	{
		this->Initialize(1, 1, DXTImage::DDS, flags);
		ImportFilename(mx_filenames, filename);
		hres = DirectX::LoadFromDDSFile(filename.c_str(), flags, nullptr, this->GetDXTImage(0));
		if(FAILED(hres))
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER | MEU_SEVERITY_HRESULT, "DDSReadError", "There was an error while reading the DDS file.");
		}
		this->GetDXTImage(0).SetImageType(DXTImage::DDS);
	}
}

void DXTImageArray::ReadHDR(MEXF_IN)
{
	size_t i;
	std::wstring filename;
	
	if(nrhs < 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughInputsError", "Not enough arguments. Please supply a file name.");
	}
	else if(nrhs > 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyInputsError", "Too many arguments.");
	}
	const mxArray* mx_filenames = prhs[0];
	
	if(mxIsCell(mx_filenames))
	{
		this->Initialize(mxGetM(mx_filenames), mxGetN(mx_filenames));
		for(i = 0; i < this->GetSize(); i++)
		{
			ImportFilename(mxGetCell(mx_filenames, i), filename);
			hres = DirectX::LoadFromHDRFile(filename.c_str(), nullptr, this->GetDXTImage(i));
			if(FAILED(hres))
			{
				MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER | MEU_SEVERITY_HRESULT, "HDRReadError", "There was an error while reading the HDR file.");
			}
			this->GetDXTImage(i).SetImageType(DXTImage::HDR);
		}
	}
	else
	{
		this->Initialize(1, 1);
		ImportFilename(mx_filenames, filename);
		hres = DirectX::LoadFromHDRFile(filename.c_str(), nullptr, this->GetDXTImage(0));
		if(FAILED(hres))
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER | MEU_SEVERITY_HRESULT, "HDRReadError", "There was an error while reading the HDR file.");
		}
		this->GetDXTImage(0).SetImageType(DXTImage::HDR);
	}
}

void DXTImageArray::ReadTGA(MEXF_IN)
{
	size_t i;
	std::wstring filename;
	
	if(nrhs < 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughInputsError", "Not enough arguments. Please supply a file name.");
	}
	else if(nrhs > 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyInputsError", "Too many arguments.");
	}
	const mxArray* mx_filenames = prhs[0];
	
	if(mxIsCell(mx_filenames))
	{
		this->Initialize(mxGetM(mx_filenames), mxGetN(mx_filenames));
		for(i = 0; i < this->GetSize(); i++)
		{
			ImportFilename(mxGetCell(mx_filenames, i), filename);
			hres = DirectX::LoadFromTGAFile(filename.c_str(), nullptr, this->GetDXTImage(i));
			if(FAILED(hres))
			{
				MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER | MEU_SEVERITY_HRESULT, "TGAReadError", "There was an error while reading the TGA file.");
			}
			this->GetDXTImage(i).SetImageType(DXTImage::TGA);
		}
	}
	else
	{
		this->Initialize(1, 1);
		ImportFilename(mx_filenames, filename);
		hres = DirectX::LoadFromTGAFile(filename.c_str(), nullptr, this->GetDXTImage(0));
		if(FAILED(hres))
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER | MEU_SEVERITY_HRESULT, "TGAReadError", "There was an error while reading the TGA file.");
		}
		this->GetDXTImage(0).SetImageType(DXTImage::TGA);
	}
}

void DXTImageArray::Import(MEXF_IN)
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
	
	this->Initialize(mxGetM(in_struct), mxGetN(in_struct));
	if(DXTImage::IsDXTImageImport(in_struct))
	{
		for(i = 0; i < this->GetSize(); i++)
		{
			this->SetDXTImage(i, DXTImage(mxGetField(in_struct, i, "Metadata"), mxGetField(in_struct, i, "Images")));
		}
	}
	else if(DXTImage::IsDXTImageSliceImport(in_struct))
	{
		for(i = 0; i < this->GetSize(); i++)
		{
			this->SetDXTImage(i, DXTImage(mxGetField(in_struct, i, "Width"),
			                              mxGetField(in_struct, i, "Height"),
			                              mxGetField(in_struct, i, "RowPitch"),
			                              mxGetField(in_struct, i, "SlicePitch"),
			                              mxGetField(in_struct, i, "Pixels"),
			                              mxGetField(in_struct, i, "FormatID"),
			                              mxGetField(in_struct, i, "FlagsValue")));
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
	       (mxGetField(in, 0, "FlagsValue") != nullptr);
}

void DXTImageArray::ReadDDSMetadata(MEXF_SIG)
{
	std::wstring filename;
	DirectX::TexMetadata metadata = {};
	DirectX::DDS_FLAGS flags = DirectX::DDS_FLAGS_NONE;
	
	if(nrhs < 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughInputsError", "Not enough arguments. Please supply a file name.");
	}
	
	DXTImageArray::ImportFilename(prhs[0], filename);
	
	if(nrhs > 1)
	{
		g_ddsflags.ImportFlags(nrhs - 1, prhs + 1, flags);
	}
	
	hres = DirectX::GetMetadataFromDDSFile(filename.c_str(), flags, metadata);
	if(FAILED(hres))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER | MEU_SEVERITY_HRESULT, "FileReadError", "There was an error while reading the DDS file.");
	}
	
	plhs[0] = DXTImage::ExportMetadata(metadata, DXTImage::DDS);
}

void DXTImageArray::ReadHDRMetadata(MEXF_SIG)
{
	std::wstring filename;
	DirectX::TexMetadata metadata = {};
	
	if(nrhs < 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughInputsError", "Not enough arguments. Please supply a file name.");
	}
	else if(nrhs > 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyInputsError", "Too many arguments.");
	}
	DXTImageArray::ImportFilename(prhs[0], filename);
	
	
	hres = DirectX::GetMetadataFromHDRFile(filename.c_str(), metadata);
	if(FAILED(hres))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER | MEU_SEVERITY_HRESULT, "FileReadError", "There was an error while reading the HDR file.");
	}
	plhs[0] = DXTImage::ExportMetadata(metadata, DXTImage::HDR);
}

void DXTImageArray::ReadTGAMetadata(MEXF_SIG)
{
	std::wstring filename;
	DirectX::TexMetadata metadata = {};
	
	if(nrhs < 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughInputsError", "Not enough arguments. Please supply a file name.");
	}
	else if(nrhs > 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyInputsError", "Too many arguments.");
	}
	DXTImageArray::ImportFilename(prhs[0], filename);
	
	
	hres = DirectX::GetMetadataFromTGAFile(filename.c_str(), metadata);
	if(FAILED(hres))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER | MEU_SEVERITY_HRESULT, "FileReadError", "There was an error while reading the TGA file.");
	}
	plhs[0] = DXTImage::ExportMetadata(metadata, DXTImage::TGA);
}

void DXTImageArray::IsDDS(MEXF_SIG)
{
	std::wstring filename;
	DirectX::TexMetadata metadata = {};
	if(nrhs < 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughInputsError", "Not enough arguments. Please supply a file name.");
	}
	else if(nrhs > 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyInputsError", "Too many arguments.");
	}
	DXTImageArray::ImportFilename(prhs[0], filename);
	hres = DirectX::GetMetadataFromDDSFile(filename.c_str(), DirectX::DDS_FLAGS_NONE, metadata);
	if(hres != E_FAIL && hres != HRESULT_FROM_WIN32(ERROR_INVALID_DATA) && hres != HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER|MEU_SEVERITY_SYSTEM|MEU_SEVERITY_HRESULT, "FileReadError", "There was an error reading the file.");
	}
	plhs[0] = mxCreateLogicalScalar(!FAILED(hres));
}

void DXTImageArray::IsHDR(MEXF_SIG)
{
	std::wstring filename;
	DirectX::TexMetadata metadata = {};
	if(nrhs < 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughInputsError", "Not enough arguments. Please supply a file name.");
	}
	else if(nrhs > 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyInputsError", "Too many arguments.");
	}
	DXTImageArray::ImportFilename(prhs[0], filename);
	hres = DirectX::GetMetadataFromHDRFile(filename.c_str(), metadata);
	if(hres != E_FAIL && hres != HRESULT_FROM_WIN32(ERROR_INVALID_DATA) && hres != HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER|MEU_SEVERITY_SYSTEM|MEU_SEVERITY_HRESULT, "FileReadError", "There was an error reading the file.");
	}
	plhs[0] = mxCreateLogicalScalar(!FAILED(hres));
}

void DXTImageArray::IsTGA(MEXF_SIG)
{
	std::wstring filename;
	DirectX::TexMetadata metadata = {};
	if(nrhs < 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughInputsError", "Not enough arguments. Please supply a file name.");
	}
	else if(nrhs > 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyInputsError", "Too many arguments.");
	}
	DXTImageArray::ImportFilename(prhs[0], filename);
	hres = DirectX::GetMetadataFromTGAFile(filename.c_str(), metadata);
	if(hres != E_FAIL && hres != HRESULT_FROM_WIN32(ERROR_INVALID_DATA) && hres != HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER|MEU_SEVERITY_SYSTEM|MEU_SEVERITY_HRESULT, "FileReadError", "There was an error reading the file.");
	}
	plhs[0] = mxCreateLogicalScalar(!FAILED(hres));
}

void DXTImageArray::ImportFilename(const mxArray* mx_filename, std::wstring &filename)
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

DXGI_FORMAT DXTImageArray::ParseFormat(const mxArray* mx_fmt)
{
	if(!mxIsChar(mx_fmt))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER | MEU_SEVERITY_INTERNAL, "FormatParsingError", "Format input must be of class 'char'.");
	}
	MEXUtils::ToUpper((mxArray*)mx_fmt);
	char* fmtname = mxArrayToString(mx_fmt);
	DXGI_FORMAT fmt = GetFormatIDFromString(fmtname);
	mxFree(fmtname);
	return fmt;
}

void DXTImageArray::FlipRotate(MEXF_IN)
{
	size_t i;
	DXTImage* new_arr = this->CopyDXTImageArray();
	if(nrhs < 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughArgumentsError", "Not enough arguments. Please supply a flag.");
	}
	else if(nrhs > 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyInputsError", "Too many arguments.");
	}
	
	char* flag = mxArrayToString(prhs[0]);
	DWORD fr_flags = g_frflags.FindFlag(flag);
	mxFree(flag);
	
	for(i = 0; i < this->GetSize(); i++)
	{
		DXTImage& pre_op  = this->GetDXTImage(i);
		DXTImage& post_op = new_arr[i];
		hres = DirectX::FlipRotate(pre_op.GetImages(), pre_op.GetImageCount(), pre_op.GetMetadata(), fr_flags, post_op);
		if(FAILED(hres))
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "FlipRotateError", "There was an error while rotating or flipping the image.");
		}
	}
	this->ReplaceArray(new_arr);
}

void DXTImageArray::Resize(MEXF_IN)
{
	size_t i;
	DXTImage* new_arr = this->CopyDXTImageArray();
	size_t w, h;
	DirectX::TEX_FILTER_FLAGS filter_flags = DirectX::TEX_FILTER_DEFAULT;
	if(nrhs < 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughArgumentsError", "No enough arguments. Please supply a size.");
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
			g_filterflags.ImportFlags(nrhs - 2, prhs + 2, filter_flags);
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
		if(nrhs > 1)
		{
			g_filterflags.ImportFlags(nrhs - 1, prhs + 1, filter_flags);
		}
	}
	
	for(i = 0; i < this->GetSize(); i++)
	{
		DXTImage& pre_op  = this->GetDXTImage(i);
		DXTImage& post_op = new_arr[i];
		hres = DirectX::Resize(pre_op.GetImages(), pre_op.GetImageCount(), pre_op.GetMetadata(), w, h, filter_flags, post_op);
		if(FAILED(hres))
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "ResizeError", "There was an error while resizing the image.");
		}
	}
	this->ReplaceArray(new_arr);
}

void DXTImageArray::Convert(MEXF_IN)
{
	size_t i;
	DXGI_FORMAT fmt;
	DXTImage* new_arr = this->CopyDXTImageArray();
	float threshold = DirectX::TEX_THRESHOLD_DEFAULT;
	DirectX::TEX_FILTER_FLAGS filter_flags = DirectX::TEX_FILTER_DEFAULT;
	if(nrhs < 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughArgumentsError", "Not enough arguments. Please supply a format");
	}
	
	fmt = DXTImageArray::ParseFormat(prhs[0]);
	
	if(nrhs > 1)
	{
		if(mxIsDouble(prhs[1]))
		{
			if(!mxIsScalar(prhs[1]))
			{
				MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InvalidInputError", "The number of levels must be scalar.");
			}
			threshold = (float)mxGetScalar(prhs[1]);
			if(nrhs > 2)
			{
				g_filterflags.ImportFlags(nrhs - 2, prhs + 2, filter_flags);
			}
		}
		else if(mxIsChar(prhs[1]))
		{
			g_filterflags.ImportFlags(nrhs - 1, prhs + 1, filter_flags);
		}
		else
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InvalidInputError", "The first argument must either be of class 'double' or class 'char'.");
		}
	}
	
	for(i = 0; i < this->GetSize(); i++)
	{
		DXTImage& pre_op  = this->GetDXTImage(i);
		DXTImage& post_op = new_arr[i];
		hres = DirectX::Convert(pre_op.GetImages(), pre_op.GetImageCount(), pre_op.GetMetadata(), fmt, filter_flags, threshold, post_op);
		if(FAILED(hres))
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "ConvertError", "There was an error while converting the image.");
		}
	}
	this->ReplaceArray(new_arr);
}

void DXTImageArray::ConvertToSinglePlane(MEXF_IN)
{
	size_t i;
	DXTImage* new_arr = this->CopyDXTImageArray();
	if(nrhs > 0)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyArgumentsError", "Too many arguments.");
	}
	
	for(i = 0; i < this->GetSize(); i++)
	{
		DXTImage& pre_op  = this->GetDXTImage(i);
		DXTImage& post_op = new_arr[i];
		hres = DirectX::ConvertToSinglePlane(pre_op.GetImages(), pre_op.GetImageCount(), pre_op.GetMetadata(), post_op);
		if(FAILED(hres))
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "ConvertToSinglePlaneError", "There was an error while converting the image to a single plane.");
		}
	}
	this->ReplaceArray(new_arr);
}

void DXTImageArray::GenerateMipMaps(MEXF_IN)
{
	size_t i;
	DXTImage* new_arr = this->CopyDXTImageArray();
	size_t levels = 0;
	DirectX::TEX_FILTER_FLAGS filter_flags = DirectX::TEX_FILTER_DEFAULT;
	
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
				g_filterflags.ImportFlags(nrhs - 1, prhs + 1, filter_flags);
			}
		}
		else if(mxIsChar(prhs[0]))
		{
			g_filterflags.ImportFlags(nrhs, prhs, filter_flags);
		}
		else
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InvalidInputError", "The first argument must either be of class 'double' or class 'char'.");
		}
	}
	
	for(i = 0; i < this->GetSize(); i++)
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
	this->ReplaceArray(new_arr);
}

void DXTImageArray::ScaleMipMapsAlphaForCoverage(MEXF_IN)
{
	size_t i, j;
	DXTImage* new_arr = this->CopyDXTImageArray();
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
	
	for(i = 0; i < this->GetSize(); i++)
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
	this->ReplaceArray(new_arr);
}

void DXTImageArray::PremultiplyAlpha(MEXF_IN)
{
	size_t i;
	DXTImage* new_arr = this->CopyDXTImageArray();
	DirectX::TEX_PMALPHA_FLAGS pmalpha_flags = DirectX::TEX_PMALPHA_DEFAULT;
	g_pmflags.ImportFlags(nrhs, prhs, pmalpha_flags);
	
	for(i = 0; i < this->GetSize(); i++)
	{
		DXTImage& pre_op  = this->GetDXTImage(i);
		DXTImage& post_op = new_arr[i];
		hres = DirectX::PremultiplyAlpha(pre_op.GetImages(), pre_op.GetImageCount(), pre_op.GetMetadata(), pmalpha_flags, post_op);
		if(FAILED(hres))
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "PremultiplyAlphaError", "There was an error while premultiplying the alpha of the image.");
		}
	}
	this->ReplaceArray(new_arr);
}

void DXTImageArray::Compress(MEXF_IN)
{
	size_t i;
	DXGI_FORMAT fmt;
	DXTImage* new_arr = this->CopyDXTImageArray();
	DirectX::TEX_COMPRESS_FLAGS compress_flags = DirectX::TEX_COMPRESS_DEFAULT;
	float threshold = DirectX::TEX_THRESHOLD_DEFAULT;
	if(nrhs < 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughArgumentsError", "No enough arguments. Please supply a format.");
	}
	
	fmt = DXTImageArray::ParseFormat(prhs[0]);
	
	if(nrhs > 1)
	{
		if(mxIsDouble(prhs[1]))
		{
			if(!mxIsScalar(prhs[1]))
			{
				MEXError::PrintMexError(MEU_FL,
				                        MEU_SEVERITY_USER,
				                        "InvalidInputError",
				                        "The alpha threshold must be scalar.");
			}
			threshold = (float)mxGetScalar(prhs[1]);
			if(nrhs > 2)
			{
				g_compressflags.ImportFlags(nrhs - 2, prhs + 2, compress_flags);
			}
		}
		else if(mxIsChar(prhs[1]))
		{
			g_compressflags.ImportFlags(nrhs - 1, prhs + 1, compress_flags);
		}
		else
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InvalidInputError", "The optional second argument must either be of class 'double' or class 'char'.");
		}
	}
	
	for(i = 0; i < this->GetSize(); i++)
	{
		DXTImage& pre_op  = this->GetDXTImage(i);
		DXTImage& post_op = new_arr[i];
		hres = DirectX::Compress(pre_op.GetImages(), pre_op.GetImageCount(), pre_op.GetMetadata(), fmt, compress_flags, threshold, post_op);
		if(FAILED(hres))
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "CompressError", "There was an error while compressing the image.");
		}
	}
	this->ReplaceArray(new_arr);
}

void DXTImageArray::Decompress(MEXF_IN)
{
	size_t i;
	DXTImage* new_arr = this->CopyDXTImageArray();
	DXGI_FORMAT fmt = DXGI_FORMAT_UNKNOWN;
	if(nrhs > 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyArgumentsError", "Too many arguments.");
	}
	
	if(nrhs == 1)
	{
		fmt = DXTImageArray::ParseFormat(prhs[0]);
	}
	
	for(i = 0; i < this->GetSize(); i++)
	{
		DXTImage& pre_op  = this->GetDXTImage(i);
		DXTImage& post_op = new_arr[i];
		hres = DirectX::Decompress(pre_op.GetImages(), pre_op.GetImageCount(), pre_op.GetMetadata(), fmt, post_op);
		if(FAILED(hres))
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "DecompressError", "There was an error while decompressing the image.");
		}
	}
	this->ReplaceArray(new_arr);
}

void DXTImageArray::ComputeNormalMap(MEXF_IN)
{
	size_t i;
	DXTImage* new_arr = this->CopyDXTImageArray();
	DXGI_FORMAT fmt;
	float amplitude;
	DirectX::CNMAP_FLAGS cn_flags = DirectX::CNMAP_DEFAULT;
	if(nrhs < 2)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughArgumentsError", "No enough arguments. Please supply a format and an amplitude.");
	}
	
	fmt = DXTImageArray::ParseFormat(prhs[0]);
	
	if(!mxIsDouble(prhs[1]) || !mxIsScalar(prhs[1]))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InvalidSizeError", "Amplitude must be a scalar double.");
	}
	amplitude = (float)mxGetScalar(prhs[1]);
	
	if(nrhs > 2)
	{
		g_cnflags.ImportFlags(nrhs - 2, prhs + 2, cn_flags);
	}
	
	for(i = 0; i < this->GetSize(); i++)
	{
		DXTImage& pre_op  = this->GetDXTImage(i);
		DXTImage& post_op = new_arr[i];
		hres = DirectX::ComputeNormalMap(pre_op.GetImages(), pre_op.GetImageCount(), pre_op.GetMetadata(), cn_flags, amplitude, fmt, post_op);
		if(FAILED(hres))
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "ComputeNormalMapError", "There was an error while computing the normal map.");
		}
	}
	this->ReplaceArray(new_arr);
}

void DXTImageArray::CopyRectangle(DXTImageArray& dst, DXTImageArray& src, MEXF_IN)
{
	size_t i, j;
	DirectX::TEX_FILTER_FLAGS filter_flags = DirectX::TEX_FILTER_DEFAULT;
	size_t out_x;
	size_t out_y;
	if(nrhs < 2)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughArgumentsError", "No enough arguments. Please supply a 4-element vector specifying the rectangle to copy, and "
															"a 2-element vector specifying its destination coordinates.");
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
	data = (double*)mxGetData(prhs[1]);
	out_x = (size_t)data[1];
	out_y = (size_t)data[0];
	
	if(nrhs > 2)
	{
		g_filterflags.ImportFlags(nrhs - 2, prhs + 2, filter_flags);
	}
	
	if(src.GetSize() == 1)
	{
		DXTImage& src_dxtimage  = src.GetDXTImage(0);
		auto src_slices = src_dxtimage.GetImages();
		if(src_dxtimage.GetImageCount() == 1)
		{
			for(i = 0; i < dst.GetSize(); i++)
			{
				DXTImage& dst_dxtimage = dst.GetDXTImage(i);
				auto dst_slices = dst.GetDXTImage(i).GetImages();
				for(j = 0; j < dst_dxtimage.GetImageCount(); j++)
				{
					hres = DirectX::CopyRectangle(*src_slices, rect, *(dst_slices + j), filter_flags, out_x, out_y);
					if(FAILED(hres))
					{
						MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "CopyRectangleError", "There was an error while copying over the rectangle.");
					}
				}
			}
		}
		else
		{
			for(i = 0; i < dst.GetSize(); i++)
			{
				DXTImage& dst_dxtimage = dst.GetDXTImage(i);
				if(src_dxtimage.GetImageCount() != dst_dxtimage.GetImageCount())
				{
					MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InputMismatchError", "The source and destination arguments are not the same size.");
				}
				auto dst_slices = dst_dxtimage.GetImages();
				for(j = 0; j < src_dxtimage.GetImageCount(); j++)
				{
					hres = DirectX::CopyRectangle(*(src_slices + j), rect, *(dst_slices + j), filter_flags, out_x, out_y);
					if(FAILED(hres))
					{
						MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "CopyRectangleError", "There was an error while copying over the rectangle.");
					}
				}
			}
		}
	}
	else
	{
		if(src.GetSize() != dst.GetSize())
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InputMismatchError", "The source and destination arguments are not the same size.");
		}
		
		for(i = 0; i < src.GetSize(); i++)
		{
			DXTImage& src_dxtimage  = src.GetDXTImage(i);
			DXTImage& dst_dxtimage = dst.GetDXTImage(i);
			
			if(src_dxtimage.GetImageCount() != dst_dxtimage.GetImageCount())
			{
				MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InputMismatchError", "The source and destination arguments are not the same size.");
			}
			
			auto src_slices = src_dxtimage.GetImages();
			auto dst_slices = dst_dxtimage.GetImages();
			for(j = 0; j < src_dxtimage.GetImageCount(); j++)
			{
				hres = DirectX::CopyRectangle(*(src_slices + j), rect, *(dst_slices + j), filter_flags, out_x, out_y);
				if(FAILED(hres))
				{
					MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "CopyRectangleError", "There was an error while copying over the rectangle.");
				}
			}
		}
	}
}

void DXTImageArray::ComputeMSE(DXTImageArray& dxtimagearray1, DXTImageArray& dxtimagearray2, MEXF_SIG)
{
	size_t i;
	DirectX::CMSE_FLAGS cmse_flags = DirectX::CMSE_DEFAULT;
	g_cmseflags.ImportFlags(nrhs, prhs, cmse_flags);
	
	if(dxtimagearray1.GetSize() != dxtimagearray2.GetSize())
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InputMismatchError", "The comparison arguments are not the same size.");
	}
	
	if(dxtimagearray1.GetSize() == 1)
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

void DXTImageArray::ComputeMSE(DXTImage& dxtimage1, DXTImage& dxtimage2, DirectX::CMSE_FLAGS cmse_flags, mxArray*& mx_dxtimage_mse, mxArray*& mx_dxtimage_mseV)
{
	size_t i, j, k;
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
		mx_dxtimage_mse = mxCreateCellMatrix(MAX(metadata.arraySize, metadata.depth), metadata.mipLevels);
		mx_dxtimage_mseV = mxCreateCellMatrix(MAX(metadata.arraySize, metadata.depth), metadata.mipLevels);
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

void DXTImageArray::ComputeMSE(DXTImage& dxtimage1, DXTImage& dxtimage2, DirectX::CMSE_FLAGS cmse_flags, mxArray*& mx_dxtimage_mse)
{
	size_t i, j, k;
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
		mx_dxtimage_mse = mxCreateCellMatrix(MAX(metadata.arraySize, metadata.depth), metadata.mipLevels);
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

void DXTImageArray::ComputeMSE(const DirectX::Image* img1, const DirectX::Image* img2, DirectX::CMSE_FLAGS cmse_flags, mxArray*& mx_dxtimageslice_mse)
{
	float mse;
	hres = DirectX::ComputeMSE(*img1, *img2, mse, nullptr, cmse_flags);
	if(FAILED(hres))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "ComputeMSEError", "There was an error while computing the mean-squared error.");
	}
	mx_dxtimageslice_mse = mxCreateDoubleScalar((double)mse);
}

void DXTImageArray::ComputeMSE(const DirectX::Image* img1, const DirectX::Image* img2, DirectX::CMSE_FLAGS cmse_flags, mxArray*& mx_dxtimageslice_mse, mxArray*& mx_dxtimageslice_mseV)
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

void DXTImageArray::WriteDDS(MEXF_IN)
{
	size_t i;
	std::wstring filename;
	DirectX::DDS_FLAGS ctrl_flags = DirectX::DDS_FLAGS_NONE;
	if(nrhs < 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughArgumentsError", "No enough arguments. Please supply a filename.");
	}
	
	if(nrhs > 1)
	{
		g_ddsflags.ImportFlags(nrhs - 1, prhs + 1, ctrl_flags);
	}
	
	if(mxIsChar(prhs[0]))
	{
		DXTImageArray::ImportFilename(prhs[0], filename);
		if(this->GetSize() > 1)
		{
			/* if filename has an extension put the numbers before the extension */
			std::wstring ext;
			size_t dot_pos = filename.find_last_of('.');
			if(dot_pos != std::string::npos)
			{
				filename = filename.substr(0, dot_pos);
				ext      = filename.substr(dot_pos + 1);
			}
			for(i = 0; i < this->GetSize(); i++)
			{
				std::wstring out_fn = filename + std::to_wstring(i).append(ext);
				DXTImage& pre_op = this->GetDXTImage(i);
				hres = DirectX::SaveToDDSFile(pre_op.GetImages(), pre_op.GetImageCount(), pre_op.GetMetadata(), ctrl_flags, out_fn.c_str());
				if(FAILED(hres))
				{
					MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "SaveToDDSFileError", "There was an error while saving the DDS file.");
				}
			}
		}
		else
		{
			DXTImage& dxt_image = this->GetDXTImage(0);
			hres = DirectX::SaveToDDSFile(dxt_image.GetImages(), dxt_image.GetImageCount(), dxt_image.GetMetadata(), ctrl_flags, filename.c_str());
			if(FAILED(hres))
			{
				MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "SaveToDDSFileError", "There was an error while saving the DDS file.");
			}
		}
	}
	else if(mxIsCell(prhs[0]))
	{
		if(mxGetNumberOfElements(prhs[0]) != this->GetSize())
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InvalidArgumentError", "The number of DXTImage objects differs from the number of filenames.");
		}
		
		for(i = 0; i < this->GetSize(); i++)
		{
			DXTImageArray::ImportFilename(mxGetCell(prhs[0], i), filename);
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

void DXTImageArray::WriteHDR(MEXF_IN)
{
	size_t i, j, k, l;
	bool remove_idx_if_singular = false;
	std::wstring filename;
	std::wstring ext;
	if(nrhs < 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughArgumentsError", "No enough arguments. Please supply a filename.");
	}
	
	if(nrhs == 2)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyArgumentsError", "Too many arguments.");
	}
	
	if(nrhs > 3)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyArgumentsError", "Too many arguments.");
	}
	
	if(nrhs == 3)
	{
		if(!mxIsChar(prhs[1]))
		{
			MEXError::PrintMexError(MEU_FL,
			                        MEU_SEVERITY_USER,
			                        "InvalidKeyError",
			                        "All keys must be class 'char'.");
		}
		if(!mxIsLogicalScalar(prhs[2]))
		{
			MEXError::PrintMexError(MEU_FL,
			                        MEU_SEVERITY_USER,
			                        "InvalidValueError",
			                        "All flag values must be scalar class 'logical'.");
		}
		MEXUtils::ToUpper((mxArray*)prhs[1]);
		char* flagname = mxArrayToString(prhs[1]);
		if(strcmp(flagname, "RemoveIndexIfSingular") == 0)
		{
			remove_idx_if_singular = mxIsLogicalScalarTrue(prhs[1]);
		}
	}
	
	if(mxIsChar(prhs[0]))
	{
		DXTImageArray::ImportFilename(prhs[0], filename);
		/* if filename has an extension put the numbers before the extension */
		size_t dot_pos = filename.find_last_of('.');
		if(dot_pos != std::string::npos)
		{
			ext      = filename.substr(dot_pos + 1);
			filename = filename.erase(dot_pos);
		}
		if(this->GetSize() > 1 || !remove_idx_if_singular)
		{
			for(i = 0; i < this->GetSize(); i++)
			{
				std::wstring out_fn = filename + std::to_wstring(i).append(L"_");
				this->GetDXTImage(i).WriteHDR(out_fn, ext);
			}
		}
		else
		{
			this->GetDXTImage(0).WriteHDR(filename, ext, remove_idx_if_singular);
		}
	}
	else if(mxIsCell(prhs[0]))
	{
		if(mxGetNumberOfElements(prhs[0]) != this->GetSize())
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InvalidArgumentError", "The number of DXTImageSlice objects differs from the number of filenames.");
		}
		
		for(i = 0; i < this->GetSize(); i++)
		{
			const mxArray* mx_filename = mxGetCell(prhs[0], i);
			if(mxIsChar(mx_filename))
			{
				this->GetDXTImage(i).WriteHDR(filename, ext, remove_idx_if_singular);
			}
			else
			{
				DXTImage& dxt_image = this->GetDXTImage(i);
				if(mxGetNumberOfElements(mx_filename) != dxt_image.GetImageCount())
				{
					MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InvalidArgumentError", "The number of DXTImageSlice objects differs from the number of filenames.");
				}
				
				const DirectX::TexMetadata& metadata = dxt_image.GetMetadata();
				size_t depth = metadata.depth;
				for(j = 0; j < metadata.mipLevels; j++)
				{
					for(k = 0; k < metadata.arraySize; k++)
					{
						for(l = 0; l < depth; l++)
						{
							DXTImageArray::ImportFilename(mxGetCell(
							mx_filename,
							dxt_image.ComputeIndexMEX(j, k, l)),
							                              filename);
							dxt_image.WriteHDR(filename, j, k, l);
						}
					}
					if (depth > 1)
					{
						depth >>= 1u;
					}
				}
			}
		}
	}
	else
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InvalidArgumentError", "The filename argument must either be class 'char' or class 'cell'.");
	}
}

void DXTImageArray::WriteTGA(MEXF_IN)
{
	size_t i, j, k, l;
	bool remove_idx_if_singular = false;
	std::wstring filename;
	std::wstring ext;
	if(nrhs < 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "NotEnoughArgumentsError", "No enough arguments. Please supply a filename.");
	}
	if(nrhs > 2)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyArgumentsError", "Too many arguments.");
	}
	
	if(nrhs > 1)
	{
		remove_idx_if_singular = mxIsLogicalScalarTrue(prhs[1]);
	}
	
	if(mxIsChar(prhs[0]))
	{
		DXTImageArray::ImportFilename(prhs[0], filename);
		/* if filename has an extension put the numbers before the extension */
		size_t dot_pos = filename.find_last_of('.');
		if(dot_pos != std::string::npos)
		{
			ext      = filename.substr(dot_pos + 1);
			filename = filename.erase(dot_pos);
		}
		if(this->GetSize() > 1 || !remove_idx_if_singular)
		{
			for(i = 0; i < this->GetSize(); i++)
			{
				std::wstring out_fn = filename + std::to_wstring(i).append(L"_");
				this->GetDXTImage(i).WriteTGA(out_fn, ext);
			}
		}
		else
		{
			this->GetDXTImage(0).WriteTGA(filename, ext, remove_idx_if_singular);
		}
	}
	else if(mxIsCell(prhs[0]))
	{
		if(mxGetNumberOfElements(prhs[0]) != this->GetSize())
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InvalidArgumentError", "The number of DXTImageSlice objects differs from the number of filenames.");
		}
		
		for(i = 0; i < this->GetSize(); i++)
		{
			const mxArray* mx_filename = mxGetCell(prhs[0], i);
			if(mxIsChar(mx_filename))
			{
				this->GetDXTImage(i).WriteTGA(filename, ext, remove_idx_if_singular);
			}
			else
			{
				DXTImage& dxt_image = this->GetDXTImage(i);
				if(mxGetNumberOfElements(mx_filename) != dxt_image.GetImageCount())
				{
					MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "InvalidArgumentError", "The number of DXTImageSlice objects differs from the number of filenames.");
				}
				
				const DirectX::TexMetadata& metadata = dxt_image.GetMetadata();
				size_t depth = metadata.depth;
				for(j = 0; j < metadata.mipLevels; j++)
				{
					for(k = 0; k < metadata.arraySize; k++)
					{
						for(l = 0; l < depth; l++)
						{
							DXTImageArray::ImportFilename(mxGetCell(
							mx_filename,
							dxt_image.ComputeIndexMEX(j, k, l)),
							                              filename);
							dxt_image.WriteTGA(filename, j, k, l);
						}
					}
					if (depth > 1)
					{
						depth >>= 1u;
					}
				}
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
	if(this->GetSize() == 1)
	{
		if(nlhs > 1)
		{
			this->GetDXTImage(0).ToImage(plhs[0], plhs[1]);
		}
		else
		{
			this->GetDXTImage(0).ToImage(plhs[0]);
		}
	}
	else
	{
		if(nlhs > 1)
		{
			plhs[0] = mxCreateCellMatrix(this->GetM(), this->GetN());
			plhs[1] = mxCreateCellMatrix(this->GetM(), this->GetN());
			for(i = 0; i < this->GetSize(); i++)
			{
				mxArray* tmp[2];
				this->GetDXTImage(i).ToImage(tmp[0], tmp[1]);
				mxSetCell(plhs[0], i, tmp[0]);
				mxSetCell(plhs[1], i, tmp[1]);
			}
		}
		else
		{
			plhs[0] = mxCreateCellMatrix(this->GetM(), this->GetN());
			for(i = 0; i < this->GetSize(); i++)
			{
				mxArray* tmp;
				this->GetDXTImage(i).ToImage(tmp);
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
	
	if(this->GetSize() == 1)
	{
		if(nlhs > 1)
		{
			this->GetDXTImage(0).ToMatrix(plhs[0], plhs[1]);
		}
		else
		{
			this->GetDXTImage(0).ToMatrix(plhs[0], combine_alpha);
		}
	}
	else
	{
		if(nlhs > 1)
		{
			plhs[0] = mxCreateCellMatrix(this->GetM(), this->GetN());
			plhs[1] = mxCreateCellMatrix(this->GetM(), this->GetN());
			for(i = 0; i < this->GetSize(); i++)
			{
				mxArray* tmp[2];
				this->GetDXTImage(i).ToMatrix(tmp[0], tmp[1]);
				mxSetCell(plhs[0], i, tmp[0]);
				mxSetCell(plhs[1], i, tmp[1]);
			}
			
			
		}
		else
		{
			plhs[0] = mxCreateCellMatrix(this->GetM(), this->GetN());
			for(i = 0; i < this->GetSize(); i++)
			{
				mxArray* tmp;
				this->GetDXTImage(i).ToMatrix(tmp, combine_alpha);
				mxSetCell(plhs[0], i, tmp);
			}
		}
		
	}
}

void DXTImageArray::ToExport(MEXF_OUT)
{
	size_t i;
	const char* fieldnames[] = {"Metadata", "Images"};
	mxArray* out = mxCreateStructMatrix(this->GetM(), this->GetN(), ARRAYSIZE(fieldnames), fieldnames);
	for(i = 0; i < this->GetSize(); i++)
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
