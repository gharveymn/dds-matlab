#pragma once

#include "DirectXTex.h"
#include <unordered_map>
#include "dxtmex_dxtimagearray.hpp"
#include "dxtmex_mexerror.hpp"

namespace DXTMEX
{
	/* treat BiMap as const --- allocate statically */
	template <typename T, typename S>
	class BiMap
	{
		
		std::unordered_map<T,S> _1to2_map;
		std::unordered_map<S,T> _2to1_map;
		
	public:
		
		BiMap(const std::initializer_list<std::pair<T,S>> &lst)
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
		
		auto Find(const S& key) -> decltype(_2to1_map.find(key))
		{
			return _2to1_map.find(key);
		}
		
		bool IsValid(typename std::unordered_map<T,S>::iterator iter)
		{
			return (iter != _1to2_map.end());
		}
		
		bool IsValid(typename std::unordered_map<S,T>::iterator iter)
		{
			return (iter != _2to1_map.end());
		}
		
		/*
		decltype(_1to2_map.end()) end_1to2()
		{
			return _1to2_map.end();
		}
		
		
		std::string &operator[](const size_t &key)
		{
			return _1to2_map[key];
		}
		
		decltype(_2to1_map.end()) end_2to1()
		{
			return _2to1_map.end();
		}
		
		
		size_t &operator[](const std::string &key)
		{
			return _2to1_map[key];
		}
		 */
		
	};
	
	extern BiMap<DXGI_FORMAT, std::string> g_format_map;
	extern BiMap<DirectX::TEX_ALPHA_MODE, std::string> g_alphamode_map;
	extern BiMap<DXTImage::IMAGE_TYPE, std::string> g_imagetype_map;
	
}

