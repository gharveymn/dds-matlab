#include <string>

#include "mex.h"
#include "dxtmex_flags.hpp"
#include "dxtmex_maps.hpp"
#include "dxtmex_dxtimagearray.hpp"
#include "dxtmex_mexerror.hpp"
#include "dxtmex_mexutils.hpp"

#ifdef min
#  undef min
#endif

#ifdef max
#  undef max
#endif

using namespace DXTMEX;

#include <unordered_map>
static std::unordered_map<std::string, DXTImageArray::OPERATION> g_directive_map
{
	{"READ_DDS",                         DXTImageArray::OPERATION::READ_DDS                        },
	{"READ_HDR",                         DXTImageArray::OPERATION::READ_HDR                        },
	{"READ_TGA",                         DXTImageArray::OPERATION::READ_TGA                        },
	{"WRITE_DDS",                        DXTImageArray::OPERATION::WRITE_DDS                       },
	{"WRITE_HDR",                        DXTImageArray::OPERATION::WRITE_HDR                       },
	{"WRITE_TGA",                        DXTImageArray::OPERATION::WRITE_TGA                       },
	{"WRITE_MATRIX_DDS",                 DXTImageArray::OPERATION::WRITE_MATRIX_DDS                },
	{"WRITE_MATRIX_HDR",                 DXTImageArray::OPERATION::WRITE_MATRIX_HDR                },
	{"WRITE_MATRIX_TGA",                 DXTImageArray::OPERATION::WRITE_MATRIX_TGA                },
	{"READ_DDS_META",                    DXTImageArray::OPERATION::READ_DDS_META                   },
	{"READ_HDR_META",                    DXTImageArray::OPERATION::READ_HDR_META                   },
	{"READ_TGA_META",                    DXTImageArray::OPERATION::READ_TGA_META                   },
	{"IS_DDS",                           DXTImageArray::OPERATION::IS_DDS                          },
	{"IS_HDR",                           DXTImageArray::OPERATION::IS_HDR                          },
	{"IS_TGA",                           DXTImageArray::OPERATION::IS_TGA                          },
	{"FLIP_ROTATE",                      DXTImageArray::OPERATION::FLIP_ROTATE                     },
	{"RESIZE",                           DXTImageArray::OPERATION::RESIZE                          },
	{"CONVERT",                          DXTImageArray::OPERATION::CONVERT                         },
	{"CONVERT_TO_SINGLE_PLANE",          DXTImageArray::OPERATION::CONVERT_TO_SINGLE_PLANE         },
	{"GENERATE_MIPMAPS",                 DXTImageArray::OPERATION::GENERATE_MIPMAPS                },
	{"GENERATE_MIPMAPS_3D",              DXTImageArray::OPERATION::GENERATE_MIPMAPS_3D             },
	{"SCALE_MIPMAPS_ALPHA_FOR_COVERAGE", DXTImageArray::OPERATION::SCALE_MIPMAPS_ALPHA_FOR_COVERAGE},
	{"PREMULTIPLY_ALPHA",                DXTImageArray::OPERATION::PREMULTIPLY_ALPHA               },
	{"COMPRESS",                         DXTImageArray::OPERATION::COMPRESS                        },
	{"DECOMPRESS",                       DXTImageArray::OPERATION::DECOMPRESS                      },
	{"COPY_RECTANGLE",                   DXTImageArray::OPERATION::COPY_RECTANGLE                  },
	{"COMPUTE_NORMAL_MAP",               DXTImageArray::OPERATION::COMPUTE_NORMAL_MAP              },
	{"COMPUTE_MSE",                      DXTImageArray::OPERATION::COMPUTE_MSE                     },
	{"TO_IMAGE",                         DXTImageArray::OPERATION::TO_IMAGE                        },
	{"TO_MATRIX",                        DXTImageArray::OPERATION::TO_MATRIX                       }
};

