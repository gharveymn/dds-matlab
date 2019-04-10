#include <stdint.h>
#include <stdio.h>

int main(int argc, char* argv[])
{

	uint8_t pixels[] = 
	{
		0b00001100,
		0b00110011,
		0b11111000,
		0b00111110,
		0b01100010,
		0b11110000,
		0b01010011,

		0b00111010,
		0b10100011,
		0b11011000,
		0b10110110,
		0b01010010,
		0b11010010,
		0b01000011,		
	};

	size_t num_ch = 1;

	size_t width  = 54;
	size_t height = 2;
	size_t pitch = (width * 1 + 7u) / 8u;
     size_t slice = pitch * height;

	size_t num_pixels = width * height;

	void* data = malloc(width * height * num_ch * sizeof(uint8_t));

	uint8_t* data_l = (uint8_t*)data;
	size_t i, j, k, l;
	size_t px_idx = 0;
	for(i = 0; i < height; i++)
	{
		/* use 32-bit uints because DWORD size is fastest on x86-64 and i686 */
		uint32_t* pixels32 = (uint32_t*)(pixels + i*pitch);
		for(j = 0; j < pitch / 4; j++)
		{
			uint32_t page = pixels32[j];
			for(k = 32; k > 0; k--, px_idx++)
			{
				/* there should only be one channel, but it may be repeatedly stored */
				uint32_t channel_data = (page >> (k - 1)) & 1;
				for(l = 0; l < num_ch; l++)
				{
					size_t dst_idx = px_idx / width + (px_idx % width) * height + l * num_pixels;
					data_l[dst_idx] = channel_data;
				}
			}
		}
		uint8_t* pixels8 = (uint8_t*)pixels32;
		for(j *= 4; j < pitch - 1; j++)
		{
			uint8_t page = pixels8[j];
			for(k = 8; k > 0; k--, px_idx++)
			{
				/* there should only be one channel, but it may be repeatedly stored */
				uint8_t channel_data = (page >> (k - 1)) & 1;
				for(l = 0; l < num_ch; l++)
				{
					size_t dst_idx = px_idx / width + (px_idx % width) * height + l * num_pixels;
					data_l[dst_idx] = channel_data;
				}
			}
		}
		uint8_t page = pixels8[j];
		for(k = width - ((pitch - 1) * 8); k > 0; k--, px_idx++)
		{
			/* there should only be one channel, but it may be repeatedly stored */
			uint8_t channel_data = (page >> (k - 1)) & 1;
			for(l = 0; l < num_ch; l++)
			{
				size_t dst_idx = px_idx / width + (px_idx % width) * height + l * num_pixels;
				data_l[dst_idx] = channel_data;
			}
		}
	}
	for(i = 0; i < height; i++)
	{
		for(j = 0; j < width; j++)
		{
			size_t idx = i + j * height;
			printf("%d", data_l[idx]);
			if(j == width - 1)
			{
				printf("\n");
			}
			else
			{
				printf(" ");
			}
		}
	}
	free(data);
	return(0);
}