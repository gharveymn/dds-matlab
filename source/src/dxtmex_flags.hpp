#pragma once

#include <unordered_map>

#include "mex.h"
#include "DirectXTex.h"
#include "dxtmex_mexerror.hpp"


namespace DXTMEX
{
	template <typename T>
	class DXTFlags
	{
		const size_t _size;
		const T* _flags;
		const char** _names;
		std::unordered_map<T, std::string> _toname_map;
		std::unordered_map<std::string, T> _toflag_map;
		
	public:
		DXTFlags(size_t size, const T* flags, const char** names) :
		_size(size),
		_flags(flags),
		_names(names),
		_toname_map(std::unordered_map<T, std::string>(size)),
		_toflag_map(std::unordered_map<std::string, T>(size))
		{
			int i;
			for(i = 0; i < _size; i++)
			{
				_toname_map.emplace(flags[i], names[i]);
				auto tmpname = std::string(names[i]);
				std::transform(tmpname.begin(), tmpname.end(), tmpname.begin(), [](char c){return static_cast<char>(::toupper(c));});
				_toflag_map.emplace(tmpname, flags[i]);
			}
		};
		
		size_t GetNumberOfFlags() {return _size;}
		
		const char** GetNames() {return _names;}
		
		T FindFlag(const std::string &flag_name)
		{
			auto found = _toflag_map.find(flag_name);
			if(found == _toflag_map.end())
			{
				MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "UnexpectedFlagError", "Unexpected flag '%s' was encountered during input parsing.", flag_name.c_str());
			}
			return found->second;
		}
		
		const std::string& FindName(T flag)
		{
			auto found = _toname_map.find(flag);
			if(found == _toname_map.end())
			{
				MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "UnexpectedFlagError", "An unexpected flag (0x%X) was encountered during export.", flag);
				
			}
			return found->second;
		}
		
		std::unordered_map<T, std::string>& GetFlagToNameMap() {return _toname_map;}
		
		std::unordered_map<std::string, T>& GetNameToFlagMap() {return _toflag_map;}

		void ImportFlags(int num_options, const mxArray* mx_options[], T& flags);

		void ImportFlags(const mxArray* mx_flags, T& flags)
		{
			if(!mxIsCell(mx_flags))
			{
				MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER, "OptionError", "Value of flags must be class 'cell'");
			}

			this->ImportFlags(static_cast<int>(mxGetNumberOfElements(mx_flags)), reinterpret_cast<const mxArray * *>(mxGetData(mx_flags)), flags);

		}

		mxArray* ExportFlags(T flags);
		
	};
	
	extern DXTFlags<DirectX::DDS_FLAGS> g_ddsflags;
	extern DXTFlags<DirectX::TEX_FILTER_FLAGS> g_filterflags;
	extern DXTFlags<DirectX::TEX_FR_FLAGS> g_frflags;
	extern DXTFlags<DirectX::TEX_PMALPHA_FLAGS> g_pmflags;
	extern DXTFlags<DirectX::TEX_COMPRESS_FLAGS> g_compressflags;
	extern DXTFlags<DirectX::CNMAP_FLAGS> g_cnflags;
	extern DXTFlags<DirectX::CMSE_FLAGS> g_cmseflags;
	extern DXTFlags<DirectX::CP_FLAGS> g_cpflags;



}