void DXTImageArray::WriteMatrixDDS(int nrhs, const mxArray* prhs[])
{
	if(nrhs < 3)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_INTERNAL, "NumInputsError", "Not enough inputs. Must supply a matrix, a map, and a filename");
	}

	const mxArray* mx_data = prhs[0];
	const mxArray* mx_map = prhs[1];
	const mxArray* mx_filename = prhs[2];

	if(!mxIsEmpty(mx_map))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_INTERNAL, "IndexedImageError", "Indexed images are not currently supported.");
	}

	/* parse options */
	int num_opts = nrhs - 3;
	const mxArray** mx_opts = prhs + 3;

	/* Initial Conversion
	 * input colorspace
	 * alpha mode
	 * is_cubemap
	 * cp_flags
	 * 
	 * Final Conversion:
	 * fmt
	 * filter_flags
	 * threshold
	 * 
	 * Saving:
	 * dds_flags
	 */

	MEXToDXT::COLORSPACE input_colorspace = MEXToDXT::COLORSPACE::DEFAULT;
	DirectX::TEX_ALPHA_MODE alpha_mode = DirectX::TEX_ALPHA_MODE_UNKNOWN;
	bool is_cubemap = false;
	DirectX::CP_FLAGS cp_flags = DirectX::CP_FLAGS_NONE;

	DXGI_FORMAT fmt = DXGI_FORMAT_UNKNOWN;
	DirectX::TEX_FILTER_FLAGS filter_flags = DirectX::TEX_FILTER_DEFAULT;
	float threshold = DirectX::TEX_THRESHOLD_DEFAULT;

	DirectX::DDS_FLAGS dds_flags = DirectX::DDS_FLAGS_NONE;

	if((num_opts % 2) != 0)
	{
		MEXError::PrintMexError(MEU_FL,
			MEU_SEVERITY_USER,
			"KeyValueError",
			"Invalid number of arguments. A key is likely missing a value.");
	}

	for(int i = 0; i < num_opts; i+=2)
	{
		const mxArray* mx_curr_key = mx_opts[i];
		const mxArray* mx_curr_val = mx_opts[i + 1];
		if(!mxIsChar(mx_curr_key))
		{
			MEXError::PrintMexError(MEU_FL,
				MEU_SEVERITY_USER,
				"InvalidKeyError",
				"All keys must be class 'char'.");
		}
		MEXUtils::ToUpper(const_cast<mxArray*>(mx_curr_key));
		char* keyname = mxArrayToString(mx_curr_key);
		if(strcmp(keyname, "OUTPUTFORMAT") == 0)
		{
			if(!mxIsChar(mx_curr_val))
			{
				MEXError::PrintMexError(MEU_FL,
					MEU_SEVERITY_USER,
					"InvalidValueError",
					"OutputFormat value must be class 'char'");
			}
			char* val = mxArrayToString(mx_curr_val);
			fmt = g_format_map.FindIDFromString(val);
			mxFree(val);
		}
		else if(strcmp(keyname, "INPUTCOLORSPACE") == 0)
		{
			if(!mxIsChar(mx_curr_val))
			{
				MEXError::PrintMexError(MEU_FL,
					MEU_SEVERITY_USER,
					"InvalidValueError",
					"InputColorspace value must be class 'char'");
			}
			char* val = mxArrayToString(mx_curr_val);
			input_colorspace = g_colorspace_map.FindIDFromString(val);
			mxFree(val);
		}
		else if(strcmp(keyname, "ALPHAMODE") == 0)
		{
			if(!mxIsChar(mx_curr_val))
			{
				MEXError::PrintMexError(MEU_FL,
					MEU_SEVERITY_USER,
					"InvalidValueError",
					"AlphaMode value must be class 'char'");
			}
			char* val = mxArrayToString(mx_curr_val);
			alpha_mode = g_alphamode_map.FindIDFromString(val);
			mxFree(val);
		}
		else if(strcmp(keyname, "CUBEMAP") == 0)
		{
			if(!mxIsLogicalScalar(mx_curr_val))
			{
				MEXError::PrintMexError(MEU_FL,
					MEU_SEVERITY_USER,
					"InvalidValueError",
					"Cubemap value must be class 'logical'");
			}
			is_cubemap = mxIsLogicalScalarTrue(mx_curr_val);
		}
		else if(strcmp(keyname, "CPFLAGS") == 0)
		{
			g_cpflags.ImportFlags(mx_curr_val, cp_flags);
		}
		else if(strcmp(keyname, "FILTERFLAGS") == 0)
		{
			g_filterflags.ImportFlags(mx_curr_val, filter_flags);
		}
		else if(strcmp(keyname, "THRESHOLD") == 0)
		{
			if(!mxIsScalar(mx_curr_val))
			{
				MEXError::PrintMexError(MEU_FL,
					MEU_SEVERITY_USER,
					"InvalidValueError",
					"Threshold value must be scalar");
			}
			threshold = static_cast<float>(mxGetScalar(mx_curr_val));
		}
		else if(strcmp(keyname, "DDSFLAGS") == 0)
		{
			g_ddsflags.ImportFlags(mx_curr_val, dds_flags);
		}
		else
		{
			MEXError::PrintMexError(MEU_FL,
				MEU_SEVERITY_USER,
				"UnexpectedKeyError",
				"Unexpected key '%s'", keyname);
		}
	}
	
	DirectX::ScratchImage sc_img;
	MEXToDXT::ConvertToOutput(fmt, filter_flags, threshold, sc_img, mx_data, input_colorspace, alpha_mode, is_cubemap, cp_flags);

	std::wstring filename;
	ImportFilename(mx_filename, filename);

	hres = DirectX::SaveToDDSFile(sc_img.GetImages(), sc_img.GetImageCount(), sc_img.GetMetadata(), dds_flags, filename.c_str());
	if(FAILED(hres))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "SaveToDDSFileError", "There was an error while saving the DDS file.");
	}

}


