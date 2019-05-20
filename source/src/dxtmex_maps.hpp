#pragma once

#include "DirectXTex.h"
#include <unordered_map>
#include "dxtmex_dxtimagearray.hpp"
#include "dxtmex_mexerror.hpp"


namespace DXTMEX
{
	/* treat BiMap as const --- allocate statically */
	template <typename T>
	class BiMap
	{
		
		std::unordered_map<T, std::string> _1to2_map;
		std::unordered_map<std::string, T> _2to1_map;
		
	public:
		
		BiMap(const std::initializer_list<std::pair<T, std::string>> &lst)
		{
			for(auto& item : lst)
			{
				_1to2_map[item.first] = item.second;
				_2to1_map[item.second] = item.first;
			}
		}
		
		auto Find(const T& key) -> decltype(_1to2_map.find(key))
		{
			return _1to2_map.find(key);
		}
		
		auto Find(const std::string& key) -> decltype(_2to1_map.find(key))
		{
			return _2to1_map.find(key);
		}
		
		bool IsValid(typename std::unordered_map<T, std::string>::iterator iter)
		{
			return (iter != _1to2_map.end());
		}
		
		bool IsValid(typename std::unordered_map<std::string, T>::iterator iter)
		{
			return (iter != _2to1_map.end());
		}

		std::string FindStringFromID(T fmt)
		{
			auto found = this->Find(fmt);
			if(!this->IsValid(found))
			{
				MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER | MEU_SEVERITY_INTERNAL, "UnexpectedIDError", "An unexpected ID was encountered (ID: %u).", fmt);
			}
			return found->second;
		}

		T FindIDFromString(const std::string& str)
		{
			auto found = this->Find(str);
			if(!this->IsValid(found))
			{
				MEXError::PrintMexError(MEU_FL, MEU_SEVERITY_USER | MEU_SEVERITY_INTERNAL, "UnexpectedStringError", "Unexpected string '%s' was encountered.", str.c_str());
			}
			return found->second;
		}
		
	};
	
	extern BiMap<DXGI_FORMAT> g_format_map;
	extern BiMap<DirectX::TEX_ALPHA_MODE> g_alphamode_map;
	extern BiMap<DXTImage::IMAGE_TYPE> g_imagetype_map;
	extern BiMap<MEXToDXT::COLORSPACE> g_colorspace_map;
	
}

