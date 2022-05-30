#include "shaderpixel.h"
#include "chunk.h"

/*
 * name of file you save into is the seed;
*/
void	save(t_chunk_info *info)
{
	FILE	*fp;

	fp = fopen(SAVE_PATH, "wb");
}