void DXTImageArray::WriteMatrixHDR(int nrhs, const mxArray* prhs[])
{
	if(nrhs < 3)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_INTERNAL, "NumInputsError", "Not enough inputs. Must supply a matrix, a map, and a filename");
	}

	const mxArray* mx_data = prhs[0];
	const mxArray* mx_map = prhs[1];
	const mxArray* mx_filename = prhs[2];

	if(!mxIsEmpty(mx_map))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_INTERNAL, "IndexedImageError", "Indexed images are not currently supported.");
	}

	if(mxIsCell(mx_data))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "ImportError", "Image data must be a matrix.");
	}

	/* parse options */
	int num_opts = nrhs - 3;
	const mxArray** mx_opts = prhs + 3;

	/* Initial Conversion
	 * input colorspace
	 * alpha mode
	 * is_cubemap
	 * cp_flags
	 *
	 * Final Conversion:
	 * fmt
	 * filter_flags
	 * threshold
	 *
	 * Saving:
	 * dds_flags
	 */

	MEXToDXT::COLORSPACE input_colorspace = MEXToDXT::COLORSPACE::DEFAULT;
	DirectX::TEX_ALPHA_MODE alpha_mode = DirectX::TEX_ALPHA_MODE_UNKNOWN;
	bool is_cubemap = false;
	DirectX::CP_FLAGS cp_flags = DirectX::CP_FLAGS_NONE;

	DXGI_FORMAT fmt = DXGI_FORMAT_UNKNOWN;
	DirectX::TEX_FILTER_FLAGS filter_flags = DirectX::TEX_FILTER_DEFAULT;
	float threshold = DirectX::TEX_THRESHOLD_DEFAULT;

	if((num_opts % 2) != 0)
	{
		MEXError::PrintMexError(MEU_FL,
			MEU_SEVERITY_USER,
			"KeyValueError",
			"Invalid number of arguments. A key is likely missing a value.");
	}

	for(int i = 0; i < num_opts; i += 2)
	{
		const mxArray* mx_curr_key = mx_opts[i];
		const mxArray* mx_curr_val = mx_opts[i + 1];
		if(!mxIsChar(mx_curr_key))
		{
			MEXError::PrintMexError(MEU_FL,
				MEU_SEVERITY_USER,
				"InvalidKeyError",
				"All keys must be class 'char'.");
		}
		MEXUtils::ToUpper(const_cast<mxArray*>(mx_curr_key));
		char* keyname = mxArrayToString(mx_curr_key);
		if(strcmp(keyname, "OUTPUTFORMAT") == 0)
		{
			if(!mxIsChar(mx_curr_val))
			{
				MEXError::PrintMexError(MEU_FL,
					MEU_SEVERITY_USER,
					"InvalidValueError",
					"OutputFormat value must be class 'char'");
			}
			char* val = mxArrayToString(mx_curr_val);
			fmt = g_format_map.FindIDFromString(val);
			mxFree(val);
		}
		else if(strcmp(keyname, "INPUTCOLORSPACE") == 0)
		{
			if(!mxIsChar(mx_curr_val))
			{
				MEXError::PrintMexError(MEU_FL,
					MEU_SEVERITY_USER,
					"InvalidValueError",
					"InputColorspace value must be class 'char'");
			}
			char* val = mxArrayToString(mx_curr_val);
			input_colorspace = g_colorspace_map.FindIDFromString(val);
			mxFree(val);
		}
		else if(strcmp(keyname, "ALPHAMODE") == 0)
		{
			if(!mxIsChar(mx_curr_val))
			{
				MEXError::PrintMexError(MEU_FL,
					MEU_SEVERITY_USER,
					"InvalidValueError",
					"AlphaMode value must be class 'char'");
			}
			char* val = mxArrayToString(mx_curr_val);
			alpha_mode = g_alphamode_map.FindIDFromString(val);
			mxFree(val);
		}
		else if(strcmp(keyname, "CUBEMAP") == 0)
		{
			if(!mxIsLogicalScalar(mx_curr_val))
			{
				MEXError::PrintMexError(MEU_FL,
					MEU_SEVERITY_USER,
					"InvalidValueError",
					"Cubemap value must be class 'logical'");
			}
			is_cubemap = mxIsLogicalScalarTrue(mx_curr_val);
		}
		else if(strcmp(keyname, "CPFLAGS") == 0)
		{
			g_cpflags.ImportFlags(mx_curr_val, cp_flags);
		}
		else if(strcmp(keyname, "FILTERFLAGS") == 0)
		{
			g_filterflags.ImportFlags(mx_curr_val, filter_flags);
		}
		else if(strcmp(keyname, "THRESHOLD") == 0)
		{
			if(!mxIsScalar(mx_curr_val))
			{
				MEXError::PrintMexError(MEU_FL,
					MEU_SEVERITY_USER,
					"InvalidValueError",
					"Threshold value must be scalar");
			}
			threshold = static_cast<float>(mxGetScalar(mx_curr_val));
		}
		else
		{
			MEXError::PrintMexError(MEU_FL,
				MEU_SEVERITY_USER,
				"UnexpectedKeyError",
				"Unexpected key '%s'", keyname);
		}
	}

	DirectX::ScratchImage sc_img;
	MEXToDXT::ConvertToOutput(fmt, filter_flags, threshold, sc_img, mx_data, input_colorspace, alpha_mode, is_cubemap, cp_flags);

	if(sc_img.GetImageCount() != 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "ImportError", "Output data must contain only a single image.");
	}

	std::wstring filename;
	ImportFilename(mx_filename, filename);

	hres = DirectX::SaveToHDRFile(*sc_img.GetImage(0, 0, 0), filename.c_str());
	if(FAILED(hres))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "SaveToHDRFileError", "There was an error while saving the HDR file.");
	}

}


