#include "vox.h"
#include "chunk.h"

/*
 * name of file you save into is the seed;
*/
void	save(t_world *info)
{
	FILE	*fp;

	fp = fopen(SAVE_PATH, "wb");
}