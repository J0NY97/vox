#include "vox.h"

void	memset_pattern(void *dest, size_t dest_size, void *src, size_t src_size)
{
	size_t	i;

	i = 0;
	while (i < dest_size)
	{
		((char *)dest)[i] = ((char *)src)[i % src_size];
		i++;
	}
}

/*
 * takes in screen coordinates and returns them in a range from -1 to 1;
 * w & h are the screen dimensions;
*/
void	normalized_device_coords(float *v3, float *mouse, int w, int h)
{
	v3[0] = (2.0f * mouse[0]) / w - 1.0f;
	v3[1] = 1.0f - (2.0f * mouse[1]) / h;
	v3[2] = 1.0f;
}