void DXTImageArray::WriteMatrixTGA(int nrhs, const mxArray* prhs[])
{
	if(nrhs < 3)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_INTERNAL, "NumInputsError", "Not enough inputs. Must supply a matrix, a map, and a filename");
	}

	const mxArray* mx_data = prhs[0];
	const mxArray* mx_map = prhs[1];
	const mxArray* mx_filename = prhs[2];

	if(!mxIsEmpty(mx_map))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_INTERNAL, "IndexedImageError", "Indexed images are not currently supported.");
	}

	if(mxIsCell(mx_data))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "ImportError", "Image data must be a matrix.");
	}

	/* parse options */
	int num_opts = nrhs - 3;
	const mxArray** mx_opts = prhs + 3;

	/* Initial Conversion
	 * input colorspace
	 * alpha mode
	 * is_cubemap
	 * cp_flags
	 *
	 * Final Conversion:
	 * fmt
	 * filter_flags
	 * threshold
	 *
	 * Saving:
	 * dds_flags
	 */

	MEXToDXT::COLORSPACE input_colorspace = MEXToDXT::COLORSPACE::DEFAULT;
	DirectX::TEX_ALPHA_MODE alpha_mode = DirectX::TEX_ALPHA_MODE_UNKNOWN;
	bool is_cubemap = false;
	DirectX::CP_FLAGS cp_flags = DirectX::CP_FLAGS_NONE;

	DXGI_FORMAT fmt = DXGI_FORMAT_UNKNOWN;
	DirectX::TEX_FILTER_FLAGS filter_flags = DirectX::TEX_FILTER_DEFAULT;
	float threshold = DirectX::TEX_THRESHOLD_DEFAULT;

	if((num_opts % 2) != 0)
	{
		MEXError::PrintMexError(MEU_FL,
			MEU_SEVERITY_USER,
			"KeyValueError",
			"Invalid number of arguments. A key is likely missing a value.");
	}

	for(int i = 0; i < num_opts; i += 2)
	{
		const mxArray* mx_curr_key = mx_opts[i];
		const mxArray* mx_curr_val = mx_opts[i + 1];
		if(!mxIsChar(mx_curr_key))
		{
			MEXError::PrintMexError(MEU_FL,
				MEU_SEVERITY_USER,
				"InvalidKeyError",
				"All keys must be class 'char'.");
		}
		MEXUtils::ToUpper(const_cast<mxArray*>(mx_curr_key));
		char* keyname = mxArrayToString(mx_curr_key);
		if(strcmp(keyname, "OUTPUTFORMAT") == 0)
		{
			if(!mxIsChar(mx_curr_val))
			{
				MEXError::PrintMexError(MEU_FL,
					MEU_SEVERITY_USER,
					"InvalidValueError",
					"OutputFormat value must be class 'char'");
			}
			char* val = mxArrayToString(mx_curr_val);
			fmt = g_format_map.FindIDFromString(val);
			mxFree(val);
		}
		else if(strcmp(keyname, "INPUTCOLORSPACE") == 0)
		{
			if(!mxIsChar(mx_curr_val))
			{
				MEXError::PrintMexError(MEU_FL,
					MEU_SEVERITY_USER,
					"InvalidValueError",
					"InputColorspace value must be class 'char'");
			}
			char* val = mxArrayToString(mx_curr_val);
			input_colorspace = g_colorspace_map.FindIDFromString(val);
			mxFree(val);
		}
		else if(strcmp(keyname, "ALPHAMODE") == 0)
		{
			if(!mxIsChar(mx_curr_val))
			{
				MEXError::PrintMexError(MEU_FL,
					MEU_SEVERITY_USER,
					"InvalidValueError",
					"AlphaMode value must be class 'char'");
			}
			char* val = mxArrayToString(mx_curr_val);
			alpha_mode = g_alphamode_map.FindIDFromString(val);
			mxFree(val);
		}
		else if(strcmp(keyname, "CUBEMAP") == 0)
		{
			if(!mxIsLogicalScalar(mx_curr_val))
			{
				MEXError::PrintMexError(MEU_FL,
					MEU_SEVERITY_USER,
					"InvalidValueError",
					"Cubemap value must be class 'logical'");
			}
			is_cubemap = mxIsLogicalScalarTrue(mx_curr_val);
		}
		else if(strcmp(keyname, "CPFLAGS") == 0)
		{
			g_cpflags.ImportFlags(mx_curr_val, cp_flags);
		}
		else if(strcmp(keyname, "FILTERFLAGS") == 0)
		{
			g_filterflags.ImportFlags(mx_curr_val, filter_flags);
		}
		else if(strcmp(keyname, "THRESHOLD") == 0)
		{
			if(!mxIsScalar(mx_curr_val))
			{
				MEXError::PrintMexError(MEU_FL,
					MEU_SEVERITY_USER,
					"InvalidValueError",
					"Threshold value must be scalar");
			}
			threshold = static_cast<float>(mxGetScalar(mx_curr_val));
		}
		else
		{
			MEXError::PrintMexError(MEU_FL,
				MEU_SEVERITY_USER,
				"UnexpectedKeyError",
				"Unexpected key '%s'", keyname);
		}
	}

	DirectX::ScratchImage sc_img;
	MEXToDXT::ConvertToOutput(fmt, filter_flags, threshold, sc_img, mx_data, input_colorspace, alpha_mode, is_cubemap, cp_flags);

	if(sc_img.GetImageCount() != 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "ImportError", "Output data must contain only a single image.");
	}

	std::wstring filename;
	ImportFilename(mx_filename, filename);

	hres = DirectX::SaveToTGAFile(*sc_img.GetImage(0, 0, 0), filename.c_str());
	if(FAILED(hres))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "SaveToTGAFileError", "There was an error while saving the TGA file.");
	}

}


