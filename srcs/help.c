#include "shaderpixel.h"

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

