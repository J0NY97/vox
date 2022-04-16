#include "shaderpixel.h"

void	water_placer(t_chunk_info *info, float *world_pos)
{
	set_block_at_world_pos(info, world_pos, BLOCK_WATER);
}