std::unique_ptr<DXTImage[]> DXTImageArray::CopyDXTImageArray()
{
	std::unique_ptr<DXTImage[]> out = std::make_unique<DXTImage[]>(this->_size);
	for(size_t i = 0; i < this->_size; i++)
	{
		out[i].SetImageType(this->_arr[i].GetImageType());
		out[i].SetFlags(this->_arr[i].GetFlags());
	}
	return out;
}

void DXTImageArray::ReadDDS(int nrhs, const mxArray* prhs[])
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
		this->Initialize(mxGetM(mx_filenames), mxGetN(mx_filenames), DXTImage::IMAGE_TYPE::DDS, flags);
		for(i = 0; i < this->GetSize(); i++)
		{
			ImportFilename(mxGetCell(mx_filenames, i), filename);
			hres = DirectX::LoadFromDDSFile(filename.c_str(), flags, nullptr, this->GetDXTImage(i));
			if(FAILED(hres))
			{
				MEXError::PrintMexError(MEU_FL,
				                        MEU_SEVERITY_USER | MEU_SEVERITY_HRESULT,
				                        "DDSReadError",
				                        "There was an error while reading the DDS file.\n"
				                        "Filename: \"%s\"",
				                        mxArrayToString(mxGetCell(mx_filenames, i)));
			}
		}
	}
	else
	{
		this->Initialize(1, 1, DXTImage::IMAGE_TYPE::DDS, flags);
		ImportFilename(mx_filenames, filename);
		hres = DirectX::LoadFromDDSFile(filename.c_str(), flags, nullptr, this->GetDXTImage(0));
		if(FAILED(hres))
		{
			MEXError::PrintMexError(MEU_FL,
			                        MEU_SEVERITY_USER | MEU_SEVERITY_HRESULT,
			                        "DDSReadError",
			                        "There was an error while reading the DDS file.\n"
					              "Filename: \"%s\"",
					              mxArrayToString(mx_filenames));
		}
		this->GetDXTImage(0).SetImageType(DXTImage::IMAGE_TYPE::DDS);
	}
}

