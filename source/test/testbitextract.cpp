#include <stdio.h>
#include <stdint.h>

template <typename T, size_t N>
inline T signextend(uint32_t x)
{
  struct {T x:N;} s;
  return s.x = x;
}

float SNormToFloat(const size_t ch_size, uint32_t val)
{
	uint32_t MSB = (uint32_t)1 << (ch_size-1);
	if(val == MSB)
	{
		return -1.0f;
	}
	//int32_t s_val = (val & MSB)? val | ~(((uint32_t)1 << (ch_size))-1) : val;
	int32_t s_val = signextend<int32_t, 8>(val);
	float c = s_val;
	return c * (1.0f / (MSB - 1));
}

int main(int argc, char* argv[])
{
	const size_t ch_sz = 8;
	size_t mod_bit_offset = 16;
	uint32_t pixel = 0b00000000100000010000000000000000;
	uint32_t mask = (((uint32_t)1 << ch_sz) - (uint32_t)1) << mod_bit_offset;
	uint32_t ext = (pixel & mask) >> mod_bit_offset;
	float flt = SNormToFloat(ch_sz, ext);
	printf("%f\n", flt);
}
