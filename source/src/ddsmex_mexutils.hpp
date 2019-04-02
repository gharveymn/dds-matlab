#pragma once

namespace DDSMEX
{
	namespace MEXUtils
	{
		void ToUpper(mxArray* mx_str);
		
		void ToLower(mxArray* mx_str);
		
		bool CompareMEXString(const mxArray* mx_arr, const char* cmpstr);
		
		template<class T>
		void SetScalarField(mxArray* mx_struct, mwIndex idx, const char* field, mxClassID classid, T data)
		{
			mxArray* scalar = mxCreateNumericMatrix(1, 1, classid, mxREAL);
			*(T*)mxGetData(scalar) = data;
			mxSetField(mx_struct, idx, field, scalar);
		}
	}
}