void DXTImageArray::ReadHDR(int nrhs, const mxArray* prhs[])
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
				MEXError::PrintMexError(MEU_FL,
				                        MEU_SEVERITY_USER | MEU_SEVERITY_HRESULT,
				                        "HDRReadError",
				                        "There was an error while reading the HDR file.\n"
				                        "Filename: \"%s\"",
				                        mxArrayToString(mxGetCell(mx_filenames, i)));
			}
			this->GetDXTImage(i).SetImageType(DXTImage::IMAGE_TYPE::HDR);
		}
	}
	else
	{
		this->Initialize(1, 1);
		ImportFilename(mx_filenames, filename);
		hres = DirectX::LoadFromHDRFile(filename.c_str(), nullptr, this->GetDXTImage(0));
		if(FAILED(hres))
		{
			MEXError::PrintMexError(MEU_FL,
			                        MEU_SEVERITY_USER | MEU_SEVERITY_HRESULT,
			                        "HDRReadError",
			                        "There was an error while reading the HDR file.\n"
			                        "Filename: \"%s\"",
			                        mxArrayToString(mx_filenames));
		}
		this->GetDXTImage(0).SetImageType(DXTImage::IMAGE_TYPE::HDR);
	}
}

void DXTImageArray::ReadTGA(int nrhs, const mxArray* prhs[])
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
				MEXError::PrintMexError(MEU_FL,
				                        MEU_SEVERITY_USER | MEU_SEVERITY_HRESULT,
				                        "TGAReadError",
				                        "There was an error while reading the TGA file.\n"
				                        "Filename: \"%s\"",
				                        mxArrayToString(mxGetCell(mx_filenames, i)));
			}
			this->GetDXTImage(i).SetImageType(DXTImage::IMAGE_TYPE::TGA);
		}
	}
	else
	{
		this->Initialize(1, 1);
		ImportFilename(mx_filenames, filename);
		hres = DirectX::LoadFromTGAFile(filename.c_str(), nullptr, this->GetDXTImage(0));
		if(FAILED(hres))
		{
			MEXError::PrintMexError(MEU_FL,
			                        MEU_SEVERITY_USER | MEU_SEVERITY_HRESULT,
			                        "TGAReadError",
			                        "There was an error while reading the TGA file.\n"
			                        "Filename: \"%s\"",
			                        mxArrayToString(mx_filenames));
		}
		this->GetDXTImage(0).SetImageType(DXTImage::IMAGE_TYPE::TGA);
	}
}

void DXTImageArray::Import(int nrhs, const mxArray* prhs[])
{
	size_t i;
	if(nrhs < 1)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER|MEU_SEVERITY_INTERNAL, "NotEnoughInputsError", "Not enough arguments. Please supply a DXTImage object.");
	}
	
	const mxArray* in_data = prhs[0];

	if(!mxIsStruct(in_data))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER | MEU_SEVERITY_INTERNAL, "InvalidImportError", "Import object must be class 'struct'.");
	}

	if(mxIsEmpty(in_data))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER | MEU_SEVERITY_INTERNAL, "InvalidImportError", "Import object must not be empty.");
	}

	this->Initialize(mxGetM(in_data), mxGetN(in_data));
	if(DXTImage::IsDXTImageImport(in_data))
	{
		for(i = 0; i < this->GetSize(); i++)
		{
			this->SetDXTImage(i, DXTImage(mxGetField(in_data, i, "Metadata"), mxGetField(in_data, i, "Images")));
		}
	}
	else if(DXTImage::IsDXTImageSliceImport(in_data))
	{
		for(i = 0; i < this->GetSize(); i++)
		{
			this->SetDXTImage(i, DXTImage(mxGetField(in_data, i, "Width"),
									mxGetField(in_data, i, "Height"),
									mxGetField(in_data, i, "RowPitch"),
									mxGetField(in_data, i, "SlicePitch"),
									mxGetField(in_data, i, "Pixels"),
									mxGetField(in_data, i, "FormatID"),
									mxGetField(in_data, i, "FlagsValue")));
		}
	}
	else
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER | MEU_SEVERITY_INTERNAL, "InvalidImportError", "The import object was invalid.");
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

void DXTImageArray::ReadDDSMetadata(int, mxArray *plhs[], int nrhs, const mxArray* prhs[])
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
	
	plhs[0] = DXTImage::ExportMetadata(metadata, DXTImage::IMAGE_TYPE::DDS);
}

void DXTImageArray::ReadHDRMetadata(int, mxArray *plhs[], int nrhs, const mxArray* prhs[])
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
	plhs[0] = DXTImage::ExportMetadata(metadata, DXTImage::IMAGE_TYPE::HDR);
}

