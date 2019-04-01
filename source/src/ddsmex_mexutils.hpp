#pragma once

namespace DDSMex
{
	namespace MEXUtils
	{
		void ToUpper(mxArray* mx_str);
		
		void ToLower(mxArray* mx_str);
		
		bool CompareString(const mxArray* mx_arr, const char cmpstr[]);
		
		template<class T>
		void SetScalarField(mxArray* mx_struct, mwIndex idx, const char* field, mxClassID classid, T data);
	}
}

