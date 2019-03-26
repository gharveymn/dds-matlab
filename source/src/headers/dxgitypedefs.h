#ifndef DDS_MATLAB_DXGITYPEDEFS_H
#define DDS_MATLAB_DXGITYPEDEFS_H

#include "mex.h"
#include <cstdint>

template <class T0, class T1, class T2, class T3>
class Pixel128_4
{
private:
	uint8_t data[128];
public:
	T0 getRed()
	{
		return (T0)*data;
	}
	T1 getGreen()
	{
		return (T1)*(data + sizeof(T0));
	}
	T2 getBlue()
	{
		return (T2)*(data + sizeof(T0) + sizeof(T1));
	}
	T3 getAlpha()
	{
		return (T3)*(data + sizeof(T0) + sizeof(T1) + sizeof(T2));
	}
	mxArray* get
};



#endif /* DDS_MATLAB_DXGITYPEDEFS_H */
