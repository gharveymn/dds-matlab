#pragma once

#include "directxtex.h"
#include <unordered_map>

namespace DXTMEX
{
	class BiMap
	{
	private:
		std::unordered_map<size_t, std::string> i2s;
		std::unordered_map<std::string, size_t> s2i;
	public:
		
		BiMap() = default;
		
		BiMap(const std::initializer_list<std::pair<size_t, std::string>> &lst)
		{
			for(auto &item : lst)
			{
				i2s[item.first] = item.second;
				s2i[item.second] = item.first;
			}
		}
		
		
		void insert(size_t item1, std::string& item2)
		{
			i2s[item1] = item2;
			s2i[item2] = item1;
		}
		
		
		auto find(const size_t &key)
		{
			return i2s.find(key);
		}
		
		
		auto i_end()
		{
			return i2s.end();
		}
		
		
		std::string &operator[](const size_t &key)
		{
			return i2s[key];
		}
		
		
		auto find(const std::string &key)
		{
			return s2i.find(key);
		}
		
		
		auto s_end()
		{
			return s2i.end();
		}
		
		
		size_t &operator[](const std::string &key)
		{
			return s2i[key];
		}
		
	};
	
	extern BiMap g_format_map;
	extern BiMap g_ctrlflag_map;
	extern BiMap g_filterflag_map;
	extern BiMap g_frflag_map;
	extern BiMap g_pmflag_map;
	extern BiMap g_compressflag_map;
	extern BiMap g_cnflag_map;
	extern BiMap g_cmseflag_map;
}