void DXTImageArray::ReadTGAMetadata(int, mxArray *plhs[], int nrhs, const mxArray* prhs[])
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
	plhs[0] = DXTImage::ExportMetadata(metadata, DXTImage::IMAGE_TYPE::TGA);
}

void DXTImageArray::IsDDS(int, mxArray *plhs[], int nrhs, const mxArray* prhs[])
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
	if(FAILED(hres) && hres != E_FAIL && hres != HRESULT_FROM_WIN32(ERROR_INVALID_DATA) && hres != HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER|MEU_SEVERITY_SYSTEM|MEU_SEVERITY_HRESULT, "FileReadError", "There was an error reading the file.");
	}
	plhs[0] = mxCreateLogicalScalar(!FAILED(hres));
}

void DXTImageArray::IsHDR(int, mxArray *plhs[], int nrhs, const mxArray* prhs[])
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
	if(FAILED(hres) && hres != E_FAIL && hres != HRESULT_FROM_WIN32(ERROR_INVALID_DATA) && hres != HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED))
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER|MEU_SEVERITY_SYSTEM|MEU_SEVERITY_HRESULT, "FileReadError", "There was an error reading the file.");
	}
	plhs[0] = mxCreateLogicalScalar(!FAILED(hres));
}

void DXTImageArray::IsTGA(int, mxArray *plhs[], int nrhs, const mxArray* prhs[])
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
	if(FAILED(hres) && hres != E_FAIL && hres != HRESULT_FROM_WIN32(ERROR_INVALID_DATA) && hres != HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED))
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
	DXGI_FORMAT fmt = g_format_map.FindIDFromString(fmtname);
	mxFree(fmtname);
	return fmt;
}

void DXTImageArray::FlipRotate(int nrhs, const mxArray* prhs[])
{
	size_t i;
	std::unique_ptr<DXTImage[]> new_arr = this->CopyDXTImageArray();
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
		hres = DirectX::FlipRotate(pre_op.GetImages(), pre_op.GetImageCount(), pre_op.GetMetadata(), fr_flags, new_arr[i]);
		if(FAILED(hres))
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_HRESULT, "FlipRotateError", "There was an error while rotating or flipping the image.");
		}
	}
	this->_arr = std::move(new_arr);
}

void DXTImageArray::Resize(int nrhs, const mxArray* prhs[])
{
	size_t i;
	std::unique_ptr<DXTImage[]> new_arr = this->CopyDXTImageArray();
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
	this->_arr = std::move(new_arr);
}

void DXTImageArray::Convert(int nrhs, const mxArray* prhs[])
{
	size_t i;
	DXGI_FORMAT fmt;
	std::unique_ptr<DXTImage[]> new_arr = this->CopyDXTImageArray();
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
	this->_arr = std::move(new_arr);
}

void DXTImageArray::ConvertToSinglePlane(int nrhs, const mxArray* [])
{
	size_t i;
	std::unique_ptr<DXTImage[]> new_arr = this->CopyDXTImageArray();
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
	this->_arr = std::move(new_arr);
}

void DXTImageArray::GenerateMipMaps(int nrhs, const mxArray* prhs[])
{
	size_t i;
	std::unique_ptr<DXTImage[]> new_arr = this->CopyDXTImageArray();
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
	this->_arr = std::move(new_arr);
}

void DXTImageArray::ScaleMipMapsAlphaForCoverage(int nrhs, const mxArray* prhs[])
{
	size_t i, j;
	std::unique_ptr<DXTImage[]> new_arr = this->CopyDXTImageArray();
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
	this->_arr = std::move(new_arr);
}

void DXTImageArray::PremultiplyAlpha(int nrhs, const mxArray* prhs[])
{
	size_t i;
	std::unique_ptr<DXTImage[]> new_arr = this->CopyDXTImageArray();
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
	this->_arr = std::move(new_arr);
}

void DXTImageArray::Compress(int nrhs, const mxArray* prhs[])
{
	size_t i;
	DXGI_FORMAT fmt;
	std::unique_ptr<DXTImage[]> new_arr = this->CopyDXTImageArray();
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
	this->_arr = std::move(new_arr);
}

void DXTImageArray::Decompress(int nrhs, const mxArray* prhs[])
{
	size_t i;
	std::unique_ptr<DXTImage[]> new_arr = this->CopyDXTImageArray();
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
	this->_arr = std::move(new_arr);
}

void DXTImageArray::ComputeNormalMap(int nrhs, const mxArray* prhs[])
{
	size_t i;
	std::unique_ptr<DXTImage[]> new_arr = this->CopyDXTImageArray();
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
	this->_arr = std::move(new_arr);
}

void DXTImageArray::CopyRectangle(DXTImageArray& dst, DXTImageArray& src, int nrhs, const mxArray* prhs[])
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

