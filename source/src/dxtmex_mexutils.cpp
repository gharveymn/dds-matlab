#include <cctype>
#include <cstring>

#include "mex.h"
#include "dxtmex_mexutils.hpp"

void DXTMEX::MEXUtils::ToUpper(mxArray* mx_str)
{
	size_t i;
	size_t n = mxGetNumberOfElements(mx_str);
	auto data = (mxChar*)mxGetData(mx_str);
	for(i = 0; i < n; i++)
	{
		*(data + i) = (mxChar)toupper(*(data + i));
	}
}

void DXTMEX::MEXUtils::ToLower(mxArray* mx_str)
{
	size_t i;
	size_t n = mxGetNumberOfElements(mx_str);
	auto data = (mxChar*)mxGetData(mx_str);
	for(i = 0; i < n; i++)
	{
		*(data + i) = (mxChar)tolower(*(data + i));
		
	}
}

bool DXTMEX::MEXUtils::CompareMEXString(const mxArray* mx_arr, const char* cmpstr)
{
	mwIndex i;
	mxChar* mx_str = mxGetChars(mx_arr);
	mwSize mx_strlen = mxGetNumberOfElements(mx_arr);
	if(strlen(cmpstr) != mx_strlen)
	{
		return false;
	}
	for(i = 0; i < mx_strlen; i++)
	{
		if(mx_str[i] != cmpstr[i])
		{
			return false;
		}
	}
	return true;
}