void DXTImageArray::ComputeMSE(DXTImageArray& dxtimagearray1, DXTImageArray& dxtimagearray2, int nlhs, mxArray *plhs[], int nrhs, const mxArray* prhs[])
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
		mx_dxtimage_mse = mxCreateCellMatrix(std::max(metadata.arraySize, metadata.depth), metadata.mipLevels);
		mx_dxtimage_mseV = mxCreateCellMatrix(std::max(metadata.arraySize, metadata.depth), metadata.mipLevels);
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
		mx_dxtimage_mse = mxCreateCellMatrix(std::max(metadata.arraySize, metadata.depth), metadata.mipLevels);
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

void DXTImageArray::WriteDDS(int nrhs, const mxArray* prhs[])
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

void DXTImageArray::WriteHDR(int nrhs, const mxArray* prhs[])
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

void DXTImageArray::WriteTGA(int nrhs, const mxArray* prhs[])
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

void DXTImageArray::ToImage(int nlhs, mxArray *plhs[], int nrhs, const mxArray* prhs[])
{
	size_t i;
	bool combine_alpha = false;
	if(nrhs > 2)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyArgumentsError", "Too many arguments.");
	}
	
	if(nrhs > 0)
	{
		if(!mxIsChar(prhs[0]))
		{
			MEXError::PrintMexError(MEU_FL,
			                        MEU_SEVERITY_USER,
			                        "InvalidKeyError",
			                        "All keys must be class 'char'.");
		}
		
		if(nrhs == 1)
		{
			MEXError::PrintMexError(MEU_FL,
			                        MEU_SEVERITY_USER,
			                        "KeyValueError",
			                        "Invalid number of arguments. The key '%s' is missing a value.", mxArrayToString(prhs[0]));
		}
		
		if(!mxIsLogicalScalar(prhs[1]))
		{
			MEXError::PrintMexError(MEU_FL,
			                        MEU_SEVERITY_USER,
			                        "InvalidValueError",
			                        "All flag values must be scalar class 'logical'.");
		}
		
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
			this->GetDXTImage(0).ToImage(plhs[0], plhs[1]);
		}
		else
		{
			this->GetDXTImage(0).ToImage(plhs[0], combine_alpha);
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
				this->GetDXTImage(i).ToImage(tmp, combine_alpha);
				mxSetCell(plhs[0], i, tmp);
			}
			
		}
	}
}

void DXTImageArray::ToMatrix(int nlhs, mxArray *plhs[], int nrhs, const mxArray* prhs[])
{
	size_t i;
	bool combine_alpha = false;
	if(nrhs > 2)
	{
		MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "TooManyArgumentsError", "Too many arguments.");
	}
	
	if(nrhs > 0)
	{
		if(!mxIsChar(prhs[0]))
		{
			MEXError::PrintMexError(MEU_FL,
			                        MEU_SEVERITY_USER,
			                        "InvalidKeyError",
			                        "All keys must be class 'char'.");
		}
		
		if(nrhs != 2)
		{
			MEXError::PrintMexError(MEU_FL,
			                        MEU_SEVERITY_USER,
			                        "KeyValueError",
			                        "Invalid number of arguments. The key '%s' is missing a value.", mxArrayToString(prhs[0]));
		}
		
		if(!mxIsLogicalScalar(prhs[1]))
		{
			MEXError::PrintMexError(MEU_FL,
			                        MEU_SEVERITY_USER,
			                        "InvalidValueError",
			                        "All flag values must be scalar class 'logical'.");
		}
		
		MEXUtils::ToUpper((mxArray*)prhs[0]);
		if(MEXUtils::CompareMEXString(prhs[0], "COMBINEALPHA"))
		{
			if(nlhs > 1)
			{
				MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "MatrixOptionError", "The 'CombineAlpha' option requires either 0 or 1 outputs.");
			}
			combine_alpha = mxIsLogicalScalarTrue(prhs[1]);
		}
		else
		{
			MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "MatrixOptionError", "Unrecognized option '%s'.", mxArrayToString(prhs[0]));
		}
	}
	
	if(this->GetSize() == 1)
	{
		this->GetDXTImage(0).ToMatrix(plhs[0]);
	}
	else
	{
		plhs[0] = mxCreateCellMatrix(this->GetM(), this->GetN());
		for(i = 0; i < this->GetSize(); i++)
		{
			mxArray* tmp;
			this->GetDXTImage(i).ToMatrix(tmp);
			mxSetCell(plhs[0], i, tmp);
		}
	}
}

void DXTImageArray::ToExport(int, mxArray *plhs[])
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

DXTImageArray::OPERATION DXTImageArray::GetOperation(const mxArray* directive)
{
	DXTImageArray::OPERATION op = OPERATION::NO_OP;
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
